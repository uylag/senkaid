#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <ranges>
#include <senkaid/utils/root.hpp>
#include <tuple>
#include <type_traits>
#include <senkaid/core/complex/complex.hpp>

namespace senkaid::core::matrix
{

template <typename TN>
concept Scalar = std::is_arithmetic_v<TN>;

template <typename TN>
concept Matrix = requires(TN t)
{
    typename TN::value_type;
    { t(0, 0) } -> std::convertible_to<typename TN::value_type>;
    { t.rows() } -> std::same_as<std::size_t>;
    { t.cols() } -> std::same_as<std::size_t>;
};

template <typename TM, typename TN>
concept MatrixScalar = (Matrix<TM> && Matrix<TN>) ||
                       (Matrix<TM> && Scalar<TN>) ||
                       (Scalar<TM> && Matrix<TN>) ||
                       (Scalar<TM> && Scalar<TN>);

template <typename> inline constexpr bool unsupported_false = false;

// To receive values through [r, c, s, z] implemented get functions and updated std::
template <typename TN>
struct RotgParameters
{
    TN r, c, s, z;
};

template <std::size_t N, typename TN>
constexpr TN& get(RotgParameters<TN>& p) {
    if constexpr (N == 0) return p.r;
    else if constexpr (N == 1) return p.c;
    else if constexpr (N == 2) return p.s;
    else if constexpr (N == 3) return p.z;
    else static_assert(N < 4, "Index out of range for RotgParameters");
}

template <std::size_t N, typename TN>
constexpr const TN& get(const RotgParameters<TN>& p) {
    if constexpr (N == 0) return p.r;
    else if constexpr (N == 1) return p.c;
    else if constexpr (N == 2) return p.s;
    else if constexpr (N == 3) return p.z;
    else static_assert(N < 4, "Index out of range for RotgParameters");
}

template <std::size_t N, typename TN>
constexpr TN&& get(RotgParameters<TN>&& p) {
    if constexpr (N == 0) return std::move(p.r);
    else if constexpr (N == 1) return std::move(p.c);
    else if constexpr (N == 2) return std::move(p.s);
    else if constexpr (N == 3) return std::move(p.z);
    else static_assert(N < 4, "Index out of range");
}

template <typename TN, typename Derived>
struct SDMatrixBase
{
public:
    using Base = SDMatrixBase<TN, Derived>;

    constexpr SENKAID_FORCE_INLINE TN& operator()(int i, int j);

    // LEVEL 1 BLAS

    constexpr SENKAID_FORCE_INLINE Derived dot(const Derived& other)
    {
        return Derived::dot(*this, other);
    };

    constexpr SENKAID_FORCE_INLINE void dot_inplace(const Derived& other)
    {
        return Derived::dot(this, other);
    };

    constexpr SENKAID_FORCE_INLINE TN sum() const
    {
        return Derived::sum(*this); 
    };

    template <typename TM, typename TO>
    constexpr SENKAID_FORCE_INLINE Derived fma(const TM& b, const TO& c) const
    {
        if constexpr (MatrixScalar<TM, TO>)
            return Derived::axpy(*this, b, c);
        else
            static_assert(unsupported_false<TM>, "Unsupported argument types for fma");
    };
 
    template <typename TM, typename TO>
    constexpr SENKAID_FORCE_INLINE Derived fma_inplace(const TM& b, const TO& c)
    {
        if constexpr (MatrixScalar<TM, TO>)
            return Derived::axpy_inplace(*this, b, c);
        else
            static_assert(unsupported_false<TM>, "Unsupported argument types for fma_inplace");
    };

    template <Matrix M>
    constexpr SENKAID_FORCE_INLINE void copy_to_this(M& other)
    {
        auto& self = static_cast<Derived&>(*this);
        Derived::copy(other, self);
    };

    template <Matrix M>
    constexpr SENKAID_FORCE_INLINE void copy_to_other(M& other)
    {
        auto& self = static_cast<Derived&>(*this);
        Derived::copy(self, other);

    };

