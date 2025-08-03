#pragma once

#include <cstdint>
#include "base.hpp"

namespace senkaid::core::matrix 
{

enum class SDMajor
{
    RowMajor,
    ColumnMajor
};

template <typename Rows, typename Columns, typename TN = double, SDMajor Major = SDMajor::RowMajor> 
class SDDEnseMatrix : public SDMatrixBase<TN, SDDEnseMatrix<Rows, Columns, TN, Major>>
{

};

}

