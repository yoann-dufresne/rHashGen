#include <vector>
#include <memory>
#include <sstream>
#include <string>

#include "Operator.hpp"
#include "Masking.hpp"

#ifndef HASHFUNCTION_HPP
#define HASHFUNCTION_HPP

template <typename myuint>
class HashFunction
{
private:
    // A list of operations that constitute the hash function
    std::vector<std::unique_ptr<Operator<myuint>>> m_operators;

    // The size (in bits) of the values to manipulate
    size_t m_value_size;

public:
    HashFunction(size_t value_size) : m_value_size(value_size) {};
    ~HashFunction() = default;

    /** Add an operator to the hash function
     * @param op The operator to add
     */
    void add_operator(std::unique_ptr<Operator<myuint>> op)
    {
        m_operators.push_back(std::move(op));
    }


    /** Fill the vector of operations with the necessary masking operators to complete the hash function
     * The previous list of operations is replaced by a new one.
     */
    void complete_with_masks()
    {
        std::vector<std::unique_ptr<Operator<myuint>>> new_operations;
        // We add a masking operators when it is needed
        bool ongoing_overflow{false};
        for (std::unique_ptr<Operator<myuint>> & op_ptr : m_operators)
        {
            // Skip the loop is the operator is a masking operator
            if (auto const* masking_ptr = dynamic_cast<const Masking<myuint>*>(op_ptr.get())) {
                // op_ptr est de type std::unique_ptr<Masking<myuint>>
                continue;
            }

            // If we have an ongoing overflow and the operator needs to clean the left bits, we add a masking operator
            if (ongoing_overflow and op_ptr->clean_leftbits_needed())
            {
                new_operations.push_back(std::make_unique<Masking<myuint>>(m_value_size - m_operators.size()));
            }

            // We add the operator
            new_operations.push_back(std::move(op_ptr));

            // If the operator is overflowing, we set the ongoing_overflow flag
            if (op_ptr->left_overflowing())
            {
                ongoing_overflow = true;
            }
        }

        // If we have an ongoing overflow at the end, we add a masking operator
        if (m_operators.size() > 0 and m_operators.back()->clean_leftbits_needed())
        {
            new_operations.push_back(std::make_unique<Masking<myuint>>(m_value_size - m_operators.size()));
        }

        m_operators = std::move(new_operations);
    }

    /** Get the string representation of the hash function
     * @return The string representation of the hash function
     */
    std::string to_string() const
    {
        std::stringstream ss;
        ss << "template <typename myuint>\n";
        ss << "myuint hash(myuint val)\n";
        ss << "{\n";

        for (auto const & op : m_operators)
        {
            ss << op->to_string() << "\n";
        }

        ss << "return val;\n";
        ss << "}\n";

        return ss.str();
    }
};

#endif // HASHFUNCTION_HPP
