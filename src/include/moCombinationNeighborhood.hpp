#ifndef MOCOMBINATIONNEIGHBORHOOD_HPP
#define MOCOMBINATIONNEIGHBORHOOD_HPP

#include <mo>

#include "moCombinationNeighbor.hpp"

template <class EOT, class Fitness=typename EOT::Fitness>
class moCombinationNeighborhood : public moNeighborhood< moCombinationNeighbor<EOT,Fitness> >
{
    public:
        /** Shortcut for neighbor's type. */
        using Neighbor = moCombinationNeighbor<EOT, Fitness>;

        /** Shortcut for Atom’s type. */
        using AtomType = typename EOT::AtomType;

        moCombinationNeighborhood() : _is_init(false) { }

        /** Initialize the neighborhood.
         *
         * This actually make the neighborhood point to the first possible change:
         * at the first index, using the first value.
         */
        virtual void init(EOT& /*from*/, Neighbor& to) override {
            _i_index = 0;
            _j_value = 0;
            to.set(_i_index, _j_value);
            to.invalidate();
            _is_init = true;
        }

        /** Point to the next neighbor. */
        virtual void next(EOT& from, Neighbor& to) override {
            assert(_is_init);
            assert(cont(from));

            if( _j_value >= from.nb_options()-1) {
                _j_value = 0; // Reset inner loop.
                _i_index++; // Next outer loop.
            } else {
                _j_value++; // Next inner loop.
            }

            // Skip identity moves.
            #ifndef NDEBUG
            if(from.at(_i_index) == _j_value)
            #else
            if(from[_i_index] == _j_value)
            #endif
            {
                next(from, to);
            }

            // Implant this move in the neighbor.
            to.set(_i_index, _j_value);
            to.invalidate();
        }

        /** Returns true if there is more neighbors to be enumerated. */
        virtual bool cont(EOT& from) override {
            assert(_is_init);

            // If reached the last item of the outer loop.
            if( _i_index >= from.size()-1
            and _j_value >= from.nb_options()-1) {
                return false;
            } else { // There is still some items in the outer loop.
                     // and thus also in the inner loop.
                assert( _i_index < from.size() );
                return true;
            }
        }

        /** Returns true if there is actual neighbors in the neighborhood.
         *
         * Essentially just tells if the sequence is not empty
         * and there is options to choose.
         *
         * Should be always true in normal conditions.
         */
        virtual bool hasNeighbor(EOT& solution) override {
            // assert(_is_init);
            return solution.size() > 0 and solution.nb_options() > 0;
        }

        //! Class name for state management.
        virtual std::string className() const override {
            return "moCombinationNeighborhood";
        }

#ifndef NDEBUG
    public:
        bool _is_init;
#else
    protected:
#endif
        /** Index of the currently pointed index. */
        size_t _i_index;

        /** Index of the currently pointed value. */
        size_t _j_value;
};


#endif // MOCOMBINATIONNEIGHBORHOOD_HPP
