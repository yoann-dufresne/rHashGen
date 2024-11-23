#include <iostream>
#include <cstdint>
#include <cassert>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

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


class CombinationIterator {
public:
    CombinationIterator(const std::vector<std::string>& list, size_t const size)
        : list(list), size(size), indices(size, 0), finished(false)
    {}

    // Begin iterator
    CombinationIterator begin() {
        return *this;
    }

    // End iterator
    CombinationIterator end() {
        CombinationIterator endIterator = *this;
        endIterator.finished = true;
        return endIterator;
    }

    // Move to the next combination
    CombinationIterator& operator++() {
        if (!advance()) {
            finished = true;
        }
        return *this;
    }

    // Compare two iterators
    bool operator!=(const CombinationIterator& other) const {
        return finished != other.finished;
    }

    // Access the current combination
    std::vector<std::string> operator*() const {
        std::vector<std::string> combination;
        for (int idx : indices) {
            combination.push_back(list[idx]);
        }
        return combination;
    }

private:
    const std::vector<std::string>& list; // List of elements
    int size;                             // Combination size
    std::vector<int> indices;             // Current indices
    bool finished;                        // Indicates if all combinations have been generated

    // Advance to the next combination
    bool advance() {
        int total = list.size();

        for (int i = size - 1; i >= 0; --i) {
            if (indices[i] < total - 1) {
                ++indices[i];
                for (int j = i + 1; j < size; ++j) {
                    indices[j] = 0;
                }
                return true;
            }
        }

        return false; // No more combinations
    }
};


double test_hash_function(size_t const value_size, std::vector<std::string> const& operators, std::vector<uint64_t> const& parameters)
{
    // Create the hash function
    HashFunction<uint64_t> hashFunc(value_size, "hash");

    // Add the operators to the hash function
    for (size_t i = 0; i < operators.size(); ++i)
    {
        if (operators[i] == "XorLeftShift")
        {
            hashFunc.add_operator(std::make_unique<XorLeftShift<uint64_t>>(parameters[i], value_size));
        }
        else if (operators[i] == "XorRightShift")
        {
            hashFunc.add_operator(std::make_unique<XorRightShift<uint64_t>>(parameters[i], value_size));
        }
        else if (operators[i] == "AddShift")
        {
            hashFunc.add_operator(std::make_unique<AddShift<uint64_t>>(parameters[i], value_size));
        }
        else if (operators[i] == "Multiply")
        {
            hashFunc.add_operator(std::make_unique<Multiply<uint64_t>>(parameters[i], value_size));
        }
        else
        {
            std::cerr << "Unknown operator: " << operators[i] << std::endl;
            return 0;
        }
    }

    hashFunc.complete_with_masks();


    // Perform the avalanche test
    FullAvalancheTest<uint64_t> test(value_size);
    test.set_hash_function(hashFunc);
    double score = test();

    return score;
}


struct ParamLimits {
    uint64_t min;
    uint64_t max;
    uint64_t step;
};


class ParamEnumerator {
public:
    ParamEnumerator(size_t const value_size, std::vector<std::string> const& operators)
        : value_size(value_size), operators(operators), params(operators.size(), 0), finished(operators.size() == 0)
    {
        operators_parameters["XorLeftShift"] = {1, value_size - 1, 1};
        operators_parameters["XorRightShift"] = {1, value_size - 1, 1};
        operators_parameters["AddShift"] = {1, value_size - 1, 1};

        uint64_t const max_value {(~static_cast<uint64_t>(0)) >> (sizeof(uint64_t) * 8 - value_size)};
        operators_parameters["Multiply"] = {3, max_value, 2};

        for (size_t i = 0; i < operators.size(); ++i) {
            params[i] = operators_parameters[operators[i]].min;
        }
    }

    // Begin iterator
    ParamEnumerator begin() {
        return *this;
    }

    // End iterator
    ParamEnumerator end() {
        ParamEnumerator endIterator = *this;
        endIterator.finished = true;
        return endIterator;
    }

    // Move to the next combination
    ParamEnumerator& operator++() {
        if (!advance()) {
            finished = true;
        }
        return *this;
    }

    // Compare two iterators
    bool operator!=(const ParamEnumerator& other) const {
        return finished != other.finished;
    }

    // Access the current combination
    std::vector<uint64_t> operator*() const {
        std::vector<uint64_t> parameters;
        for (int param : params) {
            parameters.push_back(param);
        }
        return parameters;
    }

private:
    size_t value_size;                 // Number of bits
    const std::vector<std::string>& operators; // List of operators
    std::vector<uint64_t> params;          // Current params
    bool finished;                     // Indicates if all combinations have been generated
    std::unordered_map<std::string, ParamLimits> operators_parameters; // Parameters for each operator

    // Advance to the next combination
    bool advance() {
        for (int i = operators.size() - 1; i >= 0; --i) {
            std::string op = operators[i];
            ParamLimits& limits = operators_parameters[op];

            if (params[i] + limits.step <= limits.max) {
                params[i] += limits.step;

                for (size_t j = i + 1; j < operators.size(); ++j) {
                    params[j] = operators_parameters[operators[j]].min;
                }
                return true;
            }
        }

        return false; // No more combinations
    }
};


int main(int argc, char* argv[])
{
    eoParser argparser(argc, argv);

    /***** Classical arguments *****/

    size_t const value_size {argparser.createParam<size_t>(8, "num-bits", "Number of bits for the hash function", 'b', "Hash function").value()};

    size_t const func_len {argparser.createParam<size_t>(2, "num-ops", "Number of operators for the hash function", 'n', "Hash function").value()};

    size_t const skipt_to {argparser.createParam<size_t>(0, "skip-to", "Skip the first combinations to start the brutforce later. This option is made to allow save and restart.").value()};


    clutchlog_config(); // common config
    auto& log = clutchlog::logger();
    log.threshold("XDebug");
    CLUTCHLOG(progress, "Set config...");
    
    // List of operator classes
    std::vector<std::string> operators{"XorLeftShift", "XorRightShift", "AddShift", "Multiply"};

    std::vector<std::string> best_operators;
    std::vector<uint64_t> best_parameters;
    double best_score = 10000000;

    if (skipt_to > 0)
        CLUTCHLOG(note, "Skipping the first " << skipt_to << " operator combinations");

    CombinationIterator c_iter(operators, func_len);
    uint64_t idx=1;
    uint64_t const total = std::pow(operators.size(), func_len);
    for (std::vector<std::string> operators : c_iter)
    {
        if (idx < skipt_to) {
            idx++;
            continue;
        }

        CLUTCHLOG(progress, "" << idx << "/" << total);

        // Skip if there are two consecutive mutliply operators
        bool to_skip = false;
        for (size_t i = 1; i < operators.size(); ++i)
        {
            if (operators[i] == "Multiply" && operators[i - 1] == "Multiply")
            {
                to_skip = true;
                break;
            }
        }

        if (not to_skip)
        {
            // Compute all the operator combinations
            ParamEnumerator p_iter(value_size, operators);
            for (std::vector<uint64_t> parameters : p_iter)
            {
                double score = test_hash_function(value_size, operators, parameters);
                if (score < best_score)
                {
                    best_score = score;
                    best_operators = operators;
                    best_parameters = parameters;

                    std::stringstream ss;
                    ss << best_score << " ";
                    for (size_t i = 0; i < best_operators.size(); ++i)
                    {
                        ss << best_operators[i] << " " << best_parameters[i] << " ";
                    }

                    CLUTCHLOG(note, "New best score: " << ss.str());

                }
            }
        }

        idx += 1;
    }

    return 0;
}

