// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2016
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <stl2/detail/numeric/inner_product.hpp>
#include "../detail/int128.hpp"
#include "../simple_test.hpp"
#include <deque>
#include <list>
#include <numeric>
#include <stl2/detail/concepts/number.hpp>
#include <stl2/detail/fwd.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include <iostream>

namespace ranges = __stl2;

template <typename U, typename V>
void CHECK_algorithm(const U& u, const V& v)
{
   auto s = std::inner_product(u.begin(), u.end(), v.begin(), ranges::value_type_t<U>{});

   CHECK(s == ranges::inner_product(u.begin(), u.end(), v.begin()));
   CHECK(s == ranges::inner_product(u, v.begin()));
}

template <typename T, typename U, ranges::Callable<T, U> F1, ranges::Callable<T, U> F2>
void CHECK_callable(const std::vector<T>& a, const std::vector<U>& b, F1 f1, F2 f2)
{
   using ranges::value_type_t;
   auto s = std::inner_product(a.begin(), a.end(), b.begin(), T{}, f1, f2);

   CHECK(s == ranges::inner_product(a.begin(), a.end(), b.begin(), T{}, f1, f2));
   CHECK(s == ranges::inner_product(a, b.begin(), T{}, f1, f2));
}

// not sure where (or if) I should sneak this into the headers...
template <typename T>
using mapped_type_t = typename T::mapped_type;

template <typename U, typename V, typename Proj1, typename Proj2>
void CHECK_projection(U u, V v, Proj1 proj1, Proj2 proj2)
{
   using ranges::value_type_t;
   auto s = std::inner_product(u.begin(), u.end(), v.begin(), mapped_type_t<U>{},
                           [](const auto& i, const auto& j) {
                              return std::plus<mapped_type_t<U>>{}(i, j);
                           },
                           [](const auto& i, const auto& j) {
                              return std::multiplies<mapped_type_t<U>>{}(i.first, j.second);
                           });
   CHECK(s == ranges::inner_product(u.begin(), u.end(), v.begin(), mapped_type_t<U>{},
                                    ranges::plus<>{},
                                    ranges::multiplies<>{},
                                    proj1,
                                    proj2));
   CHECK(s == ranges::inner_product(u, v.begin(), mapped_type_t<U>{},
                                    ranges::plus<>{},
                                    ranges::multiplies<>{},
                                    proj1,
                                    proj2));
}

int main()
{
   CHECK_algorithm(std::vector<int>{}, std::vector<int>{});
   CHECK_algorithm(std::deque<int>{}, std::vector<double>{});
   CHECK_algorithm(std::vector<double>{}, std::list<int>{});

   CHECK_algorithm(std::vector<int>{}, std::vector<int>{1});
   CHECK_algorithm(std::vector<int>{}, std::vector<double>{1});
   CHECK_algorithm(std::vector<double>{}, std::vector<int>{1});

   // CHECK_algorithm(std::vector<int>{1, 2, 3}, std::vector<int>{});

   CHECK_algorithm(std::vector<int>{1}, std::vector<int>{2});
   CHECK_algorithm(std::vector<int>{10}, std::deque<float>{3.0f});
   CHECK_algorithm(std::list<double>{4.0}, std::list<int>{7});

   CHECK_algorithm(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                   std::vector<int>{10, 20, 30, 40, 50, 60, 70, 80, 90});
   CHECK_algorithm(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                   std::deque<double>{1.0, 2.1, 3.2, 4.3, 5.4, 6.5, 7.6, 8.7, 9.8, 10.9});
   CHECK_algorithm(std::vector<double>{-7.0, 5.3, 3.1415965},
                   std::list<int>{0xa, 0xb, 0xc, 0xd});

   CHECK_callable<int, int>({0, 1, 2, 3, 4, 5, 6, 7, 8}, {8, 1, 5, 3, 4, 2, 7, 0},
                            [](const auto a, const auto b) {
                              return a < b || b == 0 ? a * b : a / b;
                            },
                            [](const auto a, const auto b) {
                               return a < b ? a + b : a - b;
                            });
   CHECK_callable<int, double>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                               {0, 0.000'01, 0.000'2, 0.003, 0.04, 0.5, 6.0, 70.0, 800.0, 9'000.0},
                               [](const auto a, const auto b) {
                                  return 300 * a * b;
                               },
                               ranges::plus<>{});
   CHECK_callable<double, int>({0, 0.000'01, 0.000'2, 0.003, 0.04, 0.5, 6.0, 70.0, 800.0, 9'000.0},
                               {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                               [](const auto a, const auto b) {
                                  return 300 * a * b;
                               },
                               ranges::plus<>{});

   CHECK_projection(std::unordered_map<int, int>{{0, 0},
                                                 {1, 1},
                                                 {2, 2},
                                                 {3, 3},
                                                 {4, 4},
                                                 {5, 5}},
                    std::unordered_map<std::string, int>{{"hello", 15},
                                                         {"world", 20},
                                                         {"first", 25},
                                                         {"last", 30},
                                                         {"begin", 35},
                                                         {"end", 40}},
                    [](const auto& i) { return i.first; },
                    [](const auto& i) { return i.second; });
   return test_result();
}