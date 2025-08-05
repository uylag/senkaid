#pragma once

#include <cstdint>
#include <iostream>
#include "base.hpp"

namespace senkaid::core::matrix 
{

enum class SDMajor: uint8_t
{
    RowMajor = 0x01,
    ColumnMajor = 0x02
};

template <int Rows = -1, int Columns = -1, typename TN = double, SDMajor Major = SDMajor::RowMajor> 
class SDDenseMatrix : public SDMatrixBase<TN, SDDenseMatrix<Rows, Columns, TN, Major>>
{
public:
    using Base = SDMatrixBase<TN, SDDenseMatrix<Rows, Columns, TN, Major>>;
    using Base::rotg;
    using Base::rot;

    using SDDM = SDDenseMatrix<Rows, Columns, TN, Major>;

    using value_type = TN;
    using index_type = std::size_t;
    using size_type = std::size_t;
    using shape_type = TN*;

    constexpr SDDenseMatrix()
    {
        std::cout << "Constructor with no parameters called.\n";
    }

    

    // FUNCTIONS

    /* TODO:
        static axpy(a, b, c)
        static dot(a, b) with support
            - a as value
            - a as ref (&a, b)
        static sum(Argc...)
        static copy(&a, &b)
        static conjugate(a)
        static norm(a, p_order)
    */

    template <typename TM, typename TO, typename TP>
    requires MatrixScalar<TM, TO> && MatrixScalar<TO, TP>
    constexpr SENKAID_FORCE_INLINE static SDDM axpy(TM a, TO b, TP c);

    template <typename TM, typename TO>
    requires MatrixScalar<TM, TO>
    constexpr SENKAID_FORCE_INLINE static SDDM dot(const TM& a, const TO& b);

    template <typename TM, typename TO>
    requires MatrixScalar<TM, TO>
    constexpr SENKAID_FORCE_INLINE static SDDM dot(TM& a, const TO& b);

    template <typename... Args>
    constexpr SENKAID_FORCE_INLINE static TN sum(Args... args); // usecase: args...
   
    template <typename FROM, typename TO>
    requires Matrix<FROM> && Matrix<TO>
    constexpr SENKAID_FORCE_INLINE static void copy(const FROM& a, TO& b);

    template <typename TM>
    requires Matrix<TM>
    constexpr SENKAID_FORCE_INLINE static SDDM conjugate(const SDDM& a);

    template <typename TM>
    requires Matrix<TM>
    constexpr SENKAID_FORCE_INLINE static SDDM conjugate_inplace(SDDM& a);


private:
    TN* _data;
    size_type _capacity;
    size_type _size;

    friend struct SDMatrixBase<TN, SDDenseMatrix<Rows, Columns, TN, Major>>;
};

}

