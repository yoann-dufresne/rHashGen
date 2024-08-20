#include <iostream>
#include <cstdint>
#include <memory>

#include "HashFunction.hpp"
#include "Operator.hpp"
#include "XorLeftShift.hpp"
#include "XorRightShift.hpp"
#include "Multiply.hpp"
#include "AddShift.hpp"
#include "Masking.hpp"
#include "Multiply.hpp"


int main()
{
    // The size of the values to manipulate is 57 bits.
    size_t value_size{32};
    using myuint = uint64_t;

    // Create an instance of HashFunction with a value size of 64 bits
    HashFunction<myuint> hashFunc(value_size);

    // Add shift operators
    hashFunc.add_operator(std::make_unique<Multiply<myuint>>(9, value_size));
    hashFunc.add_operator(std::make_unique<XorLeftShift<myuint>>(17, value_size));
    hashFunc.add_operator(std::make_unique<XorLeftShift<myuint>>(5, value_size));
    hashFunc.add_operator(std::make_unique<AddShift<myuint>>(19, value_size));
    hashFunc.add_operator(std::make_unique<XorRightShift<myuint>>(3, value_size));
    hashFunc.add_operator(std::make_unique<Multiply<myuint>>(9, value_size));

    // Complete with masks if necessary
    hashFunc.complete_with_masks();

    // Print the string representation of the hash function
    std::cout << hashFunc.to_string() << std::endl;

    // Get the inverse function
    HashFunction<myuint> revHashFunc{hashFunc.invert()};
    
    // Print the string representation of the inverted hash function
    std::cout << "Inverted function:" << std::endl;
    std::cout << revHashFunc.to_string() << std::endl;

    return 0;
}