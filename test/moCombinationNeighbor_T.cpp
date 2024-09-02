#include <gtest/gtest.h>

#include "moCombinationNeighbor.hpp"

TEST(moCombinationNeighbor, Move)
{
    moCombination<double> sol({0,1,2,3,4,5}, 6);
    moCombinationNeighbor<moCombination<double>> neighb;
    neighb.set(0,1);
    neighb.move(sol);
    ASSERT_EQ(sol.at(0), 1);
}

