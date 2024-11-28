#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include "Operator.hpp"

#include <clutchlog/clutchlog.h>

#ifndef MULTIPLY_HPP
#define MULTIPLY_HPP

template <typename myuint>
class Multiply : public Operator<myuint>
{
private:
    // Mutliplier constant
    myuint m_multiplier;
    // The size of the variable to manipulate
    size_t m_value_size;

public:
    Multiply(myuint multiplier, size_t value_size, bool trusted_values=false) : m_multiplier(multiplier), m_value_size(value_size) {
        // Skip verification of calculability of the inverse if the values are trusted
        if (trusted_values)
            return;

        // Compute the pgcd between the multiplier and pow(2, value_size) to make sure that the multiplier is invertible
        myuint a = m_multiplier;
        myuint b = 1 << m_value_size;
        myuint r;
        while (b != 0)
        {
            r = a % b;
            a = b;
            b = r;
        }
        if (a != 1)
        {
            std::ostringstream msg;
            msg << "The multiplier " << a << " is not invertible";
            CLUTCHLOG(error, msg.str());
            throw std::runtime_error(msg.str());
        }
    }

    Multiply(Multiply const & other) : Multiply(other.m_multiplier, other.m_value_size, true) {}
    ~Multiply() {}

    myuint get_invert_multiplier() const
    {
        if (m_value_size >= sizeof(myuint) * 8) {
            std::ostringstream msg;
            msg << "Integer size " << m_value_size << " >= sizeof(myuint)*8 = " << sizeof(myuint) * 8 << " is too small. Multiplication inverse impossible. 1 more bit is needed.";
            CLUTCHLOG(error, msg.str());
            throw std::runtime_error(msg.str());
        }
        // Compute the inverse of the multiplier by using the extended euclidean algorithm (cf https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm)
        // The algorithm is modified for unsigned integers. Explanation at https://stackoverflow.com/questions/67097428

        myuint r0 = m_multiplier;
        myuint r1 = 1 << m_value_size;
        myuint s0 = 1;
        myuint s1 = 0;
        myuint t0 = 0;
        myuint t1 = 1;
        size_t n = 0;

        while (r1) {
            myuint q = r0 / r1;
            r0 = r0>q*r1?r0-q*r1:q*r1-r0; std::swap(r0,r1);
            s0 = s0+q*s1; std::swap(s0,s1);
            t0 = t0+q*t1; std::swap(t0,t1);
            ++n;
        }
        // gcd = r0
        if (n%2) s0=(1 << m_value_size)-s0;
        else     t0=m_multiplier-t0;

        return s0;
    }

    // Implement the invert function
    std::vector<std::unique_ptr<Operator<myuint>>> invert() const override
    {
        std::vector<std::unique_ptr<Operator<myuint>>> inverted;

        // Compute the inverse of the multiplier
        myuint inv_multiplier = get_invert_multiplier();
        // Register the inverse operator
        inverted.push_back(std::make_unique<Multiply<myuint>>(inv_multiplier, m_value_size));

        return inverted;
    }

    // Implement the to_string function
    std::string to_string() const override
    {
        std::stringstream ss;
        ss << "val *= " << std::to_string(m_multiplier) << ";";
        return ss.str();
    }

    std::string to_short() const override
    {
        std::ostringstream os;
        os << "x" << m_multiplier;
        return os.str();
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
