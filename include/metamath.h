#pragma once

// A collection of compile-time math functions


////////////////////////////////////////////////////////////////////////////////
// INCLUDE
////////////////////////////////////////////////////////////////////////////////


#include <cassert>


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


template <typename T>
constexpr T metaHeronSqrt(T x, T guess){
    // See https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Heron's_method
    // In this implementaton guess has to be at least as large as the solution, otherwise the recursion will break too early. For integer types, this will return the integer part of the square root.
    T new_guess = (guess + x/guess)/2;
    return new_guess < guess ? metaHeronSqrt(x, new_guess) : guess;
}

template <typename T>
constexpr T metaSqrt(T x) {
    assert(x >= 0);
    return metaHeronSqrt(x, (x + 1)/2);
}

template <typename T>
constexpr T metaAbs(T x) {
    return x >= 0 ? x : -x;
}
