#ifndef EVALFUNC_HPP
#define EVALFUNC_HPP

#include <eo>
#include <moeo>

#include "log.h"

//! Structure to gather forward and reverse hash functions, as outputed by make_hashfuncs.
template <typename myuint>
class HashFunctionPair
{
public:
    HashFunctionPair( HashFunction<myuint> f, HashFunction<myuint> r ) : forward(f), reverse(r) {}
    HashFunction<myuint> forward;
    HashFunction<myuint> reverse;
};

/** Instantiate the forward and reverse HashFunc from the given solution.
 *
 * @param sol the Paradiseo solution representing a hash function (i.e. a sequence of indices)
 * @param value_size The size (in bits) of the values to manipulate
 * @param forge The set of possible parametrized hash operators.
 */
template<typename myuint, typename EOT>
HashFunctionPair<myuint> make_hashfuncs( EOT& sol, size_t value_size, eoForgeVector<Operator<myuint>>& forge )
{
    ASSERT(sol.size() > 0);
    // ASSERT(sol.nb_options() == forge.size());

    HashFunction<myuint> hff(value_size);

    CLUTCHLOG(xdebug, "Instantiate " << sol.size() << " operators:");
    ASSERT(sol.size() > 0);
    for(size_t i : sol) {
        // CLUTCHLOG(debug, "Instantiate " << i << "th operator");
        // NOTE: this is why we need a shared_ptr and cannot have a unique_ptr.
        hff.add_operator( forge.instantiate_ptr(i) );
    }
    ASSERT(hff.size() == sol.size());
    CLUTCHLOG(xdebug, "Got hash function: " << hff.get_name());
    CLUTCHLOG(xdebug, "Complete with masks");
    hff.complete_with_masks();
    // ASSERT(hff.size() >= sol.size());

    CLUTCHLOG(xdebug, "Invert");
    HashFunction<myuint> hfr{hff.invert()};
    // ASSERT(hfr.size() > 0);
    CLUTCHLOG(xdebug, "Got inverted hash function: " << hfr.get_name());

    #ifndef NDEBUG
        CLUTCHLOG(xdebug, "Check inversion");
        myuint value {0xDADBEEF};
        myuint hashed {hff.apply(value)};
        myuint recovered {hfr.apply(hashed)};
        ASSERT(value == recovered);
    #endif

    return HashFunctionPair<myuint>(hff, hfr);
}

//! Evaluates a mono-objective solution from scratch.
template<typename myuint, typename EOT>
class EvalFull : public eoEvalFunc< EOT >
{
public:
    using OpItf = Operator<myuint>;

protected:
    const size_t m_value_size;
    eoForgeVector<OpItf>& m_forge;

public:
    /** Constructor
     *
     * @param value_size The size (in bits) of the values to manipulate
     * @param forge The set of possible parametrized hash operators.
     */
    EvalFull(size_t value_size, eoForgeVector<OpItf>& forge) :
        m_value_size(value_size),
        m_forge(forge)
    { }

    //! Call interface.
    virtual void operator()(EOT& sol) {
        CLUTCHLOG(xdebug, "Evaluate solution: " << sol);

        auto hffr = make_hashfuncs(sol, m_value_size, m_forge);

        HashFunction<myuint> hff = hffr.forward;
        HashFunction<myuint> hfr = hffr.reverse;

        // TODO: have a real objective function.
        const double quality = hff.size() + hfr.size();

        sol.fitness( quality );
        CLUTCHLOG(xdebug, "Evaluated solution: " << sol);
        CLUTCHLOG(xdebug, "Evaluated hash function: " << hff.get_name());

        ASSERT(not sol.invalid());
    }

};

//! Partial evaluator that actually perform a full evaluation.
template<typename myuint, typename EOT>
class EvalTest : public moEval<moCombinationNeighbor<EOT>>
{
    protected:
        //! Underlying full evaluator.
        EvalFull<myuint,EOT>& _full_eval;

    public:
        /** Constructor.
         *
         * @param full_eval The full evaluator.
         */
        EvalTest(EvalFull<myuint,EOT>& full_eval) :
            _full_eval(full_eval)
        { }

        /** Incremental evaluation.
         *
         * @param solution the solution on which to apply the move.
         * @param neighbor the move to consider.
         */
        void operator()(EOT& solution, moCombinationNeighbor<EOT> & neighbor)
        {
            CLUTCHLOG(debug, "Full Eval of: " << solution << " moved as " << neighbor);
            // Apply the neighbor move on a temp solution.
            EOT newsol = solution;
            neighbor.move(newsol);
            // Full evaluation.
            _full_eval(newsol);
            neighbor.fitness( newsol.fitness() );
            ASSERT(not neighbor.invalid());
        }

        //! Accessor to the underlying full evaluator.
        EvalFull<myuint,EOT>& full_eval();
};


/** Multi-objective trait
 *
 * Indicates the number of objectives, and which one is to be minimized.
 */
class QualityAndRuntimeTraits : public moeoObjectiveVectorTraits
{
public:
    static bool minimizing(int d)
    {
        switch(d) {
            case 0: // quality
                return false;
            case 1: // runtime
                return true;
            default:
                throw(std::out_of_range("Only two objectives are supported"));
        }
    }
    static bool maximizing(int d)
    {
        switch(d) {
            case 0: // quality
                return true;
            case 1: // runtime
                return false;
            default:
                throw(std::out_of_range("Only two objectives are supported"));
        }
    }
    static unsigned int nObjectives()
    {
        return 2;
    }
};

//! Objectives values are stored in a vector of values.
using QualityAndRuntime = moeoRealObjectiveVector<QualityAndRuntimeTraits>;


/** Multi-objective evaluation.
 */
template<typename myuint, typename MOEOT>
class EvalMO : public moeoEvalFunc<MOEOT>
{
public:
public:
    using OpItf = Operator<myuint>;

protected:
    const size_t m_value_size;
    eoForgeVector<OpItf>& m_forge;

public:
    /** Constructor
     *
     * @param value_size The size (in bits) of the values to manipulate
     * @param forge The set of possible parametrized hash operators.
     */
    EvalMO(size_t value_size, eoForgeVector<OpItf>& forge) :
        m_value_size(value_size),
        m_forge(forge)
    { }

    //! Call interface.
    void operator()(MOEOT& sol)
    {
        CLUTCHLOG(xdebug, "Evaluate solution: " << sol);

        auto hffr = make_hashfuncs(sol, m_value_size, m_forge);

        HashFunction<myuint> hff = hffr.forward;
        HashFunction<myuint> hfr = hffr.reverse;

        // TODO: have a real objective function.
        const QualityAndRuntime::Type quality = hff.size() * hfr.size();
        const QualityAndRuntime::Type runtime = hff.size() + hfr.size();

        sol.objectiveVector(0, quality );
        sol.objectiveVector(1, runtime );
        CLUTCHLOG(xdebug, "Evaluated solution: " << sol);
        CLUTCHLOG(xdebug, "Evaluated hash function: " << hff.get_name());

        ASSERT(not sol.invalid());
    }
};


#endif // EVALFUNC_HPP
