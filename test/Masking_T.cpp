#include <gtest/gtest.h>
#include "Masking.hpp" // Include your Masking header file

// Example test case for the Masking operator
TEST(MaskingTest, ApplyMask)
{
    using myuint = uint32_t;
    myuint value = 0b1101; // Example value
    size_t mask_size = 3;  // Example mask
    Masking<myuint> const maskingOperator(mask_size);

    myuint result = maskingOperator.apply(value);

    EXPECT_EQ(result, value & 0b111);
}
