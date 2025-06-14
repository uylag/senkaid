#pragma once

#include "../../utils/config/pragmas.hpp"
#include "../../utils/config/root.hpp"

namespace senkaid::core::matrix {
    namespace major {
        enum Major {
            RowMajor,
            ColumnMajor
        };
    }

    template<typename Derived, typename Type>
    struct MatrixBase {
        Derived operator+(const Derived& other) {
            return Derived(static_cast<const Derived&>(*this)) += other;
        }

        Derived operator+(const Type& other) {
            return Derived(static_cast<const Derived&>(*this)) += other;
        }

        Derived operator-(const Derived& other) {
            return Derived(static_cast<const Derived&>(*this)) -= other;
        }

        Derived operator-(const Type& other) {
            return Derived(static_cast<const Derived&>(*this)) += other;
        }

        Derived operator*(const Derived& other) {
            return Derived(static_cast<const Derived&>(*this)) *= other;
        }

 Derived operator*(const Type& other) {
            return Derived(static_cast<const Derived&>(*this)) += other;
        }

        Derived operator/(const Derived& other) {
            return Derived(static_cast<const Derived&>(*this)) /= other;
        }

        Derived operator/(const Type& other) {
            return Derived(static_cast<const Derived&>(*this)) += other;
        }

        Type norm(const char* NormType) {

        } // function
    }; // class MatrixBase
} // namespace senkaid::core::matrix
