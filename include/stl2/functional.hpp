// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_FUNCTIONAL_HPP
#define STL2_FUNCTIONAL_HPP

#include <functional>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/functional/comparisons.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/functional/not_fn.hpp>

STL2_OPEN_NAMESPACE {
	template<class T>
	struct __unwrap_ : __unwrap_<std::decay_t<T>> {};
	template<_Decayed T>
	struct __unwrap_<T> { using type = T; };
	template<class T>
	struct __unwrap_<reference_wrapper<T>> { using type = T&; };
	template<class T>
	struct __unwrap_<std::reference_wrapper<T>> { using type = T&; };
	template<class T>
	using __unwrap = meta::_t<__unwrap_<T>>;

	///////////////////////////////////////////////////////////////////////////
	// identity [func.identity]
	//
	struct identity {
		template<class T>
		constexpr T&& operator()(T&& t) const noexcept {
			return std::forward<T>(t);
		}

		using is_transparent = std::true_type;
	};
} STL2_CLOSE_NAMESPACE

#endif
