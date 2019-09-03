// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_ORBIT_HPP
#define STL2_VIEW_ORBIT_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/unreachable.hpp>
#include <stl2/detail/semiregular_box.hpp>

STL2_OPEN_NAMESPACE {
    namespace ext {
        template <move_constructible_object T, copy_constructible_object F>
        requires assignable_from<T&, invoke_result_t<F&, T&>>
        struct orbit_view
        private:
            STL2_NO_UNIQUE_ADDRESS semiregular_box<T> value_;
            STL2_NO_UNIQUE_ADDRESS semiregular_box<F> f_;
        public:
            orbit_view() = default;
            template <class U>
            requires convertible_to<U, T>
            constexpr explicit orbit_view(U&& u) noexcept(std::is_nothrow_constructible_v<T, U>)
            : value_{std::in_place_t, static_cast<U&&>(u)} {}

            constexpr auto begin() const noexcept {
                if constexpr (copy_constructible_object<T>) {
                    // store copies in iterators, model forward_range
                    struct iterator {
                        iterator() = default;
                        constexpr explicit iterator(const orbit_view& v)
                        noexcept(std::is_nothrow_copy_constructible_v<T> &&
                            std::is_nothrow_copy_constructible_v<F>)
                        : value_{v.value_}, f_{v.f_} {}

                        T operator*() const
                        noexcept(std::is_nothrow_copy_constructible_v<T>) {
                            return value_;
                        }
                        iterator& operator++()
                        noexcept(noexcept(value_.get() = f_.get()(value_.get()))) {
                            value_.get() = f_.get()(value_.get());
                            return *this;
                        }
                        iterator operator++(int) {
                            auto tmp = *this;
                            ++*this;
                            return tmp;
                        }
                    private:
                        STL2_NO_UNIQUE_ADDRESS semiregular_box<T> value_;
                        STL2_NO_UNIQUE_ADDRESS semiregular_box<F> f_;
                    };
                    return iterator{*this};
                } else {
                    // model input_range
                    struct iterator {
                        iterator() = default;
                        constexpr explicit iterator(const orbit_view& v) noexcept
                        : view_{std::addressof(v)} {}
                    private:
                        orbit_view* view_ = nullptr;
                    };
                    return iterator{*this};
                }
            }
            constexpr auto begin() const noexcept requires copy_constructible_object<T> {

            }
            constexpr auto end() const noexcept { return unreachable_sentinel; }
        };
    } // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
