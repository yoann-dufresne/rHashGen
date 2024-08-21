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
class moCombination : public EO<FitT>
{
    public:
        /** The type for indices. */
        using AtomType = size_t;

        /** The data structures holding the indices. */
        using ContainerType = std::vector<AtomType>;

        /** Constructor with initial sequence.
         *
         * @param combination A container.
         * @param nb_options The past-the-maximum value that can be set.
         */
        moCombination(ContainerType combination, size_t nb_options) :
            #ifndef NDEBUG
                _is_init(true),
            #endif
            _nb_options(nb_options),
            _combination(combination)
        {
            CLUTCHLOG(debug, "instantiated with combination of size " << _combination.size() << " and " << _nb_options << " options");
            assert(_nb_options >= 2); // At least 2 options.
            for(const auto i : _combination) {
                assert(i < _nb_options);
            }
        }

        /** Constructor with sizes.
         *
         * @param size The size of the sequence.
         * @param nb_options The past-the-maximum value that can be set.
         */
        moCombination(size_t size, size_t nb_options) :
            #ifndef NDEBUG
                _is_init(false),
            #endif
            _nb_options(nb_options),
            _combination(size, nb_options)
        {
            CLUTCHLOG(debug, "instantiated with size " << _combination.size() << " and " << _nb_options << "options");
            assert(_nb_options >= 2);
        }

        /** Empty constructor.
         */
        moCombination() :
            #ifndef NDEBUG
                _is_init(false),
            #endif
            _nb_options(0)
        {
            CLUTCHLOG(debug, "empty instantiated");
            assert(_combination.size() == 0);
        }

        /** Setter.
         *
         * @param index The index at which to change the atom.
         * @param value The atom value to put there.
         */
        void set(size_t index, size_t value)
        {
            CLUTCHLOG(debug, "set combination: @" << index << "/" << _combination.size() << " =" << value << "/" << _nb_options);
            assert(_is_init);
            assert(index < _combination.size());
            assert(value < _nb_options);
            if(_combination[index] == value) {
                std::clog << "call to `set` produces identical state" << std::endl;
                // FIXME: eo::log << eo::warnings << "call to `set` produces identical state" << std::endl;
            }
            _combination[index] = value;
        }

        /** Accessor to the sequence. */
        ContainerType& get() {
            return _combination;
        }

        /** Accessor to an item of the sequence. */
        AtomType& at(size_t index) {
            return _combination.at(index);
        }

        /** Accessor to an item of the sequence. */
        AtomType& operator[](size_t index) {
            return _combination[index];
        }

        /** The current size of the sequence. */
        size_t size() const {
            return _combination.size();
        }

        /** The size of the possible indices. */
        size_t nb_options() const {
            return _nb_options;
        }

        /** Fancy print. */
        virtual void printOn(std::ostream& out) const override {
            assert(_is_init);
            EO<FitT>::printOn(out); // Fitness.
            out << _combination.size();
            for(const auto i : _combination) {
                out << " " << i;
            }
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
        ContainerType _combination;
};

#endif // MOCOMBINATION_HPP
