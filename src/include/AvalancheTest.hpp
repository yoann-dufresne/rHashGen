#include <random>
#include "HashFunction.hpp"
#include <array>
#include <cmath>

#ifndef AVALANCHE_HPP
#define AVALANCHE_HPP

template<typename myuint>
class AvalancheTest
{
protected:
    const size_t m_value_size;
    // Random number generator
    std::random_device rd;
    std::mt19937 m_gen;
    std::uniform_int_distribution<myuint> m_dis;
    const size_t default_nb_tests;
    // Hash function to test
    std::unique_ptr<HashFunction<myuint>> m_hash_function_ptr;
    // Static matrix to store the test results
    size_t m_total_test;
    std::array<std::array<size_t, sizeof(myuint)*8>, sizeof(myuint)*8> m_diff_matrix;

public:
    /** Constructor
     *
     * @param hash_function The hash function to test
     */
    AvalancheTest(const size_t value_size, const size_t nb_tests = 100) :
        m_value_size(value_size),
        m_gen(rd()),
        m_dis(0, (static_cast<size_t>(1) << value_size) - 1),
        default_nb_tests(std::min(nb_tests, static_cast<size_t>(std::numeric_limits<myuint>::max())+1)),
        m_hash_function_ptr(nullptr),
        m_total_test(0)
    { }

    /** Set the hash function to test
     *
     * @param hash_function The hash function to test
     */
    void set_hash_function(HashFunction<myuint>& hash_function)
    {
        // Verify the compatibility of the hash function
        assert(hash_function.get_value_size() == m_value_size);
        m_hash_function_ptr = std::make_unique<HashFunction<myuint>>(hash_function);
        // Reset the diff matrix for the new hash function
        m_total_test = 0;
        for (size_t i{0} ; i<m_diff_matrix.size() ; i++)
            for (size_t j{0} ; j<m_diff_matrix[i].size() ; j++)
                m_diff_matrix[i][j] = 0;
    }

    /** Run the test
     *
     * @param nb_tests The number of tests to run
     * @return The percentage of bits that changed
     */
    virtual double operator()(size_t nb_tests = 0) = 0;

    size_t get_value_size() const
    {
        return m_value_size;
    }

    double average_chi2()
    {
        // Xi² average test
        double mean {0};
        double const expected_diff {static_cast<double>(m_total_test) / static_cast<double>(m_value_size * 2.0)};
        for (size_t i{0} ; i<m_value_size ; i++)
        {
            for (size_t j{0} ; j<m_value_size ; j++)
            {
                // std::cout << "[" << m_diff_matrix[i][j] << "]";
                double const diff {(static_cast<double>(m_diff_matrix[i][j]) - expected_diff) / expected_diff};
                mean += (diff * diff) / static_cast<double>(m_value_size * m_value_size);
            }
            // std::cout << std::endl;
        }

        // Return the percentage of bits that changed
        return sqrt(mean);
    }

    void print_matrix(std::ostream& os)
    {
        for (size_t i{0} ; i<m_value_size ; i++)
        {
            for (size_t j{0} ; j<m_value_size ; j++)
            {
                os << "[" << m_diff_matrix[i][j] << "]";
            }
            os << std::endl;
        }
    }
};



template<typename myuint>
class FullAvalancheTest : public AvalancheTest<myuint>
{
public:
    /** Constructor
     *
     * @param hash_function The hash function to test
     */
    FullAvalancheTest(const size_t value_size) :
        AvalancheTest<myuint>(value_size, 0)
    { }

