#include <vector>
#include <memory>
#include <sstream>
#include <string>

#include "Operator.hpp"
#include "Masking.hpp"
#include "log.h"

#ifndef HASHFUNCTION_HPP
#define HASHFUNCTION_HPP

template <typename myuint>
class HashFunction
{
private:
    // Function name
    std::string m_function_name;

    // A list of operations that constitute the hash function
    std::vector<std::shared_ptr<Operator<myuint>>> m_operators;

    // The size (in bits) of the values to manipulate
    size_t m_value_size;

public:
    HashFunction(std::string const & function_name, size_t value_size) : m_function_name(function_name), m_value_size(value_size) {};
    HashFunction(HashFunction & other) : m_function_name(other.m_function_name), m_operators{std::move(other.m_operators)}, m_value_size(other.m_value_size) {};
    ~HashFunction() = default;

    /** Get the name of the hash function
     * @return The name of the hash function
     */
    std::string get_name() const
    {
        return m_function_name;
    }

    /** Add an operator to the hash function
     * @param op The operator to add
     */
    void add_operator(std::shared_ptr<Operator<myuint>>&& op)
    {
        m_operators.push_back(std::move(op));
    }

    /** Apply the hash function to a value
     * @param value The value to hash
     * @return The hashed value
     */
    myuint apply(myuint value) const
    {
        for (auto const & op : m_operators)
        {
            value = op->apply(value);
        }

        return value;
    }

    /** Fill the vector of operations with the necessary masking operators to complete the hash function
     * The previous list of operations is replaced by a new one.
     */
    void complete_with_masks()
    {
        std::vector<std::shared_ptr<Operator<myuint>>> new_operations;
        // We add a masking operators when it is needed
        bool ongoing_overflow{false};
        for (std::shared_ptr<Operator<myuint>> & op_ptr : m_operators)
        {
            ASSERT(op_ptr != nullptr);

            // Skip the loop if the operator is a masking operator
            if (auto const* masking_ptr = dynamic_cast<const Masking<myuint>*>(op_ptr.get())) {
                // op_ptr est de type std::shared_ptr<Masking<myuint>>
                continue;
            }

            // If we have an ongoing overflow and the operator needs to clean the left bits, we add a masking operator
            if (ongoing_overflow and op_ptr->clean_leftbits_needed())
            {
                new_operations.push_back(std::make_unique<Masking<myuint>>(m_value_size));
                ongoing_overflow = false;
            }

            // If the operator is overflowing, we set the ongoing_overflow flag
            if (op_ptr->left_overflowing())
            {
                ongoing_overflow = true;
            }

            // We add the operator
            new_operations.push_back(std::move(op_ptr));
        }

        // If we have an ongoing overflow at the end, we add a masking operator
        if (new_operations.size() > 0 and ongoing_overflow)
        {
            new_operations.push_back(std::make_unique<Masking<myuint>>(m_value_size));
        }

        m_operators = std::move(new_operations);
    }

    /**
     * Inverts the hash function by reversing the order of operators and applying their inverses.
     *
     * @return The inverted hash function.
     */
    HashFunction<myuint> invert() const
    {
        // Create a new HashFunction object with the same value size as the current hash function
        HashFunction<myuint> inverted{std::string("inverted_") + m_function_name, m_value_size};

        // Iterate over each operator in the current hash function
        for (size_t i = m_operators.size(); i > 0; --i)
        {
            // Get the inverse operators for the current operator
            auto inverse_operators = m_operators[i - 1]->invert();

            // Add the inverse operators to the inverted hash function
            for (auto & inv_op : inverse_operators)
            {
                inverted.add_operator(std::move(inv_op));
            }
        }

        // Complete the inverted hash function with masking operators
        inverted.complete_with_masks();

        // Return the inverted hash function
        return inverted;
    }

    /** Get the string representation of the hash function
     * @return The string representation of the hash function
     */
    std::string to_string() const
    {
        std::stringstream ss;
        ss << "template <typename myuint>\n";
        ss << "myuint " << m_function_name << "(myuint val)\n";
        ss << "{\n";

        for (auto const & op : m_operators)
        {
            ss << "  " << op->to_string() << "\n";
        }

        ss << "  return val;\n";
        ss << "}\n";

        return ss.str();
    }

    size_t size() const {
        return m_operators.size();
    }

};

#endif // HASHFUNCTION_HPP