    constexpr static SENKAID_FORCE_INLINE RotgParameters<TN> rotg(TN a, TN b) // TODO: SIMD and GPU version
    {
        TN r, c, s, z;

        if (b == TN(0))
        {
            c = TN(0);
            s = TN(0);
            r = a;
            z = TN(0);
        }
        else 
        {
            r = std::sqrt(a * a + b * b); // hypot and fma eat up runtime
            c = a / r;
            s = b / r;
            z = (std::abs(a) > std::abs(b) ? TN(1) : (s != 0 ? TN(1) / s : 0));
        }

        return {r, c, s, z};
    };

    constexpr SENKAID_FORCE_INLINE RotgParameters<TN> rotg() // Based on derived has two elements
    {
        auto& derived = static_cast<Derived&>(*this);
        if (derived.size() > 2)
        {
            SENKAID_LOG_ERROR("rotg: derived matrix has more than two elements.");
            return {TN(0), TN(0), TN(0), TN(0)};
        }

        TN a, b;

        if (derived.row(0).size() == 1)
        {
            a = derived(0, 0);
            b = derived(1, 0);
        }
        else 
        {
            a = derived(0, 0);
            b = derived(0, 1);
        }

        return Base::rotg(a, b);
    };

    constexpr SENKAID_FORCE_INLINE Derived conjugate()
    {
        return Derived::conjugate(*this);
    }

    constexpr SENKAID_FORCE_INLINE Derived conjugate_inplace()
    {
        auto& derived = static_cast<Derived&>(*this);
        return Derived::conjugate_inplace(derived);
    }

    constexpr SENKAID_FORCE_INLINE TN norm(size_t p_order = 2)
    {
        if (p_order < 1)
            SENKAID_LOG_WARNING("error in norm function: p_order less than 1");

        return Derived::norm(*this, p_order);
    }
     
