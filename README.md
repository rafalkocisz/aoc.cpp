# aoc.cpp
[Advent of Code](https://adventofcode.com/) puzzle solutions implemented in C++.


## Build instructions
In order to build a puzzle solution for a specific year and day, described in the 'yyyydd' format (e.g., 201501, 201807, 202125), use one of the following commands, depending on the compiler you are using:

- MSVC
```console
cl 201501.cpp aoc.cpp -std:c++20 -EHsc
```

- gcc
```console
g++ 201501.cpp aoc.cpp -std=c++20 -pedantic -o 201501
```

- Clang
```console
clang++ 201501.cpp aoc.cpp -std=c++20 -pedantic -o 201501
```

In order to build a puzzle solution test runner for a specific year and day, described in the 'yyyydd' format (e.g., 201501, 201807, 202125), use one of the following commands, depending on the compiler you are using:

- MSVC
```console
cl 201501.cpp aoc.cpp -std:c++20 -DAOC_TEST_SOLUTION -EHsc /Fe:201501-test
```

- gcc
```console
g++ 201501.cpp aoc.cpp -std=c++20 -pedantic -DAOC_TEST_SOLUTION -o 201501-test
```

- Clang
```console
clang++ 201501.cpp aoc.cpp -std=c++20 -pedantic -DAOC_TEST_SOLUTION -o 201501-test
```

In order to build test runner for aoc.cpp use one the following commands, depending on the compiler you are using:

- MSVC
```console
cl aoc.cpp -std:c++20 -DAOC_TEST -EHsc /Fe:aoc-test
```

- gcc
```console
g++ aoc.cpp -std=c++20 -pedantic -DAOC_TEST -o aoc-test
```

- Clang
```console
clang++ aoc.cpp -std=c++20 -pedantic -DAOC_TEST -o aoc-test
