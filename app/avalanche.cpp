#include <iostream>
#include <cstdint>
#include <memory>

#include "AvalancheTest.hpp"
#include "HashFunction.hpp"
#include "Operator.hpp"
#include "XorLeftShift.hpp"
#include "XorRightShift.hpp"
#include "Multiply.hpp"
#include "AddShift.hpp"
#include "Masking.hpp"
#include "Multiply.hpp"
#include "log.h"


int main()
{
    CLUTCHLOG(progress, "Set config");
    clutchlog_config(); // common config
    auto& log = clutchlog::logger();
    log.threshold("XDebug");

    // Max size will be 63 bits (1 more bit needed for mult).
    using myuint = uint64_t;
    

    CLUTCHLOG(progress, "Load the hash function from file");
    std::string hash_file {"data/hash_loading.txt"};
    // Open the file containing the hash functions that we want to test
    std::ifstream file(hash_file);
    if (!file.is_open())
    {
        CLUTCHLOG(critical, "Could not open the file " << hash_file);
        return 1;
    }

    // Read the number of hash functions that are present in the file
    std::string line{};
    while (line.size() == 0 or line[0] == '#')
    {
        std::getline(file, line);
    }

    // Create an instance of HashFunction with a value size of 64 bits
    size_t value_size{32};
    HashFunction<myuint> hashFunc(value_size, "hash");

    // Add shift operators
    // Should be [16 7feb352d 15 846ca68b 16] = 0.17353355999581582 * 10^-3
    // Computed = 0.000173532 (2^32 iterations)
    hashFunc.add_operator(std::make_unique<XorRightShift<myuint>>(16, value_size));
    hashFunc.add_operator(std::make_unique<Multiply<myuint>>(0x7feb352dU, value_size));
    hashFunc.add_operator(std::make_unique<XorRightShift<myuint>>(15, value_size));
    hashFunc.add_operator(std::make_unique<Multiply<myuint>>(0x846ca68bU, value_size));
    hashFunc.add_operator(std::make_unique<XorRightShift<myuint>>(16, value_size));

    CLUTCHLOG(note, "Complete with masks");
    // Complete with masks if necessary
    hashFunc.complete_with_masks();

    // Print the string representation of the hash function
    std::cout << hashFunc.to_string() << std::endl;

    FullAvalancheTest<myuint> full_test{value_size};
    full_test.set_hash_function(hashFunc);
    CLUTCHLOG(progress, "Run StrictAvalancheTest incrementaly");
    // size_t const step_size {10000};
    // for (size_t iteration = step_size; iteration <= 1000000000UL; iteration += step_size)
    // {
    //     CLUTCHLOG(note, "\t" << iteration << " iterations:\t" << strict_test.run(step_size));
    // }
    std::cout << "Test completed " << full_test() << std::endl;

    return 0;
}
