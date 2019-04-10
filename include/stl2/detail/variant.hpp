// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015, 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_VARIANT_HPP
#define STL2_DETAIL_VARIANT_HPP

#include <cstddef>
#include <variant>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

// TODO: conditional noexcept for __unchecked_visit?

///////////////////////////////////////////////////////////////////////////
// variant utiilities
//
STL2_OPEN_NAMESPACE {
	// Obtain the index of T in TypeList; substitution failure if T
	// does not appear exactly once.
	template<class T, class TypeList,
		std::size_t I = meta::_v<meta::find_index<TypeList, T>>>
	requires
		(I != meta::_v<meta::npos> &&
		Same<meta::find_index<meta::drop_c<TypeList, I + 1>, T>, meta::npos>)
	constexpr std::size_t __index_of_type = I;

	// Like std::get<I>(v), but with a precondition that v.index() == I
	template<std::size_t I, _SpecializationOf<std::variant> Variant>
	requires (I < std::variant_size_v<__uncvref<Variant>>)
	constexpr decltype(auto) __unchecked_get(Variant&& v) noexcept {
		STL2_EXPECT(v.index() == I);
		return std::get<I>(static_cast<Variant&&>(v));
	}

	// Like std::get<T>(v), but with a precondition that holds_alternative<T>(v)
	template<class T, _SpecializationOf<std::variant> Variant>
	requires requires { __index_of_type<T, meta::as_list<__uncvref<Variant>>>; }
	constexpr decltype(auto) __unchecked_get(Variant&& v) noexcept {
		STL2_EXPECT(std::holds_alternative<T>(v));
		return std::get<T>(static_cast<Variant&&>(v));
	}

	template<class F, class... Vs>
	requires (_SpecializationOf<Vs, std::variant> && ...)
	constexpr decltype(auto) __unchecked_visit(F&& f, Vs&&... vs) {
		(STL2_EXPECT(!vs.valueless_by_exception()), ...);
		return std::visit(static_cast<F&&>(f), static_cast<Vs&&>(vs)...);
	}
} STL2_CLOSE_NAMESPACE

#endif
