#include <random>
#include "HashFunction.hpp"

#ifndef AVALANCHE_HPP
#define AVALANCHE_HPP


template<typename myuint>
class SoftAvalancheTest
{
private:
    // The hash function to test
    HashFunction<myuint> m_hash_function;
    // Random number generator
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<myuint> dis;

public:
    /** Constructor
     *
     * @param hash_function The hash function to test
     */
    SoftAvalancheTest(HashFunction<myuint> hash_function) :
        m_hash_function(hash_function),
        gen(rd()), dis(0, (static_cast<size_t>(1) << hash_function.get_value_size()) - 1)
    { }

    /** Run the test
     *
     * @param nb_tests The number of tests to run
     * @return The percentage of bits that changed
     */
    double run(size_t nb_tests)
    {
        // Number of bits where the function is encoded
        size_t const nb_bits {m_hash_function.get_value_size()};
        // Total number of bit differences
        size_t total_diff {0};

        // Position of the bit to flip
        size_t bit_position {0};

        for (size_t i{0} ; i<nb_tests ; i++)
        {
            // Pick a random value A
            myuint const A {dis(gen)};
            // flip a random bit in A to get B
            myuint const B {A ^ (static_cast<myuint>(1) << bit_position)};
            // Apply the hash function to A and B
            myuint const hash_A {m_hash_function.apply(A)};
            myuint const hash_B {m_hash_function.apply(B)};
            // Count the number of bits that changed
            total_diff += __builtin_popcount(hash_A ^ hash_B);
            // Move to the next bit to flip in the next iteration
            bit_position = (bit_position + 1) % nb_bits;
        }

        double const expected_diff {(nb_bits * nb_tests) / 2.0};
        
        // Return the percentage of bits that changed
        return (static_cast<double>(total_diff) - expected_diff) / expected_diff;
    }
};



template<typename myuint>
class StrictAvalancheTest
{
private:
    // The hash function to test
    HashFunction<myuint> m_hash_function;
    // Random number generator
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<myuint> dis;

public:
    /** Constructor
     *
     * @param hash_function The hash function to test
     */
    StrictAvalancheTest(HashFunction<myuint> hash_function) :
        m_hash_function(hash_function),
        gen(rd()), dis(0, (static_cast<size_t>(1) << hash_function.get_value_size()) - 1)
    { }

    /** Run the test
     *
     * @param nb_tests The number of tests to run
     * @return The percentage of bits that changed
     */
    double run(size_t nb_tests)
    {
        // Number of bits where the function is encoded
        size_t const nb_bits {m_hash_function.get_value_size()};
        // Difference matrix
        std::vector<std::vector<size_t>> diff_matrix(nb_bits, std::vector<size_t>(nb_bits, 0));

        // Position of the bit to flip
        size_t bit_position {0};

        for (size_t i{0} ; i<nb_tests ; i++)
        {
            // Pick a random value A and hash it
            myuint const A {dis(gen)};
            myuint const hash_A {m_hash_function.apply(A)};

            // flip a bit in A to get B and hash it
            myuint const B {A ^ (static_cast<myuint>(1) << bit_position)};
            myuint const hash_B {m_hash_function.apply(B)};

            myuint diff {hash_A ^ hash_B};
        
            // Register the output bits that changed
            for (size_t b_pos{0} ; b_pos<nb_bits ; b_pos++)
            {
                diff_matrix[bit_position][b_pos] += diff & 1;
                diff >>= 1;
            }

            // Move to the next bit to flip in the next iteration
            bit_position = (bit_position + 1) % nb_bits;
        }

        // Xi² average test
        double mean {0};
        double const expected_diff {nb_tests / (nb_bits * 2.0)};
        for (size_t i{0} ; i<nb_bits ; i++)
        {
            for (size_t j{0} ; j<nb_bits ; j++)
            {
                // std::cout << "[" << diff_matrix[i][j] << "]";
                double const diff {(diff_matrix[i][j] - expected_diff) / expected_diff};
                mean += (diff * diff) / static_cast<double>(nb_bits * nb_bits);
            }
            // std::cout << std::endl;
        }
        
        // Return the percentage of bits that changed
        return sqrt(mean);
    }
};


#endif // AVALANCHE_HPP
