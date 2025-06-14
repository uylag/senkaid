#include <iostream>
#include <immintrin.h>
#include <cstring> // For std::memcpy
#include "senkaid/core/matrix/base.hpp"

int main()
{
    std::cout << senkaid::utils::config::version::_full << '\n';     
    std::cout << senkaid::core::matrix::major::RowMajor;

    return 0;
}
