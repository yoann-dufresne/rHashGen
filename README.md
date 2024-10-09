# Reversible Hash Function Generator

This project is a reversible hash function generator that allows you to create custom hash functions with various operators.

## Compilation

To compile the example and test binaries in a build directory, follow these steps:

```bash
# Clone the repository, navigate to the project directory, create a build directory, navigate to the build directory
git clone --recursive https://github.com/username/repo.git && cd repo
mkdir build && cd build
# Configure the build using CMake
cmake ..
# Compile the solver
make search
# See how to run the solver
./search --help
```

## Run tests

To run the tests, use the following commands:

```bash
mkdir -p build && cd build && cmake -CMAKE_BUILD_TYPE=Debug .. && make && make test
```

