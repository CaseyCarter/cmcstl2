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
#ifndef STL2_TUPLE_HPP
#define STL2_TUPLE_HPP

#include <stl2/functional.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/tuple_like.hpp>

STL2_OPEN_NAMESPACE {
	// tagged_tuple
	template <TaggedType... Types>
	using tagged_tuple =
		tagged<tuple<__tagged::element<Types>...>, __tagged::specifier<Types>...>;

	// make_tagged_tuple
	template <TagSpecifier... Tags, class... Types>
	constexpr auto make_tagged_tuple(Types&&... ts) {
		return tagged_tuple<Tags(__unwrap<Types>)...>{
			std::forward<Types>(ts)...
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
