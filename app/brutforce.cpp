#include <iostream>
#include <cstdint>
#include <cassert>
#include <vector>
#include <string>

#include <eo>

#include "HashFunction.hpp"
#include "Operator.hpp"
#include "XorLeftShift.hpp"
#include "XorRightShift.hpp"
#include "Multiply.hpp"
#include "AddShift.hpp"
#include "Masking.hpp"
#include "AvalancheTest.hpp"

/* In this application we enumerates all the possible reversible hash functions for a certain number of bits and a
 * fixed number of operations. For each of such function we perform an avalanche test and keep only the ones that
 * have the best score.
*/


// class CombinationIterator {
// public:
//     CombinationIterator(const std::vector<std::string>& list, int size)
//         : list(list), size(size), indices(size, 0), finished(list.size() < size)
//     {}

//     // Begin iterator
//     CombinationIterator begin() {
//         return *this;
//     }

//     // End iterator
//     CombinationIterator end() {
//         CombinationIterator endIterator = *this;
//         endIterator.finished = true;
//         return endIterator;
//     }

//     // Move to the next combination
//     CombinationIterator& operator++() {
//         if (!advance()) {
//             finished = true;
//         }
//         return *this;
//     }

//     // Compare two iterators
//     bool operator!=(const CombinationIterator& other) const {
//         return finished != other.finished;
//     }

//     // Access the current combination
//     std::vector<std::string> operator*() const {
//         std::vector<std::string> combination;
//         for (int idx : indices) {
//             combination.push_back(list[idx]);
//         }
//         return combination;
//     }

// private:
//     const std::vector<std::string>& list; // List of elements
//     int size;                             // Combination size
//     std::vector<int> indices;             // Current indices
//     bool finished;                        // Indicates if all combinations have been generated

//     // Advance to the next combination
//     bool advance() {
//         int total = list.size();

//         for (int i = size - 1; i >= 0; --i) {
//             if (indices[i] < total - 1) {
//                 ++indices[i];
//                 for (int j = i + 1; j < size; ++j) {
//                     indices[j] = 0;
//                 }
//                 return true;
//             }
//         }

//         return false; // No more combinations
//     }
// };


int main(size_t argc, char* argv[])
{
    // eoParser argparser(argc, argv);

    // /***** Classical arguments *****/

    // const std::string log_level = argparser.createParam<std::string>("Progress", "log-level",
    //     "Maximum depth level of logging (Critical<Error<Warning<Progress<Note<Info<Debug<XDebug)", 'l', "Logging").value();

    // const std::string log_file = argparser.createParam<std::string>(".*", "log-file",
    //     "Regexp indicating which source file is allowed logging (use '.*' to allow all)", 'f', "Logging").value();

    // const std::string log_func = argparser.createParam<std::string>(".*", "log-func",
    //     "Regexp indicating which function is allowed logging (use '.*' to allow all)", 'F', "Logging").value();

    // const size_t log_depth = argparser.createParam<size_t>(std::numeric_limits<size_t>::max(), "log-depth",
    //     "Maximum stack depth above which logging is not allowed (the larger, the more is displayed)", 'D', "Logging").value();


    // clutchlog_config(); // common config
    // auto& log = clutchlog::logger();
    // ASSERT(log.levels().contains(log_level));
    // log.threshold(log_level);
    // log.depth(log_depth);
    // log.file(log_file);
    // log.func(log_func);
    // CLUTCHLOG(progress, "Set config...");


    // // Brutforce parameters
    // size_t value_size{8};
    // size_t func_len{2};
    
    // // List of operator classes
    // std::vector<std::string> operators{"XorLeftShift", "XorRightShift", "AddShift", "Multiply"};

    // for (auto& comb : CombinationIterator(operators, func_len))
    // {
    //     CLUTCHLOG(note, "Combination:");
    //     for (auto& op : comb)
    //     {
    //         CLUTCHLOG(note, "  " << op);
    //     }
    // }

    return 0;
}

