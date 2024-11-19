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
    
    // --- Preparation to file reading ---

    CLUTCHLOG(progress, "Open hash functions file");
    std::string hash_file {"../data/hash_loading.txt"};
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

    size_t num_functions {std::stoul(line)};
    if (num_functions == 0)
    {
        CLUTCHLOG(critical, "No hash function to test in the file " << hash_file);
        return 1;
    }
    CLUTCHLOG(note, num_functions << " hash functions will be tested");

    // --- Hash function testing ---
    for (size_t idx{0} ; idx < num_functions ; idx++)
    {
        // Read until we find a line that is not a comment or a spacer
        line = "";
        while (line.size() == 0 or line[0] == '#')
        {
            std::getline(file, line);
        }

        // Get the parameters from the line. Format is:  num_bits num_operators operator1 param1 [... operatorN paramN]
        std::istringstream iss{line};
        size_t num_bits, num_operators;
        iss >> num_bits;
        iss >> num_operators;
        if (num_bits == 0)
        {
            CLUTCHLOG(critical, "The number of bits for the hash function is 0");
            return 1;
        }
        if (num_operators == 0)
        {
            CLUTCHLOG(critical, "The hash function declare 0 operators");
            return 1;
        }

        // Create an instance of HashFunction with the number of bits
        HashFunction<myuint> hashFunc(num_bits, "hash");
        FullAvalancheTest<myuint> full_test{num_bits};

        // Read and add the operators to the hash function
        for (size_t op_idx{0} ; op_idx<num_operators ; op_idx++)
        {
            // Read the type and parameter of the operator
            std::string op_name;
            iss >> op_name;
            uint64_t param;
            iss >> param;

            // Instanciate the operator
            if (op_name == "XSR")
            {
                hashFunc.add_operator(std::make_unique<XorRightShift<myuint>>(param, num_bits));
            }
            else if (op_name == "XSL")
            {
                hashFunc.add_operator(std::make_unique<XorLeftShift<myuint>>(param, num_bits));
            }
            else if (op_name == "MUL")
            {
                hashFunc.add_operator(std::make_unique<Multiply<myuint>>(param, num_bits));
            }
            else if (op_name == "ASL")
            {
                hashFunc.add_operator(std::make_unique<AddShift<myuint>>(param, num_bits));
            }
            else
            {
                CLUTCHLOG(critical, "Unknown operator: " << op_name);
                return 1;
            }
        }
        // Complete the function with masks
        hashFunc.complete_with_masks();

        // Print the string representation of the hash function
        std::cout << hashFunc.to_string() << std::endl;

        // Run the full avalanche test
        full_test.set_hash_function(hashFunc);
        std::cout << "Test completed " << full_test() << std::endl;

        // Print the matrix of the test
        full_test.print_matrix(std::cout);
    }

    return 0;
}
