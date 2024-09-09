#include <string>
#include <vector>
#include <sstream>

#include "Operator.hpp"

#ifndef XORRIGHTSHIFT_HPP
#define XORRIGHTSHIFT_HPP

/** This class corresponds to the operator xor applied between a value and this value shifted from a certain amount
 * (i.e. value ^= (value >> shifts)).
 */
template <typename myuint>
class XorRightShift : public Operator<myuint>
{
private:
    // The number of shifts to apply
    size_t m_shifts;
    // The size (in bits) of the values to manipulate
    size_t m_value_size;

public:
    XorRightShift(size_t shifts, size_t value_size) : m_shifts(shifts), m_value_size(value_size) {}
    XorRightShift(XorRightShift const & other) : XorRightShift(other.m_shifts, other.m_value_size) {}
    ~XorRightShift() {}

    // Implement the invert function
    std::vector<std::unique_ptr<Operator<myuint>>> invert() const override
    {
        std::vector<std::unique_ptr<Operator<myuint>>> inverted{};

        // We start from the number of shifts and we double it until we reach the size of the myuint type
        // Doing so, we will be able to recover the original value in logaritmic time
        for (size_t recover_size{m_shifts}; recover_size < m_value_size; recover_size *= 2)
        {
            inverted.push_back(std::make_unique<XorRightShift<myuint>>(recover_size, m_value_size));
        }

        return inverted;
    }

    // Implement the to_string function
    std::string to_string() const override
    {
        std::stringstream ss;
        ss << "val ^= val >> " << m_shifts << ";";
        return ss.str();
    }

    std::string to_short() const override
    {
        std::ostringstream os;
        os << "r" << m_shifts;
        return os.str();
    }

    myuint apply(myuint value) const override
    {
        return value ^ (value >> m_shifts);
    }

    bool left_overflowing() const override
    {
        return false;
    }

    bool clean_leftbits_needed() const override
    {
        return true;
    }
};

#endif // XORRIGHTSHIFT_HPP
