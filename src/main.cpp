#include "core/matrix/base.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <senkaid/core/matrix/dense.hpp>
#include <tuple>
#include <chrono>

template <typename TN>
void output_rotg(senkaid::core::matrix::RotgParameters<TN> array)
{
    auto [r, c, s, z] = array;
    std::cout << "r: " << r << "\nc: " << c << "\ns: " << s << "\nz: " << z << "\n\n";
}

template <typename TN>
void output_array(TN* array, std::size_t size)
{
    std::cout << "Array data: ";
    for (int i = 0; i < size; ++i)
    {
        std::cout << array[i] << (i < size - 1 ? " " : "");
    }
    std::cout << ";\n";
}

using namespace std::chrono;

using namespace senkaid::core::matrix;
auto rotg = static_cast<
    senkaid::core::matrix::RotgParameters<double> (*)(double, double)
>(&SDDenseMatrix<>::rotg);

auto rot = static_cast<
    void (*)(double*, double*, std::size_t, double, double)
>(&SDDenseMatrix<>::template rot<double>);

auto rotmg = static_cast<
    void (*)(double&, double&, double&, double&, double*)
>(&SDDenseMatrix<>::rotmg);

int main() 
{
//  SDDEnseMatrix<> mat;
//  output_rotg(rotg(3.0, 4.0));
//  output_rotg(rotg(1e+150, 1e+150));

//  auto start = high_resolution_clock::now();

//  for (int i = 0; i < 1'000'000'000; ++i)
//  {
//      if (i % 100'000'000 == 0)
//      {
//          std::cout << "Iteration in 100million passed\n";
//      }

//      auto a = std::clamp(static_cast<double>(rand()), 0.0, 1'000'000.0);
//      auto b = std::clamp(static_cast<double>(rand()), 0.0, 1'000'000.0);
//      volatile auto tup = rotg(a, b);
//  }

//  auto end = high_resolution_clock::now();

//  auto duration = duration_cast<milliseconds>(end - start);

//  std::cout << "\nDuration of 1'000'000'000 execution: " << duration.count() << "\n\n";
    
    auto [_, c, s, __] = rotg(4.0, 5.0);

    std::array<double, 4> a = {1.0, 2.0, 3.0, 4.0};
    std::array<double, 4> b = {5.0, 6.0, 7.0, 8.0};

    double d1 = 1.0, d2 = 2.0, x1 = 3.0, y1 = 4.0;
    double params[5]{};

    rotmg(x1, y1, d1, d2, params);
    std::cout << "x: " << x1 << "\ty: " << y1 << "\nd1: " << d1 << "\td2: " << d2 << '\n';
    output_array(params, 5);

    return 0;
}
