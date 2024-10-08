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

    // The size of the values to manipulate is 57 bits.
    size_t value_size{32};
    using myuint = uint32_t;

    CLUTCHLOG(progress, "Try HashFunc");
    // Create an instance of HashFunction with a value size of 64 bits
    HashFunction<myuint> hashFunc(value_size, "hash");

    // Add shift operators
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

    StrictAvalancheTest<myuint> strict_test{hashFunc};
    CLUTCHLOG(progress, "Run SoftAvalancheTest");
    for (size_t i = 0; i < 20; i++)
    {
        CLUTCHLOG(note, "     10 000 iterations:\t" << strict_test.run(value_size * 10000UL));
    }

    return 0;
}
