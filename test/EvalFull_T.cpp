#include <gtest/gtest.h>

#include "HashFunction.hpp"
#include "Operator.hpp"
#include "XorLeftShift.hpp"
#include "XorRightShift.hpp"
#include "Multiply.hpp"
#include "AddShift.hpp"
#include "Masking.hpp"
#include "Multiply.hpp"
#include "moCombination.hpp"
#include "moCombinationNeighbor.hpp"
#include "moCombinationNeighborhood.hpp"
#include "EvalFunc.hpp"

TEST(EvalFull, LargeNeighborhood)
{
    const size_t value_size = 31;
    using myuint = uint32_t;
    using Min = eoMinimizingFitness;
    using Combi = moCombination<Min>;

    eoForgeVector< combi::EvalFull<myuint,Combi>::OpItf > forge(/*always_reinstantiate*/true);
        forge.add< Multiply     <myuint> >( 9, value_size);
        forge.add< XorLeftShift <myuint> >(17, value_size);
        forge.add< XorLeftShift <myuint> >( 5, value_size);
        forge.add< AddShift     <myuint> >(19, value_size);
        forge.add< XorRightShift<myuint> >( 3, value_size);
        forge.add< Multiply     <myuint> >( 9, value_size);

    moCombination<Min> sol({0,0,0,0}, forge.size());
    moCombinationNeighbor<moCombination<Min>> to;
    moCombinationNeighborhood<moCombination<Min>> hood;

    hood.init(sol, to); // {0,0}
    EXPECT_TRUE(hood.hasNeighbor(sol));

    SamplingAvalancheTest<myuint> test(value_size, /*nb_tests*/100);
    combi::EvalFull<myuint,Combi> eval(value_size, forge, test);

    while( hood.cont(sol) ) {
        eval(sol);
        hood.next(sol, to);
    }
}

