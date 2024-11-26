#include <iostream>
#include <cstdint>
#include <memory>
#include <map>
#include <iostream>

#include <mo>
#include <eo>
#include <edo>
#include <es.h>
#include <moeo>
#include <do/make_pop.h>
#include <do/make_run.h>
#include <do/make_continue.h>
#include <do/make_checkpoint.h>

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
using CombiMO = moeoIntVector<combi::QualityAndRuntime>;

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

void make_domain(eoForgeVector< Operator<myuint> >& forge, size_t value_size, Range shift_range, Range mult_range, std::set<std::string> allowed_operators={"XorLeftShift","XorRightShift","AddShift","Multiply"} )
{
    std::map<std::string,size_t> op_count;
        op_count["XorLeftShift"] = 0;
        op_count["XorRightShift"] = 0;
        op_count["AddShift"] = 0;
        op_count["Multiply"] = 0;

    for(size_t i = shift_range.min; i < shift_range.max; i+=shift_range.step) {
        if(allowed_operators.contains("XorLeftShift")) {
            forge.add< XorLeftShift<myuint> >(i, value_size);
            CLUTCHLOG(xdebug, "XorLeftShift << " << i);
            op_count["XorLeftShift"] += 1;
        }

        if(allowed_operators.contains("XorRightShift")) {
            forge.add< XorRightShift<myuint> >(i, value_size);
            CLUTCHLOG(xdebug, "XorRightShift << " << i);
            op_count["XorRightShift"] += 1;
        }

        if(allowed_operators.contains("AddShift")) {
            forge.add< AddShift<myuint> >(i, value_size);
            CLUTCHLOG(xdebug, "AddShift << " << i);
            op_count["AddShift"] += 1;
        }
    }

    if(allowed_operators.contains("Multiply")) {
        size_t nb_multipliers = 0;
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
        op_count["Multiply"] = nb_multipliers;
    }

    CLUTCHLOG(note, "Domain contains " << forge.size() << " operator instances:");
    CLUTCHCODE(note,
        for(auto kv : op_count) {
            CLUTCHLOGD(note, kv.second << " operator " << kv.first, 1);
        }
    );
}

std::string format_hashfunc(HashFunctionPair<myuint>& hf, std::string indent = "    ")
{
    ASSERT(hf.forward.size() > 0);
    ASSERT(hf.reverse.size() > 0);

    std::ostringstream out;
    out << "# YAML\n";
    out << "rHashGen:\n";

    std::string line;

    out << indent << "forward: |" << indent.size() << "\n";
    std::istringstream isf(hf.forward.to_string());
    while( std::getline(isf, line)) {
        out << indent << indent << line << "\n";
    }

    out << indent << "reverse: |" << indent.size() << "\n";
    std::istringstream isr(hf.reverse.to_string());
    while( std::getline(isr, line)) {
        out << indent << indent << line << "\n";
    }

    return out.str();
}

std::string format_solution(const Combi& sol, const size_t value_size, eoForgeVector< Operator<myuint> >& forge, std::string indent = "    ")
{
    CLUTCHLOG(progress, "Optimized solution:");
    CLUTCHLOG(note, sol );
    auto hf = combi::make_hashfuncs(sol, value_size, forge);

    CLUTCHLOG(progress, "Output optimized hash function:");

    std::ostringstream out;
    out << format_hashfunc(hf, indent);

    out << indent<< "solution: \"" << sol << "\"\n"
        << indent<< "quality: " << sol.fitness() << "\n"
        << "" << std::endl;

    CLUTCHLOG(progress, "Done.");

    return out.str();
}

