#include <iostream>
#include <cstdint>
#include <memory>

#include <mo>

#include "HashFunction.hpp"
#include "Operator.hpp"
#include "XorLeftShift.hpp"
#include "XorRightShift.hpp"
#include "Multiply.hpp"
#include "AddShift.hpp"
#include "Masking.hpp"
#include "Multiply.hpp"
#include "moCombination.hpp"
#include "moCombinationNeighbor.hpp"
#include "moCombinationNeighborhood.hpp"
#include "EvalFunc.hpp"
#include "log.h"

using myuint = uint32_t;
using Min = eoMinimizingFitness;
using Combi = moCombination<Min>;
using Nb = moCombinationNeighbor<Combi>;
using NbHood = moCombinationNeighborhood<Combi>;


size_t make_domain(eoForgeVector< EvalFull<myuint,Combi>::OpItf >& forge, size_t value_size)
{
    std::vector<size_t> sizes(value_size);
    std::iota(std::begin(sizes), std::end(sizes), 1); // 1, 2, 3, …, until value_size.

    for(auto size : sizes) { // TODO what shift/mask sizes do we want?
        forge.add< XorLeftShift <myuint> >(size, value_size);
        CLUTCHLOG(xdebug, "XorLeftShift << " << size);

        forge.add< XorRightShift<myuint> >(size, value_size);
        CLUTCHLOG(xdebug, "XorRightShift << " << size);

        forge.add< AddShift     <myuint> >(size, value_size);
        CLUTCHLOG(xdebug, "AddShift << " << size);

        forge.add< Masking      <myuint> >(size);
        CLUTCHLOG(xdebug, "Masking & " << size);
    }

    for(auto size : sizes) { // TODO what multipliers do we want?
        if(size % 2 == 1) { // Only odd multipliers are allowed.
            forge.add< Multiply      <myuint> >(size, value_size);
            CLUTCHLOG(xdebug, "Multiply * " << size);
        }
    }

    return sizes.size();
}

int main()
{

    CLUTCHLOG(progress, "Set config");
    clutchlog_config(); // common config
    auto& log = clutchlog::logger();
    log.threshold("Debug");

    const size_t sol_size = 10;
    const size_t value_size = 31;
    size_t seed = 0;

    CLUTCHLOG(progress, "Create the search domain...");
    eoForgeVector< EvalFull<myuint,Combi>::OpItf > forge(/*always_reinstantiate*/true);
    size_t nb_ops = make_domain(forge, value_size);
    CLUTCHLOG(info, forge.size() << " operators");
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Pick a random solution...");
    std::vector<size_t> v;
    v.reserve(sol_size);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> uni(0, forge.size()-1);
    for(size_t i=0; i<sol_size; ++i) {
        v.push_back(uni(rng));
    }
    Combi sol(v, forge.size());
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Instantiate solver...");
    eo::rng.reseed(seed);
    EvalFull<myuint, Combi> feval(value_size, forge);
    EvalTest<myuint, Combi> peval(feval);

    NbHood neighborhood;

    // Continue search until exhaustion of the neighborhood.
    moTrueContinuator<Nb> until_end;
    moCheckpoint<Nb> check(until_end);
    moBestFitnessStat<Combi> best;
    check.add(best);    // Update the best state.

    // Hill climber, selecting a random solution among the equal-best ones.
    moRandomBestHC<Nb> search(neighborhood, feval, peval, check);
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Evaluate first signature...");
    feval(sol);
    CLUTCHLOG(note, "Initial signature: " << sol);
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Solver run...");
    search(sol);
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Found signature:");
    CLUTCHLOG(note, sol );

    // Real output.
    HashFunction<myuint> hff("optimized_hash", value_size);
    for(size_t i : sol.get()) {
        CLUTCHLOG(xdebug, "Instantiate " << i << "th operator");
        hff.add_operator( forge.instantiate_ptr(i) );
    }
    CLUTCHLOG(debug, "Complete with masks");
    hff.complete_with_masks();

    CLUTCHLOG(debug, "Invert");
    HashFunction<myuint> hfr{hff.invert()};

    std::cout << hff.to_string() << std::endl;
    std::cout << hfr.to_string() << std::endl;

    CLUTCHLOG(progress, "Done.");
}