    /** Run the avalanche test over all the integers of the universe
     *
     * @param nb_tests Useless parameter for Full Avalanche Test
     * @return The percentage of bits that changed
     */
    double operator()(size_t nb_tests = 0)
    {
        assert(this->m_hash_function_ptr != nullptr);
        
        if(nb_tests == 0) {nb_tests = this->default_nb_tests;}

        for (myuint val{std::numeric_limits<myuint>::min()} ; ; val++)
        {
            // Hash the value
            myuint const hash_val {this->m_hash_function_ptr->apply(val)};

            // Flip each bit of the value to test the avalanche
            for (size_t bit_to_flip{0} ; bit_to_flip<this->m_value_size ; bit_to_flip++)
            {
                // flip a bit in A to get B and hash it
                myuint const mutant {val ^ (static_cast<myuint>(1) << bit_to_flip)};
                myuint const hash_mutant {this->m_hash_function_ptr->apply(mutant)};

                // Compare the two hashes
                myuint diff {hash_val ^ hash_mutant};
                // Register the output bits that changed
                for (size_t b_pos{0} ; b_pos<this->m_value_size ; b_pos++)
                {
                    this->m_diff_matrix[bit_to_flip][b_pos] += diff & 1;
                    diff >>= 1;
                }
                this->m_total_test += 1;
            }

            // Stop when we reach the maximum value
            if (val == std::numeric_limits<myuint>::max() or std::log2(val) >= this->m_value_size)
                break;
        }

        // the chi2 average test
        return this->average_chi2();
    }

};



template<typename myuint>
class SamplingAvalancheTest : public AvalancheTest<myuint>
{
public:
    /** Constructor
     *
     * @param hash_function The hash function to test
     */
    SamplingAvalancheTest(const size_t value_size, const size_t nb_tests = 0) :
        AvalancheTest<myuint>(value_size, nb_tests)
    { }

    /** Run the avalanche test over all the integers of the universe
     *
     * @param nb_tests Number of integers to test for the avalanche value computation. One test includes all the possible bit flips comparisons.
     * @return The percentage of bits that changed
     */
    double operator()(size_t nb_tests = 0)
    {
        assert(this->m_hash_function_ptr != nullptr);

        if(nb_tests == 0) {nb_tests = this->default_nb_tests;}

        for (size_t i{0} ; i<nb_tests ; i++)
        {
            // Hash the value
            myuint const val {this->m_dis(this->m_gen)};
            myuint const hash_val {this->m_hash_function_ptr->apply(val)};

            // Flip each bit of the value to test the avalanche
            for (size_t bit_to_flip{0} ; bit_to_flip<this->m_value_size ; bit_to_flip++)
            {
                // flip a bit in A to get B and hash it
                myuint const mutant {val ^ (static_cast<myuint>(1) << bit_to_flip)};
                myuint const hash_mutant {this->m_hash_function_ptr->apply(mutant)};

                // Compare the two hashes
                myuint diff {hash_val ^ hash_mutant};
                // Register the output bits that changed
                for (size_t b_pos{0} ; b_pos<this->m_value_size ; b_pos++)
                {
                    this->m_diff_matrix[bit_to_flip][b_pos] += diff & 1;
                    diff >>= 1;
                }
                this->m_total_test += 1;
            }

            // Stop when we reach the maximum value
            if (val == std::numeric_limits<myuint>::max())
                break;
        }

        // the chi2 average test
        return this->average_chi2();
    }

};



//     template<typename myuint>
// class StrictAvalancheTest : public AvalancheTest<myuint>
// {
// public:
//     /** Constructor
//      *
//      * @param hash_function The hash function to test
//      */
//     StrictAvalancheTest(const size_t value_size, const size_t nb_tests = 0) :
//         AvalancheTest<myuint>(value_size, nb_tests)
//     { }

//     /** Run the test
//      *
//      * @param nb_tests The number of tests to run
//      * @return The percentage of bits that changed
//      */
//     double operator()(HashFunction<myuint>& hash_function, size_t nb_tests = 0)
//     {
//         if(nb_tests == 0) {nb_tests = this->default_nb_tests;}

//         // Number of bits where the function is encoded
//         size_t const nb_bits {hash_function.get_value_size()};
//         // Difference matrix
//         std::vector<std::vector<size_t>> diff_matrix(nb_bits, std::vector<size_t>(nb_bits, 0));

