#include <iostream>
#include <cstdint>
#include <memory>

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
#include "log.h"


int main()
{
    CLUTCHLOG(progress, "Set config");
    clutchlog_config(); // common config
    auto& log = clutchlog::logger();
    log.threshold("XDebug");

    // The size of the values to manipulate is 57 bits.
    size_t value_size{31};
    using myuint = uint32_t;

    CLUTCHLOG(progress, "Try HashFunc");
    // Create an instance of HashFunction with a value size of 64 bits
    HashFunction<myuint> hashFunc(value_size, "hash");

    // Add shift operators
    hashFunc.add_operator(std::make_unique<Multiply<myuint>>(9, value_size));
    hashFunc.add_operator(std::make_unique<XorLeftShift<myuint>>(17, value_size));
    hashFunc.add_operator(std::make_unique<XorLeftShift<myuint>>(5, value_size));
    hashFunc.add_operator(std::make_unique<AddShift<myuint>>(19, value_size));
    hashFunc.add_operator(std::make_unique<XorRightShift<myuint>>(3, value_size));
    hashFunc.add_operator(std::make_unique<Multiply<myuint>>(9, value_size));

    CLUTCHLOG(note, "Complete with masks");
    // Complete with masks if necessary
    hashFunc.complete_with_masks();

    // Print the string representation of the hash function
    std::cout << hashFunc.to_string() << std::endl;

    CLUTCHLOG(note, "Invert");
    // Get the inverse function
    HashFunction<myuint> revHashFunc{hashFunc.invert()};

    // Print the string representation of the inverted hash function
    std::cout << "Inverted function:" << std::endl;
    std::cout << revHashFunc.to_string() << std::endl;

    CLUTCHLOG(progress, "Apply HashFunc");
    // Apply the hash function to a value
    myuint value {0xDADBEEF};
    myuint hashed {hashFunc.apply(value)};
    std::cout << hashFunc.get_name() << "(0x" << std::hex << value << ") = 0x" << hashed << std::endl;

    // Apply the inverse function to the hashed value
    myuint recovered {revHashFunc.apply(hashed)};
    std::cout << revHashFunc.get_name() << "(0x" << std::hex << hashed << ") = 0x" << recovered << std::dec << std::endl;

    CLUTCHLOG(progress, "Try solver");
    using Min = eoMinimizingFitness;
    using Combi = moCombination<Min>;

    eoForgeVector< EvalFull<myuint,Combi>::OpItf > forge(/*always_reinstantiate*/true);
        forge.add< Multiply     <myuint> >( 9, value_size);
        forge.add< XorLeftShift <myuint> >(17, value_size);
        forge.add< XorLeftShift <myuint> >( 5, value_size);
        forge.add< AddShift     <myuint> >(19, value_size);
        forge.add< XorRightShift<myuint> >( 3, value_size);
        forge.add< Multiply     <myuint> >( 9, value_size);

    CLUTCHLOG(note, "Try solutions");
    Combi sol({0,1,2}, forge.size());
    CLUTCHLOG(debug, "Solution: " << sol);

    CLUTCHLOG(note, "Next in neighborhood");
    moCombinationNeighbor<Combi> neighb;
    moCombinationNeighborhood<Combi> hood;
    hood.init(sol, neighb);
    hood.next(sol, neighb);
    CLUTCHLOG(debug, "Neighbor: " << neighb);

    neighb.move(sol);
    CLUTCHLOG(debug, "Solution: " << sol);

    CLUTCHLOG(note, "Evaluate");
    EvalFull<myuint,Combi> eval(value_size, forge);

    eval(sol);

    CLUTCHLOG(progress, "Final solution:");
    std::cout << sol << std::endl;

    return 0;
}
