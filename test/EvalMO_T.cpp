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

#include <eo>
#include <moeo>
#include <es.h>

TEST(EvalMO, LargeNeighborhood)
{
    const size_t length = 5;
    const size_t value_size = 31;
    using myuint = uint32_t;
    using Combi = moeoIntVector<combi::QualityAndRuntime>;

    eoForgeVector< combi::EvalMO<myuint,Combi>::OpItf > forge(/*always_reinstantiate*/true);
        forge.add< Multiply     <myuint> >( 9, value_size);
        forge.add< XorLeftShift <myuint> >(17, value_size);
        forge.add< XorLeftShift <myuint> >( 5, value_size);
        forge.add< AddShift     <myuint> >(19, value_size);
        forge.add< XorRightShift<myuint> >( 3, value_size);
        forge.add< Multiply     <myuint> >( 9, value_size);

    Combi sol(length, 0);

    combi::EvalMO<myuint,Combi> eval(value_size, forge);

    using MutWrapper = eoRealToIntMonOp<Combi, moeoRealVector<combi::QualityAndRuntime>>;
    eoDetUniformMutation< typename MutWrapper::EOTreal > mutreal(/*range*/forge.size(), /*nb*/length);
    eoIntInterval bounds(0,forge.size()-1);
    MutWrapper mutint(mutreal, bounds);

    for(size_t i=0; i < 100; ++i) {
        const bool changed = mutint(sol);
        assert(changed);
        eval(sol);
    }
}