    template <typename TM>
    constexpr static SENKAID_FORCE_INLINE void rot(TN* x, TN* y, std::size_t n, TN c, TN s) // TODO: SIMD and GPU instructions
    {
        if constexpr (std::is_same_v<TN, senkaid::core::complex::complex<TM>>)
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                TN xi = x[i];
                TN yi = y[i];

                x[i] = c * xi + s * yi;
                y[i] = Derived::conjugate(-s) * xi + c * yi;
            }

        }
        else 
        {
            for (std::size_t i = 0; i < n; ++i)
            {
                TN xi = x[i];
                TN yi = y[i];

                x[i] = c * xi + s * yi;
                y[i] = -s * xi + c * yi;
            }
        }
    }; 

    constexpr static SENKAID_FORCE_INLINE TN norm(TN* x, std::size_t sox, std::size_t p_order)
    {
        TN inner = 0;

        for (std::size_t i = 0; i < sox; ++i)
        {
            inner += std::pow(x, p_order);
        }

        return std::sqrt(inner, p_order);
    };

    // Fortran-style based because code from drotmg.f
    static SENKAID_FORCE_INLINE void rotmg(TN& x1, TN& y1, TN& d1, TN& d2, TN params[5])
    {
        constexpr double ZERO = TN(0.0);
        constexpr double ONE = TN(1.0);
        constexpr double TWO = TN(2.0);
        constexpr double GAM = 4096.0;
        constexpr double GAMSQ = 16777216.0;
        constexpr double RGAMSQ = 5.9604645e-8;

        TN dflag, dh11, dh12, dh21, dh22;
        TN dp1, dp2, dq1, dq2, du, temp;

        if (d1 < ZERO)
        {
            dflag = -ONE;
            dh11 = ZERO; dh12 = ZERO;
            dh21 = ZERO; dh22 = ZERO;
            d1 = ZERO; d2 = ZERO;
            x1 = ZERO;
        }
        else 
        {
            dp2 = d2 * y1;
            if (dp2 == ZERO) 
            {
                dflag = -TWO;
                params[0] = dflag;
                return;
            }

            dp1 = d1 * x1;
            dq2 = dp2 * y1;
            dq1 = dp1 * x1;

            if (std::abs(dq1) > std::abs(dq2))
            {
                dh21 = -y1 / x1;
                dh12 = dp2 / dp1;
                du = ONE - dh12 * dh21;
                if (du > ZERO)
                {
                    dflag = ZERO;
                    d1 /= du;
                    d2 /= du;
                    x1 *= du;
                }
                else 
                {
                    dflag = -ONE;
                    dh11 = dh12 = dh21 = dh22 = ZERO;
                    d1 = d2 = x1 = ZERO;
                }
            }
            else 
            {
                if (dq2 < ZERO)
                {
                    dflag = -ONE;
                    dh11 = dh12 = dh21 = dh22 = ZERO;
                    d1 = d2 = x1 = ZERO;
                }
                else 
                {
                    dflag = ONE;
                    dh11 = dp1 / dp2;
                    dh22 = x1 / y1;
                    du = ONE + dh11 * dh22;
                    temp = d2 / du;
                    d2 = d1 / du;
                    d1 = temp;
                    x1 = y1 * du;
                    dh12 = dh21 = ZERO;
                }
            }

            if (d1 != ZERO)
            {
                while ((d1 <= RGAMSQ) || (d1 >= GAMSQ))
                {
                    if (dflag == ZERO)
                    {
                        dh11 = ONE;
                        dh22 = ONE;
                        dflag = -ONE;
                    }
                    else 
                    {
                        dh21 = -ONE;
                        dh12 = ONE;
                        dflag = -ONE;
                    }

                    if (d1 <= RGAMSQ)
                    {
                        d1 *= GAMSQ;
                        x1 /= GAM;
                        dh11 /= GAM;
                        dh12 /= GAM;
                    }
                    else 
                    {
                        d1 /= GAMSQ;
                        x1 *= GAM;
                        dh11 *= GAM;
                        dh12 *= GAM;
                    }
                }
            }

            if (d2 != ZERO)
            {
                while ((std::abs(d2) <= RGAMSQ || (std::abs(d2) >= GAMSQ)))
                {
                    if (dflag == ZERO)
                    {
                        dh11 = ONE;
                        dh22 = ONE;
                        dflag = -ONE;
                    }
                    else
                    {
                        dh21 = -ONE;
                        dh12 = ONE;
                        dflag = -ONE;
                    }

                    if (std::abs(d2) <= RGAMSQ)
                    {
                        d2 *= GAMSQ;
                        dh21 /= GAM;
                        dh22 /= GAM;
                    }
                    else 
                    {
                        d2 /= GAMSQ;
                        dh21 *= GAM;
                        dh22 *= GAM;
                    }
                };
            };
        }
        
        params[0] = dflag;
        if (dflag < ZERO)
        {
            params[1] = dh11; params[2] = dh21;
            params[3] = dh12; params[4] = dh22;
        }
        else if (dflag == ZERO)
        {
            params[2] = dh21; params[3] = dh12;
        }
        else 
        {
            params[1] = dh11; params[4] = dh22;        
        }
    }

    // LEVEL 2 BLAS



    // LEVEL 3 BLAS

private:

};

} // senkaid::core::matrix


namespace std {
    template <typename TN>
    struct tuple_size<senkaid::core::matrix::RotgParameters<TN>> : std::integral_constant<std::size_t, 4> {};

    template <typename TN>
    struct tuple_element<0, senkaid::core::matrix::RotgParameters<TN>> {
        using type = TN;
    };
    template <typename TN>
    struct tuple_element<1, senkaid::core::matrix::RotgParameters<TN>> {
        using type = TN;
    };
    template <typename TN>
    struct tuple_element<2, senkaid::core::matrix::RotgParameters<TN>> {
        using type = TN;
    };
    template <typename TN>
    struct tuple_element<3, senkaid::core::matrix::RotgParameters<TN>> {
        using type = TN;
    };
}
