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
#ifndef STL2_DETAIL_CONCEPTS_OBJECT_ASSIGNABLE_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_ASSIGNABLE_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// assignable_from [concepts.lib.corelang.assignable]
	//
	template<class LHS, class RHS>
	META_CONCEPT assignable_from =
		std::is_lvalue_reference_v<LHS> &&
#if 0 // TODO: investigate making this change
		common_reference_with<LHS, RHS> &&
#else
		common_reference_with<
			const std::remove_reference_t<LHS>&,
			const std::remove_reference_t<RHS>&> &&
#endif
		requires(LHS lhs, RHS&& rhs) {
			{ lhs = static_cast<RHS&&>(rhs) } -> STL2_RVALUE_REQ(same_as<LHS>);
		};
} STL2_CLOSE_NAMESPACE

#endif
