#include <gtest/gtest.h>
#include "Multiply.hpp"

TEST(MultiplyTest, Apply)
{
    using myuint = uint32_t;
    myuint value = 0xDADBEEF; // Example value
    size_t size = 28;  // Example mask

    Multiply<myuint> const xls(9, size);
    myuint result = xls.apply(value);

    EXPECT_EQ(result, value * 9);
    EXPECT_EQ(xls.left_overflowing(), true);
    EXPECT_EQ(xls.clean_leftbits_needed(), true);
}

TEST(MultiplyTest, Invert)
{
    using myuint = uint32_t;
    size_t size = 28;  // Example mask

    Multiply<myuint> const xls(9, size);
    std::vector<std::unique_ptr<Operator<myuint>>> inverted = xls.invert();
    EXPECT_EQ(inverted.size(), 1);

    // Hash the DADBEEF value
    myuint value = 0xDADBEEF;
    value = xls.apply(value) & 0X0FFFFFFF;

    // Invert the hash
    value = inverted[0]->apply(value) & 0X0FFFFFFF;
    
    EXPECT_EQ(value, 0xDADBEEF);
}