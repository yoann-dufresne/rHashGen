#include <string>
#include <vector>
#include <memory>

#ifndef OPERATOR_HPP
#define OPERATOR_HPP

template <typename myuint>
class Operator
{
private:
    
public:
    virtual std::vector<std::unique_ptr<Operator<myuint>>> invert() const = 0; // This is a pure virtual function, which means that it must be implemented by any derived class. This is a way to enforce that all derived classes have a reverse function.
    virtual std::string to_string() const = 0;
    virtual myuint apply(myuint value) const = 0;
    virtual bool left_overflowing() const = 0;
    virtual bool clean_leftbits_needed() const = 0;
    virtual ~Operator() = default;
};

#endif // OPERATOR_HPP
