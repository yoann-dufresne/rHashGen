#include <gtest/gtest.h>
#include "AddShift.hpp"

TEST(AddShiftTest, Apply)
{
    using myuint = uint32_t;
    myuint value = 0xDADBEEF; // Example value
    size_t size = 28;  // Example mask

    AddShift<myuint> const add(19, size);
    myuint result = add.apply(value);

    EXPECT_EQ(result, value * (1 + (1 << 19)));
    EXPECT_EQ(add.left_overflowing(), true);
    EXPECT_EQ(add.clean_leftbits_needed(), false);
}

TEST(AddShiftTest, Invert)
{
    using myuint = uint32_t;
    size_t size = 28;  // Example mask

    AddShift<myuint> const add(19, size);
    std::vector<std::unique_ptr<Operator<myuint>>> inverted = add.invert();
    EXPECT_EQ(inverted.size(), 1);

    // Hash the DADBEEF value
    myuint value = 0xDADBEEF;
    value = add.apply(value) & 0X0FFFFFFF;

    // Invert the hash
    value = inverted[0]->apply(value) & 0X0FFFFFFF;
    
    EXPECT_EQ(value, 0xDADBEEF);
}