std::string format_solution(const CombiMO& sol, const size_t value_size, eoForgeVector< Operator<myuint> >& forge, std::string indent = "    ")
{
    CLUTCHLOG(progress, "Optimized solution:");
    CLUTCHLOG(note, sol );
    auto hf = combi::make_hashfuncs(sol, value_size, forge);

    CLUTCHLOG(progress, "Output optimized hash function:");

    std::ostringstream out;
    out << format_hashfunc(hf, indent);

    out << indent<< "solution: \"" << sol << "\"\n"
        << indent<< "quality: " << sol.objectiveVector(0) << "\n"
        << indent<< "runtime: " << sol.objectiveVector(1) << "\n"
        << "" << std::endl;

    CLUTCHLOG(progress, "Done.");

    return out.str();
}

std::set<std::string> split_in_set(std::string str, const std::string sep = ",")
{
    std::set<std::string> items;
    size_t pos = 0;
    std::string substr;
    while((pos = str.find(sep)) != std::string::npos) {
        substr = str.substr(0, pos);
        items.insert(substr);
        str.erase(0, pos + sep.length());
    }
    items.insert(str);

    return items;
}

std::vector<std::string> split_in_vec(std::string str, const std::string sep = ",")
{
    std::vector<std::string> items;
    size_t pos = 0;
    std::string substr;
    while((pos = str.find(sep)) != std::string::npos) {
        substr = str.substr(0, pos);
        items.push_back(substr);
        str.erase(0, pos + sep.length());
    }
    items.push_back(str);

    return items;
}

