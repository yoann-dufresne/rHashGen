#include <gtest/gtest.h>
#include "XorLeftShift.hpp"

TEST(XorLeftShiftTest, Apply)
{
    using myuint = uint32_t;
    myuint value = 0xDADBEEF; // Example value
    size_t size = 28;  // Example mask

    XorLeftShift<myuint> const xls(7, size);
    myuint result = xls.apply(value);

    EXPECT_EQ(result, value ^ (value << 7));
    EXPECT_EQ(xls.left_overflowing(), true);
    EXPECT_EQ(xls.clean_leftbits_needed(), false);
}

TEST(XorLeftShiftTest, Invert)
{
    using myuint = uint32_t;
    size_t size = 28;  // Example mask

    XorLeftShift<myuint> const xls(7, size);
    std::vector<std::unique_ptr<Operator<myuint>>> inverted = xls.invert();

    // Hash the DADBEEF value
    myuint value = 0xDADBEEF;
    value = xls.apply(value) & 0X0FFFFFFF;

    // Invert the hash
    for (auto const & op : inverted)
    {
        value = op->apply(value);
    }
    value &= 0X0FFFFFFF;

    EXPECT_EQ(value, 0xDADBEEF);
}