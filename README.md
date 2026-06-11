# Hash Study Lab

[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20WSL-lightgrey.svg)]()

A **learning-oriented** implementation of cryptographic hash functions following RFC specifications. This project is
designed for educational purposes to understand the internals of hash algorithms, not for production use.

> ⚠️ **Disclaimer**: These are **naive, unoptimized implementations** created solely for studying RFCs and learning
> cryptographic primitives. Do not use in production environments. For real applications, use well-audited libraries like
> OpenSSL, Crypto++, or libsodium.

## 🎯 Purpose

This project serves as a hands-on exploration of:

- **RFC Compliance**: Understanding how hash functions are specified in official documents
- **Algorithm Internals**: Learning the inner workings of cryptographic hash functions
- **Performance Analysis**: Comparing naive implementations against optimized versions
- **Educational Foundation**: Building a base for understanding more complex cryptographic concepts

## 📦 Implemented Algorithms

| Algorithm   | RFC                                             | Status     | Output Size | Block Size | Description                          |
|-------------|-------------------------------------------------|------------|-------------|------------|--------------------------------------|
| **SHA-1**   | [RFC 3174](https://tools.ietf.org/html/rfc3174) | ✅ Complete | 160 bits    | 512 bits   | Legacy hash (broken, for study only) |
| **SHA-256** | [RFC 6234](https://tools.ietf.org/html/rfc6234) | ✅ Complete | 256 bits    | 512 bits   | Current standard                     |

