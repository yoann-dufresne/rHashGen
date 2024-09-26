#include <gtest/gtest.h>

#include "moCombination.hpp"

TEST(moCombination, Instanciation)
{
    moCombination<double>::ContainerType u{0,1,2,3,4,5};
    moCombination<double> c(u,6);
    c.set(0,1);
    c.set(1,2);
    c.set(5,4);
    moCombination<double>::ContainerType v{1,2,2,3,4,4};
    moCombination<double>::ContainerType w = c;
    ASSERT_EQ(v.size(), w.size()) << "vectors have different size";
    for (size_t i=0; i < w.size(); ++i) {
        ASSERT_EQ(v[i], w[i]) << "vector not correctly set at index " << i;
    }

}

TEST(moCombination_DeathTest, Instantiation)
{
    moCombination<double>::ContainerType v{0,1,2,3,4,5};
    // Constructor with container.
    ASSERT_DEATH({
        moCombination<double> c(v,0); // Not enough options
    }, "");
    ASSERT_DEATH({
        moCombination<double> c(v,1); // Not enough options
    }, "");
    ASSERT_DEATH({
        moCombination<double> c(v,3); // Vector inconsistent with options number.
    }, "");
    ASSERT_DEATH({
        moCombination<double> c(v,3); // Vector inconsistent with options number.
    }, "");
    ASSERT_DEATH({
        moCombination<double> c(v,6);
        c.set(10,0); // Out of container bounds.
    }, "");
    ASSERT_DEATH({
        moCombination<double> c(v,6);
        c.set(0,6); // Out of options bounds.
    }, "");

    // Constructor with sizes.
    ASSERT_DEATH({
        moCombination<double> c(6,0); // Not enough options.
    }, "");
    ASSERT_DEATH({
        moCombination<double> c(6,1); // Not enough options.
    }, "");

    // Empty constructor.
    ASSERT_DEATH({
        moCombination<double> c;
        c.set(0,0); // Not initialized.
    }, "");

}

