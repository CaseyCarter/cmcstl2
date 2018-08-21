// cmcstl2 - A concept-enabled C++ standard library
//
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
#ifndef STL2_DETAIL_MEMORY_CONSTRUCT_AT_HPP
#define STL2_DETAIL_MEMORY_CONSTRUCT_AT_HPP

#include <new>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/memory/addressof.hpp>

STL2_OPEN_NAMESPACE {
	template <class T, class... Args>
	requires Constructible<T, Args...>
	void __construct_at(T& t, Args&&... args) {
		::new(const_cast<void*>(static_cast<const volatile void*>(detail::addressof(t))))
			T(std::forward<Args>(args)...);
	}

	template <DefaultConstructible T>
	void __default_construct_at(T& t)
	{
		::new(const_cast<void*>(static_cast<const volatile void*>(detail::addressof(t))))
			T;
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_CONSTRUCT_AT_HPP
