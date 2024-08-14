#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Operator.hpp"

#ifndef ADDSHIFT_HPP
#define ADDSHIFT_HPP


template <typename myuint>
class AddShift : public Operator<myuint>
{
private:
    // The number of shifts to apply
    size_t m_shifts;
    // The size of the variable to manipulate
    size_t m_val_size;

public:
    AddShift(size_t shifts, size_t val_size) : m_shifts(shifts), m_val_size(val_size) {}
    AddShift(AddShift const & other) : AddShift(other.m_shifts, other.m_val_size) {}
    ~AddShift() {}

    // Implement the invert function
    std::vector<std::unique_ptr<Operator<myuint>>> invert() const override
    {
        std::vector<std::unique_ptr<Operator<myuint>>> inverted;
        
        throw std::runtime_error("Not implemented yet");

        return inverted;
    }

    // Implement the to_string function
    std::string to_string() const override
    {
        std::stringstream ss;
        ss << "val += val << " << m_shifts << ";";
        return ss.str();
    }

    myuint apply (myuint value) const override
    {
        return value + (value << m_shifts);
    }

    bool left_overflowing() const override
    {
        return true;
    }
    
    bool clean_leftbits_needed() const override
    {
        return false;
    }
};



#endif // ADDSHIFT_HPP