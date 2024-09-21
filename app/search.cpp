#include <iostream>
#include <cstdint>
#include <memory>
#include <map>
#include <iostream>

#include <mo>

#include "HashFunction.hpp"
#include "Operator.hpp"
#include "XorLeftShift.hpp"
#include "XorRightShift.hpp"
#include "Multiply.hpp"
#include "AddShift.hpp"
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

//! Error codes returned on exit.
enum class Error : unsigned char {
    No_Error = 0,
    // No_File = 2, // ENOENT
    Invalid_Argument = 22, // EINVAL
    // Unreadable = 77, // EBADFD
    Missing_Argument = 132,
    InconsistentDomain,
    Unknown = 255
};

//! Macro to hide the necessary verbose casting.
#ifdef WITH_CLUTCHLOG
    #define EXIT_ON_ERROR(err_code, msg) { \
        if(static_cast<unsigned char>(Error::err_code) > 0) { \
            CLUTCHLOG(critical, "CRITICAL ERROR"); \
            CLUTCHLOG(critical, msg); \
        } \
        exit(static_cast<unsigned char>(Error::err_code)); \
    }
#else
    #define EXIT_ON_ERROR(err_code, msg) { \
        if(static_cast<unsigned char>(Error::err_code) > 0) { \
            std::cerr << "CRITICAL ERROR: " << msg << std::endl; \
        } \
        exit(static_cast<unsigned char>(Error::err_code)); \
    }
#endif

struct Range {
    Range(size_t mi, size_t ma, size_t st) : min(mi), max(ma), step(st) {}
    size_t min;
    size_t max;
    size_t step;
};

void make_domain(eoForgeVector< EvalFull<myuint,Combi>::OpItf >& forge, size_t value_size, Range shift_range, Range mult_range)
{
    for(size_t i = shift_range.min; i < shift_range.max; i+=shift_range.step) {
        forge.add< XorLeftShift<myuint> >(i, value_size);
        CLUTCHLOG(xdebug, "XorLeftShift << " << i);

        forge.add< XorRightShift<myuint> >(i, value_size);
        CLUTCHLOG(xdebug, "XorRightShift << " << i);

        forge.add< AddShift<myuint> >(i, value_size);
        CLUTCHLOG(xdebug, "AddShift << " << i);
    }

    #ifndef NDEBUG
    size_t nb_multipliers = 0;
    #endif
    for(size_t i = mult_range.min; i < shift_range.max; i+=mult_range.step) {
        if(i % 2 == 1) { // Only odd multipliers are allowed.
            forge.add< Multiply<myuint> >(i, value_size);
            CLUTCHLOG(xdebug, "Multiply * " << i);
            #ifndef NDEBUG
            nb_multipliers += 1;
            #endif
        }
    }
    ASSERT(nb_multipliers > 0);
}

