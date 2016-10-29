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
	///////////////////////////////////////////////////////////////////////////
	// <functional> imports
	//
	using std::result_of_t;

	using std::reference_wrapper;
	using std::ref;
	using std::cref;

	using std::plus;
	using std::minus;
	using std::multiplies;
	using std::divides;
	using std::modulus;
	using std::negate;

	using std::logical_and;
	using std::logical_or;
	using std::logical_not;

	using std::bit_and;
	using std::bit_or;
	using std::bit_xor;
	using std::bit_not;

	using std::unary_negate;
	using std::binary_negate;
	using std::not1;
	using std::not2;

	using std::is_bind_expression;
	using std::is_placeholder;
	using std::bind;
	namespace placeholders = std::placeholders;

	using std::mem_fn;
	using std::bad_function_call;

	using std::function;
	using std::hash;

	template <class T>
	struct __unwrap_ : __unwrap_<decay_t<T>> {};
	template <_Decayed T>
	struct __unwrap_<T> { using type = T; };
	template <class T>
	struct __unwrap_<reference_wrapper<T>> { using type = T&; };
	template <class T>
	using __unwrap = meta::_t<__unwrap_<T>>;

	///////////////////////////////////////////////////////////////////////////
	// identity [func.identity]
	//
	struct identity {
		template <class T>
		constexpr T&& operator()(T&& t) const noexcept {
			return __stl2::forward<T>(t);
		}

		using is_transparent = true_type;
	};
} STL2_CLOSE_NAMESPACE

#endif
