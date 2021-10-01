# structs

**structs** is a header only C++ library that allows conversion from a struct to a tuple or conversion from a tuple to a struct.

Using this library you get the flexibility of tuples and can keep the safety and clarity of named items in a struct.

# Requirements
- C++ 17
- Catch2 for test suite (https://github.com/catchorg/Catch2)
- Boost (version 1.73 was used for testing)

# Examples
More examples can be found in test.cpp.
```cpp
#include <to_tuple.h>

struct MyStruct
{
    int p1;
    double p2;
};

int main( )
{
    std::tuple< int, double > t =
        structs::to_tuple( MyStruct { 1, 2.0 } );
    ...
}
```

## Converting a struct with std::optional
Until I have more time to spend on this, the library cannot calculate the number of elements in the struct if a std::optional is part of the struct. A workaround is to specify the number of elements in the structs with structs::to_tuple_size.

```cpp
#include <to_tuple.h>

struct MyStructWithOptional
{
    int p1;
    double p2;
    std::optional< bool > p3;
};

// set the tuple size for MyStructWithOptional to 3 elements
template<>
struct structs::to_tuple_size< MyStructWithOptional >
    : std::integral_constant< std::size_t, 3 >
{
};

int main( )
{
    std::tuple< int, double, std::optional< bool > > t =
        structs::to_tuple( MyStruct { 1, 2.0 } );
    ...
}
```

# Licence
structs is an open source project licensed under MIT.