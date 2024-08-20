#include <gtest/gtest.h>
#include <random>
#include <string>

#include "HashFunction.hpp"
#include "XorLeftShift.hpp"
#include "XorRightShift.hpp"
#include "AddShift.hpp"

// Example test case for the Masking operator
TEST(HashingTest, uint32_t)
{
    using myuint = uint32_t;

    // Set up the random number generator
    std::random_device rd;  // Obtain a random number from hardware
    auto const seed{rd()};
    std::mt19937 gen(seed); // Seed the generator
    std::uniform_int_distribution<myuint> operator_dist(0, 2);
    std::uniform_int_distribution<myuint> value_dist(0, (1U << 31) - 1);
    std::uniform_int_distribution<myuint> shift_dist(1, 30);

    // Generate 100 random hash functions to test them
    for (size_t idx{0} ; idx<100 ; idx++)
    {
        HashFunction<myuint> hashFunc(std::string("hash_") + std::to_string(idx), 31);
        // Compose the hash function with 30 Operators
        for (size_t i{0} ; i<30 ; i++)
        {
            switch (operator_dist(gen))
            {
                case 0:
                    hashFunc.add_operator(std::make_unique<XorLeftShift<myuint>>(shift_dist(gen), 31));
                    break;
                case 1:
                    hashFunc.add_operator(std::make_unique<XorRightShift<myuint>>(shift_dist(gen), 31));
                    break;
                case 2:
                    hashFunc.add_operator(std::make_unique<AddShift<myuint>>(shift_dist(gen), 31));
                    break;
            }
        }
        // Complete with masks if necessary
        hashFunc.complete_with_masks();

        // Get the inverse function
        HashFunction<myuint> revHashFunc{hashFunc.invert()};

        // Iterate over 100 random values
        for (size_t i{0} ; i<100 ; i++)
        {
            myuint value {static_cast<myuint>(value_dist(gen))};
            myuint hashed {hashFunc.apply(value)};
            myuint recovered {revHashFunc.apply(hashed)};
            ASSERT_EQ(value, recovered)  << "Failed with random seed " << seed;
        }
    }
}
