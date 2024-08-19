#include <gtest/gtest.h>
#include "XorRightShift.hpp"

TEST(XorRightShiftTest, Apply)
{
    using myuint = uint32_t;
    myuint value = 0xDADBEEF; // Example value
    size_t size = 28;  // Example mask

    XorRightShift<myuint> const xrs(7, size);
    myuint result = xrs.apply(value);

    EXPECT_EQ(result, value ^ (value >> 7));
    EXPECT_EQ(xrs.left_overflowing(), false);
    EXPECT_EQ(xrs.clean_leftbits_needed(), true);
}

TEST(XorRightShiftTest, Invert)
{
    using myuint = uint32_t;
    size_t size = 28;  // Example mask

    XorRightShift<myuint> const xrs(7, size);
    std::vector<std::unique_ptr<Operator<myuint>>> inverted = xrs.invert();

    // Hash the DADBEEF value
    myuint value = 0xDADBEEF;
    value = xrs.apply(value) & 0X0FFFFFFF;

    // Invert the hash
    for (auto const & op : inverted)
    {
        value = op->apply(value);
    }
    value &= 0X0FFFFFFF;

    EXPECT_EQ(value, 0xDADBEEF);
}