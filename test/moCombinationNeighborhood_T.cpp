#include <gtest/gtest.h>

#include "moCombinationNeighborhood.hpp"

TEST(moCombinationNeighborhood, TwoValues)
{
    moCombination<double> sol({0,0}, 2);
    moCombinationNeighbor<moCombination<double>> to;
    moCombinationNeighborhood<moCombination<double>> hood;

    hood.init(sol, to); // {0,0}
    EXPECT_TRUE(hood.hasNeighbor(sol));
    EXPECT_EQ(to.index(), 0);
    EXPECT_EQ(to.value(), 0);

    hood.next(sol, to); // {1,0}
    EXPECT_EQ(to.index(), 0);
    EXPECT_EQ(to.value(), 1);

    hood.next(sol, to); // {0,1}
    EXPECT_EQ(to.index(), 1);
    EXPECT_EQ(to.value(), 1);

    EXPECT_FALSE(hood.cont(sol));
}

TEST(moCombinationNeighborhood, ThreeValues)
{
    moCombination<double> sol({0,0}, 3);
    moCombinationNeighbor<moCombination<double>> to;
    moCombinationNeighborhood<moCombination<double>> hood;

    hood.init(sol, to); // {0,0}
    EXPECT_TRUE(hood.hasNeighbor(sol));
    EXPECT_EQ(to.index(), 0);
    EXPECT_EQ(to.value(), 0);

    hood.next(sol, to); // {1,0}
    EXPECT_EQ(to.index(), 0);
    EXPECT_EQ(to.value(), 1);

    hood.next(sol, to); // {2,0}
    EXPECT_EQ(to.index(), 0);
    EXPECT_EQ(to.value(), 2);

    hood.next(sol, to); // {0,1}
    EXPECT_EQ(to.index(), 1);
    EXPECT_EQ(to.value(), 1);

    hood.next(sol, to); // {0,2}
    EXPECT_EQ(to.index(), 1);
    EXPECT_EQ(to.value(), 2);

    EXPECT_FALSE(hood.cont(sol));
}