int main(int argc, char* argv[])
{
    CLUTCHLOG(progress, "Set config...");
    eoParser argparser(argc, argv);

    /***** Classical arguments *****/

    const std::string log_level = argparser.createParam<std::string>("Progress", "log-level",
        "Maximum depth level of logging (Critical<Error<Warning<Progress<Note<Info<Debug<XDebug)", 'l', "Logging").value();

    const std::string log_file = argparser.createParam<std::string>(".*", "log-file",
        "Regexp indicating which source file is allowed logging (use '.*' to allow all)", 'f', "Logging").value();

    const std::string log_func = argparser.createParam<std::string>(".*", "log-func",
        "Regexp indicating which function is allowed logging (use '.*' to allow all)", 'F', "Logging").value();

    const size_t log_depth = argparser.createParam<size_t>(std::numeric_limits<size_t>::max(), "log-depth",
        "Maximum stack depth above which logging is not allowed (the larger, the more is displayed)", 'D', "Logging").value();

    unsigned long long seed = argparser.createParam<long>(0, "seed",
        "Seed of the pseudo-random generator (0 = use number of seconds since The Epoch)", 's', "Parameters").value();

    /***** Search domain arguments *****/

    const size_t value_size = argparser.createParam<size_t>(31, "value-size",
        "Value size (in bits)", 'v', "Search domain").value();

    const size_t func_len = argparser.createParam<size_t>(3, "func-len",
        "Number of operations in the hash function", 'n', "Search domain").value();

    const size_t shift_min = argparser.createParam<size_t>(2, "shift-min",
        "Minimum number of shifts", 't', "Search domain").value();
    const size_t shift_max = argparser.createParam<size_t>(31, "shift-max",
        "Maximum number of shifts", 'T', "Search domain").value();
    const size_t shift_step = argparser.createParam<size_t>(1, "shift-step",
        "Increment step for number of shifts", 'i', "Search domain").value();
    Range shift_range(shift_min, shift_max, shift_step);

    const size_t mult_min = argparser.createParam<size_t>(3, "mult-min",
        "Smallest multiplier", 'm', "Search domain").value();
    const size_t mult_max = argparser.createParam<size_t>(65, "mult-max",
        "Largest multiplier", 'M', "Search domain").value();
    const size_t mult_step = argparser.createParam<size_t>(2, "mult-step",
        "Increment step for multipliers (note: only odd multipliers will be allowed)", 'u', "Search domain").value();
    Range mult_range(mult_min, mult_max, mult_step);

    /***** Search domain arguments *****/

    std::map<std::string,std::string> algorithms;
    algorithms["HC"] = "Hill-Climbing";
    algorithms["SA"] = "Simulated-Annealing";
    std::ostringstream msg; msg << " (";
    for(auto& kv : algorithms) {
        msg << kv.first << ":" << kv.second << ", ";
    }
    msg << ")";
    const std::string algo = argparser.createParam<std::string>("HC", "algo",
        "Search metaheuristic"+msg.str(), 'a', "Algorithm").value();

    // make_verbose(argparser);
    make_help(argparser);

    clutchlog_config(); // common config
    auto& log = clutchlog::logger();
    ASSERT(log.levels().contains(log_level));
    log.threshold(log_level);
    log.depth(log_depth);
    log.file(log_file);
    log.func(log_func);

    if(seed == 0) {
        seed = std::time(nullptr); // Epoch
    }

    CLUTCHLOG(info, "seed       = " << seed);
    CLUTCHLOG(info, "log-level  = " << log_level);
    CLUTCHLOG(info, "log-file   = " << log_file);
    CLUTCHLOG(info, "log-func   = " << log_func);
    CLUTCHLOG(info, "log-depth  = " << log_depth);
    CLUTCHLOG(info, "value-size = " << value_size);
    CLUTCHLOG(info, "func-len   = " << func_len);
    CLUTCHLOG(info, "shift-min  = " << shift_min);
    CLUTCHLOG(info, "shift-max  = " << shift_max);
    CLUTCHLOG(info, "shift-step = " << shift_step);
    CLUTCHLOG(info, "mult-min   = " << mult_min);
    CLUTCHLOG(info, "mult-max   = " << mult_max);
    CLUTCHLOG(info, "mult-step  = " << mult_step);

    if(shift_min == 0) {
        EXIT_ON_ERROR(InconsistentDomain, "It makes no sense to set `--shift-min` to zero.");
    }
    if(shift_max < value_size) {
        EXIT_ON_ERROR(InconsistentDomain, "It makes no sense to set --shift-max=" << shift_max << " < --value-size=" << value_size <<"");
    }
    if(mult_min < 3) {
        CLUTCHLOG(warning, "It is probably wrong that `--mult-min` is less than 3.");
    }
    if(mult_step % 2 == 1) {
        CLUTCHLOG(warning, "It is probably wrong that `--mult-step` is not even. Remember that only odd multipliers are actually allowed. Even multipliers will be filtered out.");
    }

    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Create the search domain...");
    eoForgeVector< EvalFull<myuint,Combi>::OpItf > forge(/*always_reinstantiate*/true);
    make_domain(forge, value_size, shift_range, mult_range);
    CLUTCHLOG(info, forge.size() << " operators");
    ASSERT(forge.size() > 0);
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
    std::unique_ptr< moLocalSearch<Nb> > palgo;
    if( algo == "HC" ) {
        palgo = std::make_unique< moRandomBestHC<Nb> >(neighborhood, feval, peval, check);
    } else {
        palgo = std::make_unique< moSA<Nb> >(neighborhood, feval, peval, check);
    }
    moLocalSearch<Nb>& search = *palgo;
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Pick a random solution...");
    std::vector<size_t> v;
    v.reserve(func_len);
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> uni(0, forge.size()-1);
    for(size_t i=0; i<func_len; ++i) {
        v.push_back(uni(rng));
    }
    Combi sol(v, forge.size());
    CLUTCHLOG(info, "Initial solution: " << sol);
    auto hf = feval.make_hashfuncs(sol);
    CLUTCHLOG(info, "Initial hash function: " << hf.forward.get_name() << " / " << hf.reverse.get_name());

    hf = feval.make_hashfuncs(sol);
    std::clog << hf.forward.to_string() << std::endl;
    std::clog << hf.reverse.to_string() << std::endl;
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Evaluate first signature...");
    feval(sol);
    CLUTCHLOG(note, "Initial signature: " << sol);
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Solver run...");
    search(sol);
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Optimized solution:");
    CLUTCHLOG(note, sol );

    CLUTCHLOG(progress, "Output optimized hash function:");
    // Real output.
    hf = feval.make_hashfuncs(sol);
    ASSERT(hf.forward.size() > 0);
    ASSERT(hf.reverse.size() > 0);

    std::cout << hf.forward.to_string() << std::endl;
    std::cout << hf.reverse.to_string() << std::endl;

    CLUTCHLOG(progress, "Done.");
}
