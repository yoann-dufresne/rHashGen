#include "Operator.hpp"
#include <sstream>
#include <string>
#include <vector>


#ifndef MASKING_HPP
#define MASKING_HPP

template <typename myuint>
class Masking : public Operator<myuint>
{
private:
    // Number of bits to mask
    size_t m_mask_size;
    // Mask to apply
    myuint m_mask;

public:

    Masking(size_t mask_size) : m_mask_size(mask_size)
                              , m_mask((~static_cast<myuint>(0)) >> (sizeof(myuint) * 8 - mask_size))
    {}
    Masking(Masking const & other) : Masking(other.m_mask_size) {}
    ~Masking() {}

    // No reverse for masking
    std::vector<Operator<myuint> > invert() const override
    {
        std::vector<Operator<myuint> > inverted{};
        return inverted;
    }

    // Implement the to_string function
    std::string to_string() const override
    {
        std::stringstream ss;
        ss << "val &= " << std::string(m_mask) << ";";
        return ss.str();
    }

    myuint apply(myuint value) const override
    {
        return value & m_mask;
    }

    bool left_overflowing() const override
    {
        return false;
    }
    
    bool clean_leftbits_needed() const override
    {
        return false;
    }
};

#endif // MASKING_HPP
