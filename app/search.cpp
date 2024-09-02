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

int main()
{
    using myuint = uint32_t;
    using Min = eoMinimizingFitness;
    using Combi = moCombination<Min>;
    using Nb = moCombinationNeighbor<Combi>;
    using NbHood = moCombinationNeighborhood<Combi>;

    CLUTCHLOG(progress, "Set config");
    clutchlog_config(); // common config
    auto& log = clutchlog::logger();
    log.threshold("Debug");

    const size_t sol_size = 10;
    const size_t value_size = 31;
    size_t seed = 0;

    eoForgeVector< EvalFull<myuint,Combi>::OpItf > forge(/*always_reinstantiate*/true);
        forge.add< Multiply     <myuint> >( 9, value_size);
        forge.add< XorLeftShift <myuint> >(17, value_size);
        forge.add< XorLeftShift <myuint> >( 5, value_size);
        forge.add< AddShift     <myuint> >(19, value_size);
        forge.add< XorRightShift<myuint> >( 3, value_size);
        forge.add< Multiply     <myuint> >( 9, value_size);

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
