#pragma once

namespace senkaid::core::complex {

template <typename TN>
class complex
{
public:
    using value_type = TN;

    TN _re, _im;
    
    constexpr complex() : _re(0), _im(0) {};
    constexpr complex(TN re, TN im = TN()) : _re(re), _im(im) {};

    constexpr complex operator+(const complex& other) const 
    {
        return { _re + other._re, _im + other._im };
    }

    constexpr complex operator*(const complex& other) const 
    {
        return 
        {
            _re * other._re - _im * other._im,
            _re * other._im + _im * other._re
        };
    }

    friend constexpr complex conjugate(const complex& z) 
    {
        return { z._re, -z._im };
    }
};

};
