// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_CONSTRUCT_DESTRUCT_HPP
#define STL2_DETAIL_CONSTRUCT_DESTRUCT_HPP

#include <new>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		struct destruct_fn {
			template<Destructible T>
			void operator()(T& t) const noexcept {
				t.~T();
			}

			template<Destructible T, std::size_t N>
			void operator()(T (&a)[N]) const noexcept {
				std::size_t i = N;
				while (i > 0) {
					a[--i].~T();
				}
			}
		};
		namespace {
			inline constexpr destruct_fn destruct{};
		}

		struct construct_fn {
			template<class T, class... Args>
			requires Constructible<T, Args...>
			void operator()(T& t, Args&&... args) const
			noexcept(is_nothrow_constructible<T, Args...>::value)
			{
				::new(static_cast<void*>(&t))
					T{std::forward<Args>(args)...};
			}
		};
		namespace {
			inline constexpr construct_fn construct{};
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
