<img width="50%" src="/logo/amat.png">

This is a small Regular Expression engine in C++20 as a single header library.

The library makes [use of the new C++20 literal classes as Non-Type template parameters](https://ctrpeach.io/posts/cpp20-class-as-non-type-template-param/), and provides a type-safe regular expression literal and functions that accept the literal type at compile-time.

## Library Functions

### amat::match
---

Returns a boolean based on match of an input string and a regular expression literal.

* Example:
```C++
#include <iostream>
#include <amat.h>

int main() {
    if (amat::match<"abc|def">("abc")) {
        std::cout << "match!" << std::endl;
    }
    return 0;
}
```

* More examples:

```C++
#include <amat.h>

int main() {
    amat::match<"abc|def">("abc"); // true
    amat::match<"(ab)*|cd|abc">("ababab"); // true
    amat::match<"a*|bb">("b"); // false
    amat::match<"a*|bb">("bb"); // true
    return 0;
}
```

### amat::print
---

Prints the automata of a regular expression as an adjacency-list transitional graph (great for debugging).

* Example

```C++
#include <amat.h>

int main() {
    amat::print<"abc|def">();
    return 0;
}
```

* Example output:

```
Regular Expression: "abc|def"
<<NFA>>
States: 16
Initial state: 0
Final state: 16
Branches: 2
  Edges count: 5
0 ε 2  ->  2 a 3  ->  4 b 5  ->  6 c 7  ->  8 ε 16
  Edges count: 5
0 ε 9  ->  9 d 10  ->  11 e 12  ->  13 f 14  ->  15 ε 16
```

## Supported operators

* concatenation
* union operator
* kleene star

More to come!

## Building

Build by making a build directory (i.e. `build/`), run `cmake` in that dir, and then use `make` to build the desired target.

Example:

``` bash
> mkdir build && cd build
> cmake .. -DCMAKE_BUILD_TYPE=[Debug | Coverage | Release]
```

Then, to run the tests in your build directory:

`./amat_test`