int main(int argc, char* argv[])
{
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


    clutchlog_config(); // common config
    auto& log = clutchlog::logger();
    ASSERT(log.levels().contains(log_level));
    log.threshold(log_level);
    log.depth(log_depth);
    log.file(log_file);
    log.func(log_func);
    CLUTCHLOG(progress, "Set config...");

    unsigned long long seed = argparser.createParam<long>(0, "seed",
        "Seed of the pseudo-random generator (0 = use number of seconds since The Epoch)", 's', "Parameters").value();

    /***** Search domain arguments *****/

    const size_t value_size = argparser.createParam<size_t>(31, "value-size",
        "Value size (in bits)", 'v', "Search domain").value();
    ASSERT(value_size < sizeof(myuint)*CHAR_BIT);

    const size_t func_len = argparser.createParam<size_t>(3, "func-len",
        "Number of operations in the hash function", 'n', "Search domain").value();

    const size_t shift_min = argparser.createParam<size_t>(2, "shift-min",
        "Minimum number of shifts", 't', "Search domain").value();
    const size_t shift_max = argparser.createParam<size_t>(30, "shift-max",
        "Maximum number of shifts", 'T', "Search domain").value();
    ASSERT(shift_max < value_size);
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

    const std::string allowed_ops = argparser.createParam<std::string>("XorLeftShift,XorRightShift,AddShift,Multiply", "operators",
        "Operators allowed in the domain, as a comma-separated list", 'o', "Search domain").value();
    const std::set<std::string> allowed_operators = split_in_set(allowed_ops, ",");
    const std::set<std::string> all_operators = {"XorLeftShift","XorRightShift","AddShift","Multiply"};
    for(const std::string& s : allowed_operators) {
        if(not all_operators.contains(s)) {
            EXIT_ON_ERROR(Invalid_Argument, "Operator \"" << s << "\" is unknown");
        }
    }

    /***** Solver arguments *****/

    std::map<std::string,std::string> algorithms;
    algorithms["HC"] = "Hill-Climbing [mono-objective, fixed-size-combination]";
    algorithms["SA"] = "Simulated-Annealing [mono-objective, fixed-size-combination]";
    algorithms["NSGA2"] = "NSGAII [bi-objective, fixed-size-combination]";
    algorithms["CMAES"] = "CMA-ES [mono-objective, parametrize]";
    std::ostringstream msg; msg << " (";
    for(auto& kv : algorithms) {
        msg << kv.first << ":" << kv.second << ", ";
    }
    msg << ")";
    const std::string algo = argparser.createParam<std::string>("HC", "algo",
        "Search metaheuristic"+msg.str(), 'a', "Algorithm").value();

    const bool init_sol = argparser.createParam<bool>(false, "init-sol",
        "Read initial solution from standard input", 'I', "Algorithm").value();

    const bool parametrize = argparser.createParam<bool>(false, "parametrize",
        "Only tune the parameters and do not change the operators."
        "This will interpret --operators=<SEQ> as a sequence of operators to be parametrized.", 'P', "Algorithm").value();

    const size_t pop_size = argparser.createParam<size_t>(100, "pop-size",
        "Population size for evolutionary algorithms", 'p', "Algorithm").value();

    /***** Objective Functions arguments *****/

    // const size_t nb_tests = argparser.createParam<size_t>(1000, "nb-tests",
        // "Number of tests performed to assess the quality (whether the hash function distributes closely related k-mers uniformly in binary space)", 'x', "Objective Functions").value();


    // make_verbose(argparser);
    make_help(argparser);

    if(seed == 0) {
        seed = std::time(nullptr); // Epoch
    }

    CLUTCHLOGD(info, "seed       = " << seed, 1);
    CLUTCHLOGD(info, "log-level  = " << log_level, 1);
    CLUTCHLOGD(info, "log-file   = " << log_file, 1);
    CLUTCHLOGD(info, "log-func   = " << log_func, 1);
    CLUTCHLOGD(info, "log-depth  = " << log_depth, 1);
    CLUTCHLOGD(info, "value-size = " << value_size, 1);
    CLUTCHLOGD(info, "func-len   = " << func_len, 1);
    CLUTCHLOGD(info, "shift-min  = " << shift_min, 1);
    CLUTCHLOGD(info, "shift-max  = " << shift_max, 1);
    CLUTCHLOGD(info, "shift-step = " << shift_step, 1);
    CLUTCHLOGD(info, "mult-min   = " << mult_min, 1);
    CLUTCHLOGD(info, "mult-max   = " << mult_max, 1);
    CLUTCHLOGD(info, "mult-step  = " << mult_step, 1);
    CLUTCHLOGD(info, "parametrize= " << (parametrize? "ON" : "OFF"), 1);
    CLUTCHCODE(info,
        std::ostringstream msg;
        for(auto op : split_in_vec(allowed_ops, ",")) {msg << " 𐙤 " << op;}
        CLUTCHLOGD(info, "operators " << msg.str(), 1);
    );
    CLUTCHLOGD(info, "init-sol   = " << (init_sol? "ON" : "OFF"), 1);
    CLUTCHLOGD(info, "algo       = " << algo, 1);
    CLUTCHLOGD(info, "pop-size   = " << pop_size, 1);
    // CLUTCHLOGD(info, "nb-tests   = " << nb_tests, 1);

    if(shift_min == 0) {
        EXIT_ON_ERROR(InconsistentDomain, "It makes no sense to set `--shift-min` to zero.");
    }
    if(shift_max >= value_size) {
        EXIT_ON_ERROR(InconsistentDomain, "It makes no sense to set --shift-max=" << shift_max << " >= --value-size=" << value_size <<"");
    }
    if(mult_min < 3) {
        CLUTCHLOG(warning, "It is probably wrong that `--mult-min` is less than 3.");
    }
    if(mult_step % 2 == 1) {
        CLUTCHLOG(warning, "It is probably wrong that `--mult-step` is not even. Remember that only odd multipliers are actually allowed. Even multipliers will be filtered out.");
    }

    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Create the search domain...");
    eoForgeVector< Operator<myuint> > forge(/*always_reinstantiate*/true);
    make_domain(forge, value_size, shift_range, mult_range, allowed_operators);
    ASSERT(forge.size() > 0);
    CLUTCHLOG(note, "OK");

    CLUTCHLOG(progress, "Instantiate solver...");
    eo::rng.reseed(seed);

    // SamplingAvalancheTest<myuint> test(value_size, /*nb_tests*/nb_tests);
    FullAvalancheTest<myuint> test(value_size);

    if( not parametrize ) {
        if( algo == "HC" or algo == "SA" ) {

            using Nb = moCombinationNeighbor<Combi>;
            using NbHood = moCombinationNeighborhood<Combi>;

            combi::EvalFull<myuint, Combi> feval(value_size, forge, test);
            combi::EvalTest<myuint, Combi> peval(feval);

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
            } else if( algo == "SA" ) {
                palgo = std::make_unique< moSA<Nb> >(neighborhood, feval, peval, check);
            }
            moLocalSearch<Nb>& search = *palgo;
            CLUTCHLOG(note, "OK");

            Combi sol;
            if( init_sol ) {
                CLUTCHLOG(progress, "Read solution from standard input...");
                sol.readFrom(std::cin);
                CLUTCHLOG(info, "Read solution: " << sol);
                sol.invalidate(); // Always invalidate, in case fitness input is wrong..
                ASSERT(sol.size() == func_len);
                ASSERT(sol.nb_options() == forge.size());

            } else {
                CLUTCHLOG(progress, "Pick a random solution...");
                std::vector<size_t> v;
                v.reserve(func_len);
                std::mt19937 rng(seed);
                std::uniform_int_distribution<int> uni(0, forge.size()-1);
                for(size_t i=0; i<func_len; ++i) {
                    v.push_back(uni(rng));
                }
                sol = Combi(v, forge.size());
            }

            CLUTCHLOG(info, "Initial solution: " << sol);
            auto hf = combi::make_hashfuncs(sol, value_size, forge);
            CLUTCHLOG(info, "Initial hash function: " << hf.forward.get_name() << " / " << hf.reverse.get_name());

            // std::clog << hf.forward.to_string() << std::endl;
            // std::clog << hf.reverse.to_string() << std::endl;
            CLUTCHLOG(note, "OK");

            CLUTCHLOG(progress, "Evaluate first signature...");
            feval(sol);
            CLUTCHLOG(note, "Initial signature: " << sol);
            CLUTCHLOG(note, "OK");

            CLUTCHLOG(progress, "Solver run...");
            search(sol);
            CLUTCHLOG(note, "OK");

            const std::string out = format_solution(sol, value_size, forge);
            std::cout << out << std::endl;


        } else if( algo == "NSGA2" ) {

            using ReVec = moeoRealVector<combi::QualityAndRuntime>;

            combi::EvalMO<myuint,CombiMO> eval(value_size, forge, test);
            eoPopLoopEval<CombiMO> popEval(eval);

            // Crossover
            eoQuadCloneOp<CombiMO> xover; // TODO use a real crossover

            // Mutation
            using MutWrapper = eoRealToIntMonOp<CombiMO,ReVec>;
            eoDetUniformMutation<ReVec> mutreal(/*range*/1.5, /*nb*/1); // TODO tune
            eoIntInterval bounds(0,forge.size()-1);
            MutWrapper mutation(mutreal, bounds);

            using InitWrapper = eoRealToIntInit<CombiMO,ReVec>;
            eoRealVectorBounds rebounds(func_len, 0, forge.size()-1);
            eoRealInitBounded<ReVec> initreal(rebounds);
            InitWrapper init(initreal, bounds);

            eoQuadGenOp<CombiMO> genOp(xover);
            eoSGATransform<CombiMO> transform(xover, 0.1, mutation, 0.1);
            eoGenContinue<CombiMO> continuator(10);

            // build NSGA-II
            moeoNSGAII<CombiMO> algo(20, eval, xover, 1.0, mutation, 1.0);
            CLUTCHLOG(note, "OK");

            CLUTCHLOG(progress, "Initialize population...");
            eoPop<CombiMO> pop(20, init);
            CLUTCHLOG(note, "OK");

            CLUTCHLOG(progress, "Solver run...");
            algo(pop);
            CLUTCHLOG(note, "OK");

            auto sol = pop.best_element();

            const std::string out = format_solution(sol, value_size, forge);
            std::cout << out << std::endl;


        } else { // Unknown algo
            std::ostringstream msg;
            msg << "Unknown algorithm: " << algo << ", valid candidates are";
            for( auto& kv : algorithms) {
                msg << ", " << kv.first << " (" << kv.second << ")";
            }
            EXIT_ON_ERROR( Invalid_Argument, msg.str() );
        }

    } else { // parametrize == true

        if( algo == "CMAES" ) {
            std::vector<std::string> operators = split_in_vec(allowed_ops, ",");
            size_t dim = operators.size();
            size_t max_eval = 100000;

            using R = eoReal<eoMinimizingFitness>;
            using CMA = edoNormalAdaptive<R>;

            edoNormalAdaptive<R> gaussian(dim);

            eoState state;
            auto& obj_func = state.pack< param::EvalFull<myuint,R> >(value_size, operators, test);
            auto& eval     = state.pack< eoEvalCounterThrowException<R> >(obj_func, max_eval);
            auto& pop_eval = state.pack< eoPopLoopEval<R> >(eval);

            // TODO get rid of do_make* stuff
            auto& eo_continue  = do_make_continue(  argparser, state, eval);
            auto& pop_continue = do_make_checkpoint(argparser, state, eval, eo_continue);

            auto& best = state.pack< eoBestIndividualStat<R> >();
            pop_continue.add( best );
            auto& distrib_continue = state.pack< edoContAdaptiveFinite<CMA> >();

            // FIXME implement constraints: different bounds for shifts and multiply + odd multipliers
            double bmin = std::min(shift_min, mult_min);
            double bmax = std::max(shift_max, mult_max);
            auto& gen  = state.pack< eoUniformGenerator<R::AtomType> >(bmin, bmax);
            auto& bounder   = state.pack< edoBounderRng<R> >(R(dim, bmin), R(dim, bmax), gen);

            auto& init = state.pack< eoInitFixedLength<R> >(dim, gen);

            auto& selector  = state.pack< eoRankMuSelect<R> >(dim/2);
            auto& estimator = state.pack< edoEstimatorNormalAdaptive<R> >(gaussian);

            auto& sampler   = state.pack< edoSamplerNormalAdaptive<R> >(bounder);
            auto& replacor  = state.pack< eoCommaReplacement<R> >();

            auto& algo = state.pack< edoAlgoAdaptive<CMA> >(
                 gaussian , pop_eval, selector,
                 estimator, sampler , replacor,
                 pop_continue, distrib_continue);

            CLUTCHLOG(progress, "Initialize population...");
            // auto& pop = do_make_pop(argparser, state, init);
            eoPop<R> pop;
            pop.append(pop_size, init);
            R candidate;
            if( init_sol ) {
                CLUTCHLOG(progress, "Read solution from standard input...");
                candidate.readFrom(std::cin);
                CLUTCHLOG(info, "Read solution: " << candidate);
                candidate.invalidate(); // Always invalidate, in case fitness input is wrong..
                ASSERT(candidate.size() == func_len);
                pop.push_back(candidate);
            }
            CLUTCHLOG(note, "OK");

            CLUTCHLOG(progress, "Solver run...");
            try {
                pop_eval(pop,pop);
                algo(pop);
            } catch (eoMaxEvalException& e) {
                CLUTCHLOG(progress, "STOP, reached max number of evaluations");
            }
            CLUTCHLOG(note, "OK");

            auto sol = pop.best_element();

            auto hf = param::make_hashfuncs<myuint,R>(sol, value_size, operators);
            std::cout << format_hashfunc(hf) << std::endl;


        } else { // Unknown algo
            std::ostringstream msg;
            msg << "Unknown algorithm: " << algo << ", valid candidates are";
            for( auto& kv : algorithms) {
                msg << ", " << kv.first << " (" << kv.second << ")";
            }
            EXIT_ON_ERROR( Invalid_Argument, msg.str() );
        }
    }
}
