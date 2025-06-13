#include <iostream>
#include <immintrin.h>
#include <cstring> // For std::memcpy
#include "senkaid/utils/config/root.hpp"

int main()
{
    std::cout << senkaid::utils::config::version::_full;     

    return 0;
}
