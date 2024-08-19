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
    Multiply(myuint multiplier, size_t val_size) : m_multiplier(multiplier), m_val_size(val_size) {
        // Compute the pgcd between the multiplier and pow(2, val_size) to make sure that the multiplier is invertible
        myuint a = m_multiplier;
        myuint b = 1 << (m_val_size - 1);
        myuint r;
        while (b != 0)
        {
            r = a % b;
            a = b;
            b = r;
        }
        if (a != 1)
        {
            throw std::runtime_error("The multiplier is not invertible");
        }
    }

    Multiply(Multiply const & other) : Multiply(other.m_multiplier, other.m_val_size) {}
    ~Multiply() {}

    myuint get_invert_multiplier() const
    {
        // Compute the inverse of the multiplier by using the extended euclidean algorithm (cf https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm)
        myuint a = m_multiplier;
        myuint b = 1 << (m_val_size - 1);
        myuint r;
        myuint u = 1;
        myuint v = 0;
        myuint u1 = 0;
        myuint v1 = 1;
        myuint q;
        myuint t;
        while (b != 0)
        {
            q = a / b;
            r = a % b;
            a = b;
            b = r;
            t = u;
            u = u1;
            u1 = t - q * u1;
            t = v;
            v = v1;
            v1 = t - q * v1;
        }
        return u % (1 << (m_val_size - 1));
    }

    // Implement the invert function
    std::vector<std::unique_ptr<Operator<myuint>>> invert() const override
    {
        std::vector<std::unique_ptr<Operator<myuint>>> inverted;
        
        // Compute the inverse of the multiplier
        myuint inv_multiplier = get_invert_multiplier();
        // Register the inverse operator
        inverted.push_back(std::make_unique<Multiply<myuint>>(inv_multiplier, m_val_size));

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