//         // Position of the bit to flip
//         size_t bit_position {0};

//         for (size_t i{0} ; i<nb_tests ; i++)
//         {
//             // Pick a random value A and hash it
//             myuint const A {this->m_dis(this->m_gen)};
//             myuint const hash_A {hash_function.apply(A)};

//             // flip a bit in A to get B and hash it
//             myuint const B {A ^ (static_cast<myuint>(1) << bit_position)};
//             myuint const hash_B {hash_function.apply(B)};

//             myuint diff {hash_A ^ hash_B};

//             // Register the output bits that changed
//             for (size_t b_pos{0} ; b_pos<nb_bits ; b_pos++)
//             {
//                 m_diff_matrix[next_bit_to_flip][b_pos] += diff & 1;
//                 diff >>= 1;
//             }

//             // Move to the next bit to flip in the next iteration
//             next_bit_to_flip = (next_bit_to_flip + 1) % nb_bits;
//         }

//         m_total_test += nb_tests;

//         // Xi² average test
//         double mean {0};
//         double const expected_diff {static_cast<double>(m_total_test) / static_cast<double>(nb_bits * 2.0)};
//         for (size_t i{0} ; i<nb_bits ; i++)
//         {
//             for (size_t j{0} ; j<nb_bits ; j++)
//             {
//                 // std::cout << "[" << m_diff_matrix[i][j] << "]";
//                 double const diff {(static_cast<double>(m_diff_matrix[i][j]) - expected_diff) / expected_diff};
//                 mean += (diff * diff) / static_cast<double>(nb_bits * nb_bits);
//             }
//             // std::cout << std::endl;
//         }
        
//         // Return the percentage of bits that changed
//         return sqrt(mean);
//     }

//     /** Run the test
//      *
//      * @return The percentage of bits that changed
//      */
//     double run_exact_test()
//     {
//         for (size_t i{0} ; i<nb_bits ; i++)
//             for (size_t j{0} ; j<nb_bits ; j++)
//                 m_diff_matrix[i][j] = 0;

//         for (size_t i{0} ; i<(1UL<<nb_bits) ; i++)
//         {
//             if (i % 1000000 == 0) {
//                 m_total_test = i*nb_bits+1;
//                 std::cout << "\t" << i << " iterations:\t" << average_chi2() << std::endl;
//             }

//             myuint const A {i};
//             myuint const hash_A {m_hash_function.apply(A)};

//             for (size_t bit_to_flip{0} ; bit_to_flip<nb_bits ; bit_to_flip++)
//             { 
//                 // flip a bit in A to get B and hash it
//                 myuint const B {A ^ (static_cast<myuint>(1) << bit_to_flip)};
//                 myuint const hash_B {m_hash_function.apply(B)};

//                 myuint diff {hash_A ^ hash_B};
            
//                 // Register the output bits that changed
//                 for (size_t b_pos{0} ; b_pos<nb_bits ; b_pos++)
//                 {
//                     m_diff_matrix[bit_to_flip][b_pos] += diff & 1;
//                     diff >>= 1;
//                 }
//             }
//         }
        
//         return average_chi2();
//     }

//     double average_chi2()
//     {
//         // Xi² average test
//         double mean {0};
//         double const expected_diff {static_cast<double>(m_total_test) / static_cast<double>(nb_bits * 2.0)};
//         for (size_t i{0} ; i<nb_bits ; i++)
//         {
//             for (size_t j{0} ; j<nb_bits ; j++)
//             {
//                 // std::cout << "[" << m_diff_matrix[i][j] << "]";
//                 double const diff {(static_cast<double>(m_diff_matrix[i][j]) - expected_diff) / expected_diff};
//                 mean += (diff * diff) / static_cast<double>(nb_bits * nb_bits);
//             }
//             // std::cout << std::endl;
//         }

//         // Return the percentage of bits that changed
//         return sqrt(mean);
//     }
// };

#endif // AVALANCHE_HPP
