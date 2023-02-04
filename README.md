amat
====

This is a small Regular Expression engine in C++20.

## Details

## Usage

The library makes [use of the new C++20 literal classes as Non-Type template parameters](https://ctrpeach.io/posts/cpp20-class-as-non-type-template-param/), and provides a type-safe regular expression literal and functions that accept the literal type at compile-time.


## Library Functions

### amat::match

Returns `true` or `false` based on an input string and a regular expression.

```C++
amat::match<"abc|def">("abc");
```

<u>Examples:</u>

```C++
amat::match<"abc|def">("abc"); // true
amat::match<"a*|bb">("b"); // false
amat::match<"a*|bb">("bb"); // true
// ... and so on
```

### amat::print

Prints the NFA as a transitional graph and all of its edges and states.

<u>Examples:</u>

```C++
amat::print<"abc|def">();
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

### Supported operators

* concatenation
* union operator
* kleene star

### Build

> `make all`


### Tests

> `make test`


