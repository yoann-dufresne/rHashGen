#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Operator.hpp"

#ifndef MULTIPLY_HPP
#define MULTIPLY_HPP

template <typename myuint>
class Multiply : public Operator<myuint>
{
private:
    // Mutliplier constant
    myuint m_multiplier;
    // The size of the variable to manipulate
    size_t m_val_size;

public:
    Multiply(myuint multiplier, size_t val_size) : m_multiplier(multiplier), m_val_size(val_size) {}
    Multiply(Multiply const & other) : Multiply(other.m_multiplier, other.m_val_size) {}
    ~Multiply() {}

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
        ss << "val *= " << std::to_string(m_multiplier) << ";";
        return ss.str();
    }

    myuint apply (myuint value) const override
    {
        return value * m_multiplier;
    }

    bool left_overflowing() const override
    {
        return true;
    }
    
    bool clean_leftbits_needed() const override
    {
        return true;
    }
};    

#endif // MULTIPLY_HPP