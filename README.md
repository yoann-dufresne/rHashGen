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


## Command Line Interface

The `search` executable is the main interface to run the optimization solver.
It takes options of the form `--long[=<value>]` or `-s[=<value>]`.
The equal sign is mandatory, spaces between option flag and value are not supported.
A file holding a set of options can be passed using `@<filename>`.
Running the executable once produces a `search.status` file with the last setup,
and be called backed using, e.g.: `./search @search.status`.

It allows running:

- a mono-objective hill-climbing, with `--algo=HC` (the default),
- a mono-objective simulated annealing, with `--algo=SA`,
- a bi-objective evolutionary algorithm, with `--algo=NSGA2`.

Solution are encoded as a vector of indices toward a set of pre-determined
parametrized hash function operators.
The parameters domain can be set with `--shift-*` and `--mult-*` options.

One can pass an initial solution to HC and SA algorithms by using standard
input, for example:
```sh
echo "0 3 1 2 3 101" | ./search --func-len=3 --init-sol=1
```
will initialize the first solution with `𓉘r2𐙤a2𐙤l3𐙤m31𓉝`
(`m31` being added automatically to complete the forward hash).

The solution encoding reads as:
```
┌ Fitness (here a mono-objective one)
│ ┌ func-len
│ │   ┌ Operators indices
│ │ ┌─┴─┐  ┌ Total number of operators (i.e. past-the-max index)
0 3 1 2 3 101
```
Beware that you are responsible for aligning `--func-len` and the size of the
encoded solution.

