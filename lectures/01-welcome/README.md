# Lecture 01 - Welcome to CS106L

## Status

- Date started: 2026-06-21
- Source: `2026Spring-01-Welcome.pdf`
- Progress: First pass

## One-Sentence Summary

CS106L motivates learning modern C++ by showing that C++ is still central to performance-critical software and that the course focuses on writing elegant, safe, efficient C++ rather than merely using C++ syntax.

## Key Ideas

- C++ is often the invisible foundation behind games, browsers, databases, operating systems, compilers, ML runtimes, embedded systems, and high-performance computing.
- C++ tries to combine low-level control with high-level abstractions.
- Modern C++ style is different from simply writing C code inside a C++ compiler.
- Course themes include types, initialization, references, streams, containers, iterators, classes, templates, lambdas, operator overloading, special member functions, move semantics, RAII, smart pointers, and type safety.

## Terms

| Term | My Understanding |
| --- | --- |
| Modern C++ | C++ style that uses language/library features such as `auto`, RAII, smart pointers, templates, and STL abstractions to write safer and clearer code. |
| RAII | Resource Acquisition Is Initialization: bind resource lifetime to object lifetime so cleanup happens automatically. |
| STL | The C++ Standard Template Library, including containers, iterators, algorithms, and related utilities. |
| Smart pointer | A C++ object that manages pointer ownership and lifetime automatically. |
| Move semantics | A way to transfer resources instead of copying them, improving efficiency and expressing ownership changes. |

## Questions

- What exactly makes "modern C++" different from older C++?
- When should I use `auto`, and when does it hide too much information?
- Why is `std::make_unique<std::string>("Hello World!")` preferred over manually using `new`?
- How much C knowledge is useful before learning CS106L?

## Notes

The lecture uses several "valid C++ program" examples to show C++'s range:

- Assembly-like C++ gives maximum control but is hard to read and not portable.
- C-style C++ is simple and familiar, but it does not use modern abstraction or safety tools.
- Modern C++ aims to express intent while preserving efficiency.

The most useful framing for me:

> C++ is not just about being fast. CS106L is about learning how to write C++ that is fast, readable, safe, and idiomatic.

## Review Checklist

- [ ] I can explain why C++ is still used in industry.
- [ ] I can describe the difference between C, old C++, and modern C++ style.
- [ ] I can name the major topics CS106L will cover.
- [ ] I can explain why RAII and smart pointers matter at a high level.

## Follow-Up

- Read the next lecture on types and structs.
- Ask for an explanation of the first modern C++ example from slide 24.
- Create a small `experiments/hello-modern-cpp` example after setting up CMake or a simple compiler command.
