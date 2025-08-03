#pragma once

#include "base.hpp"

namespace senkaid::core::matrix {

    template<
        int Rows_ = -1, 
        int Cols_ = -1,
        typename Type_ = double,
        MatrixType MatrixType_ = MatrixType::Sparse,
        Major Major_ = Major::RowMajor
    >
    class Matrix : public MatrixBase<Matrix<Rows_, Cols_, Type_, MatrixType_, Major_>, Type_> {

    };

} // namespace senkaid::core::matrix

