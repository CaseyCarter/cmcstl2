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
#ifndef STL2_DETAIL_TUPLE_LIKE_HPP
#define STL2_DETAIL_TUPLE_LIKE_HPP

#include <tuple>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	using std::tuple_size;
	using std::tuple_element;
	using std::get;

	constexpr std::size_t tuple_not_found = -1;
	static_assert(tuple_not_found == meta::_v<meta::npos>);

	template <class T, class U>
	constexpr std::size_t tuple_find = tuple_not_found;

	template <class T, class U>
	requires !Same<U, __uncvref<U>>
	constexpr std::size_t tuple_find<T, U> =
		tuple_find<T, __uncvref<U>>;

	namespace detail {
		namespace adl {
			using std::get;

			template <std::size_t I, class T>
			constexpr auto adl_get(T&& t)
			noexcept(noexcept(get<I>(static_cast<T&&>(t))))
			-> decltype(get<I>(static_cast<T&&>(t)))
			{
				return get<I>(static_cast<T&&>(t));
			}
		} // namespace adl
		using adl::adl_get;
	} // namespace detail
} STL2_CLOSE_NAMESPACE

#endif
