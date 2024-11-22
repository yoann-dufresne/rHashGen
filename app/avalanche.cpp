#include <iostream>
#include <cstdint>
#include <memory>
#include <eo>

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


int main(int argc, char* argv[])
{
    // --- Argument parser ---

    eoParser argparser(argc, argv);

    const std::string hashfile = argparser.createParam<std::string>("hash_functions.txt", "hash-file",
        "Path to the file containing the hash functions to test", 'f', "IO", true).value();
    const std::string outfile = argparser.createParam<std::string>("", "outfile",
        "File where the matricies will be outputed", 'o', "IO").value();

    const size_t nb_tests = argparser.createParam<size_t>(0, "num-tests",
        "The number of tests to perform on the hash functions. If 0, perform a complete avalanche test",
        'n', "sampling").value();

    make_help(argparser);

    std::cout << "Hash functions file: " << hashfile << std::endl;
    std::cout << "Output file: " << outfile << std::endl;


    clutchlog_config(); // common config
    auto& log = clutchlog::logger();
    log.threshold("XDebug");

    // Max size will be 63 bits (1 more bit needed for mult).
    using myuint = uint64_t;
    
    // --- Preparation to file reading ---

    CLUTCHLOG(progress, "Open hash functions file");
    // Open the file containing the hash functions that we want to test
    std::ifstream file(hashfile);
    if (!file.is_open())
    {
        CLUTCHLOG(critical, "Could not open the file " << hashfile);
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
        CLUTCHLOG(critical, "No hash function to test in the file " << hashfile);
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

        // --- Create the function ---
        HashFunction<myuint> hashFunc(num_bits, "hash");

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

        // --- Avalanche test ---
        std::unique_ptr<AvalancheTest<myuint>> test;
        if (nb_tests == 0)
        {
            CLUTCHLOG(note, "Full avalanche test");
            test = std::make_unique<FullAvalancheTest<myuint>>(num_bits);
        }
        else
        {
            CLUTCHLOG(note, "Sampling avalanche test (" << nb_tests << " tests)");
            test = std::make_unique<SamplingAvalancheTest<myuint>>(num_bits, nb_tests);
        }
        test->set_hash_function(hashFunc);
        auto result = (*test)();
        CLUTCHLOG(progress, "Hash function " << idx << " - Avalanche test (sampling: " << nb_tests << "): " << result);

        // --- Output the matrix ---
        if (outfile.size() > 0)
        {
            std::ofstream out(outfile, std::ios::app);
            test->print_matrix(out);
        }
        else
        {
            test->print_matrix(std::cout);
        }
    }

    return 0;
}
