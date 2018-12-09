// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_DANGLING_HPP
#define STL2_DETAIL_ITERATOR_DANGLING_HPP

#include <type_traits>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	////////////////////////////////////////////////////////////////////////////
	// dangling
	// Not to spec: Kill it with fire.
	//
	template<ext::CopyConstructibleObject T>
	class dangling {
		T value;
	public:
		constexpr dangling()
		noexcept(std::is_nothrow_default_constructible<T>::value)
		requires DefaultConstructible<T>
		: value{}
		{}
		constexpr dangling(T t)
		noexcept(std::is_nothrow_move_constructible<T>::value)
		: value(std::move(t))
		{}
		constexpr T get_unsafe() const
		noexcept(std::is_nothrow_copy_constructible<T>::value)
		{
			return value;
		}
	};

	template<Range R, ext::CopyConstructibleObject U>
	using __maybe_dangling =
		meta::if_<meta::is_trait<meta::defer<__begin_t, R>>, U, dangling<U>>;

	template<Range R>
	using safe_iterator_t = __maybe_dangling<R, iterator_t<R>>;

} STL2_CLOSE_NAMESPACE

#endif
