ttre
====

This is a Regular Expression engine by way of Thompson's Construction, or the McNaughton-Yamada-Thompson's algorithm (`grep` flavor). The library is compiled to a single-header library for convenience.

# Details

## Library code

## Usage

The library makes [use of the new C++20 literal classes as Non-Type template parameters](https://ctrpeach.io/posts/cpp20-class-as-non-type-template-param/), and provides a type-safe regular expression literal and functions that accept the literal type at compile-time.


### Library Functions

#### ttre::print

Prints the NFA as a transitional graph and all of its edges and states.

<u>Example:</u>

```C++
ttre::print<"abc|def">();
```

Output:

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

#### ttre::match

Returns `true` or `false` based on an input string and a regular expression.

```C++
assert(ttre::match<"abc|def">("abc") == true); 
assert(ttre::match<"abc|def">("def") == true); 
assert(ttre::match<"abc|def">("hij") == false); 
assert(ttre::match<"abc|def">("def") == true);
assert(ttre::match<"a*|bb">("aaa") == true);
assert(ttre::match<"a*|bb">("b") == false); 
assert(ttre::match<"a*|bb">("bb") == true); 

// ... and so on
```

### Supported operators

* concatenation
* union operator
* kleene star

### Build

> `make all`


### Tests

> `make test`


