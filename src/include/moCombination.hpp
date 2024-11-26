#ifndef MOCOMBINATION_HPP
#define MOCOMBINATION_HPP

#include <vector>

#include <eo>
#include <mo>
#include <clutchlog/clutchlog.h>

/** A sequence of (possibly duplicated) indices drawn from a fixed set.
 *
 * That is, for a sequence of size n drawing from m indices:
 * \f[
 *     \{x_0, \dots, x_i, \dots, x_{n-1} \| x_i \in \{0, \dots, m-1\} \subset \mathrm{N}\}
 * \f]
 *
 * For a sequence of size 3, drawing from a set of size 4, valid sequences could be:
 * - {0,1,2}
 * - {1,1,1}
 * - {3,1,0}
 * - {3,1,3}
 */
template<class FitT>
class moCombination : public eoInt<FitT>
{
    public:
        /** The type for indices. */
        using AtomType = size_t;

        /** The data structures holding the indices. */
        // using ContainerType = std::vector<AtomType>;

        /** Constructor with initial sequence.
         *
         * @param combination A container.
         * @param nb_options The past-the-maximum value that can be set.
         */
        moCombination(std::vector<size_t> combination, size_t nb_options) :
            eoInt<FitT>(combination),
            #ifndef NDEBUG
                _is_init(true),
            #endif
            _nb_options(nb_options)
        {
            CLUTCHLOG(debug, "instantiated with combination of size " << this->size() << " and " << _nb_options << " options");
            assert(_nb_options >= 2); // At least 2 options.
            #ifndef NDEBUG
                for(const auto i : *this) {
                    assert(i < _nb_options);
                }
            #endif
        }

        /** Constructor with sizes.
         *
         * @param size The size of the sequence.
         * @param nb_options The past-the-maximum value that can be set.
         */
        moCombination(size_t size, size_t nb_options) :
            eoInt<FitT>(size, nb_options),
            #ifndef NDEBUG
                _is_init(false),
            #endif
            _nb_options(nb_options)
        {
            CLUTCHLOG(debug, "instantiated with size " << this->size() << " and " << _nb_options << "options");
            assert(_nb_options >= 2);
        }

        /** Empty constructor.
         */
        moCombination() :
            eoInt<FitT>(),
            #ifndef NDEBUG
                _is_init(false),
            #endif
            _nb_options(0)
        {
            CLUTCHLOG(debug, "empty instantiated");
            assert(this->size() == 0);
        }

        /** Setter.
         *
         * @param index The index at which to change the atom.
         * @param value The atom value to put there.
         */
        void set(size_t index, size_t value)
        {
            CLUTCHLOG(debug, "set combination: @" << index << "/" << this->size() << " =" << value << "/" << _nb_options);
            assert(_is_init);
            assert(index < this->size());
            assert(value < _nb_options);
            if((*this)[index] == value) {
                std::clog << "call to `set` produces identical state" << std::endl;
                // FIXME: eo::log << eo::warnings << "call to `set` produces identical state" << std::endl;
            }
            (*this)[index] = value;
        }

        /** The size of the possible indices. */
        size_t nb_options() const {
            return _nb_options;
        }

        /** Serialize */
        virtual void printOn(std::ostream& out) const override {
            assert(_is_init);
            eoVector<FitT,AtomType>::printOn(out);
            out << " " << this->nb_options();
        }

        /** Deserialize */
        virtual void readFrom(std::istream& in) override {
            eoVector<FitT,AtomType> vec; // Fitness & vector
            in >> vec;
            this->clear();
            this->reserve(vec.size());
            std::copy(std::begin(vec), std::end(vec), std::back_inserter(*this));
            this->fitness(vec.fitness());

            size_t nbo;
            in >> nbo;
            this->_nb_options = nbo;

            #ifndef NDEBUG
                for(auto i : *this) {
                    assert(0 <= i and i < _nb_options);
                }
                this->_is_init = true;
            #endif
        }

        //! Class name for state management.
        virtual std::string className() const override {
            return "moCombination";
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

        //! The past-the-maximum value of the indices that can be set.
        size_t _nb_options;

        //! The sequence of indices.
        // ContainerType _combination;
};

#endif // MOCOMBINATION_HPP
