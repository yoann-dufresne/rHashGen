#ifndef MOCOMBINATIONNEIGHBOR_HPP
#define MOCOMBINATIONNEIGHBOR_HPP

#include <mo>

#include "moCombination.hpp"

/** Stable neighbor for sequences of indices.
 *
 * Models how to move a solution to a neighbor,
 * by changing a single value in the sequence.
 * The size of the sequence is thus guaranted to be stable.
 *
 * The core data structure is tww indices:
 * - where in the sequence to change something,
 * - what new index to put there.
 */
template<class EOT, class Fitness=typename EOT::Fitness>
class moCombinationNeighbor : public moIndexNeighbor<EOT,Fitness>
{
    public:
        /** Shortcut for Atom’s type. */
        using AtomType = typename EOT::AtomType;

        /** Shortcut for container’s type. */
        using ContainerType = typename EOT::ContainerType;

        /** Shortcut for fitness. */
        using moIndexNeighbor<EOT, Fitness>::fitness;

        /** Shortcut for key. */
        using moIndexNeighbor<EOT, Fitness>::key;

        /** Shortcut for index. */
        using moIndexNeighbor<EOT, Fitness>::index;

        /** Default constructor.
         */
        moCombinationNeighbor() :
            moIndexNeighbor<EOT,Fitness>(),
            #ifndef NDEBUG
                _is_init(false)
            #endif
        { }

        /** Copy constructor.
         */
        moCombinationNeighbor( const moCombinationNeighbor<EOT>& other) :
            moNeighbor<EOT,Fitness>(other),
            moIndexNeighbor<EOT,Fitness>(other),
            #ifndef NDEBUG
                _is_init(other._is_init),
            #endif
            _index(other._index),
            _value(other._value)
        {
            this->fitness(other.fitness());
        }

        /** Default assignment operator.
         */
        moCombinationNeighbor<EOT>& operator=(
            const moCombinationNeighbor<EOT>& other)
        {
            moIndexNeighbor<EOT,Fitness>::operator=(other);
            #ifndef NDEBUG
                this->_is_init = other._is_init;
            #endif
            this->_index = other._index;
            this->_value = other._value;
            this->fitness(other.fitness());
            return *this;
        }

        /** Apply the currently stored move.
         *
         * That is: change item at `index` for `value`.
         */
        virtual void move(EOT& solution) override {
            assert(_is_init);
            solution.set(_index, _value);
        }

        /** Set the considered move.
         *
         * @param index The index at which to change the atom.
         * @param value The atom value to put there.
         */
        void set(size_t index, size_t value)
        {
            _index = index;
            _value = value;
            _is_init = true;
        }

        /** Set the considered move.
         *
         * @param idx_val A pair {index,value}.
         */
        void set(std::pair<AtomType,AtomType> idx_val) {
            this->set(idx_val.first, idx_val.second);
        }

        /** Get the considered move.
         *
         * @returns A pair {index, value}.
         */
        std::pair<AtomType,AtomType> get() {
            assert(_is_init);
            return std::make_pair(_index, _value);
        }

        /** Get the considered index.
         *
         * @returns The index.
         */
        AtomType index() {
            assert(_is_init);
            return _index;
        }

        /** Get the considered value.
         *
         * @returns The value.
         */
        AtomType value() {
            assert(_is_init);
            return _value;
        }

        //! Class name for state management.
        virtual std::string className() const override {
            return "moCombinationNeighbor";
        }

        /** Fancy print. */
        virtual void printOn(std::ostream& out) const override {
            assert(_is_init);
            EO<Fitness>::printOn(out); // Fitness.
            out << " "
                << " @" << _index
                << " =" << _value;
        }

#ifndef NDEBUG
    public:
#else
    protected:
#endif
        #ifndef NDEBUG
            /** Sanity flag.
             *
             * Used in debug builds to ensure that the neighbor
             * have been set before being used.
             */
            bool _is_init;
        #endif

        //! The index of the targeted sequence at which the neighbor operates.
        size_t _index;

        //! The value to put at the considered index.
        size_t _value;
};

#endif // MOCOMBINATIONNEIGHBOR_HPP
