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
#ifndef STL2_DETAIL_ITERATOR_UNREACHABLE_HPP
#define STL2_DETAIL_ITERATOR_UNREACHABLE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/increment.hpp>

///////////////////////////////////////////////////////////////////////////
// unreachable_sentinel_t [unreachable_sentinel_t.sentinel]
//
STL2_OPEN_NAMESPACE {
	struct unreachable_sentinel_t {
		template<weakly_incrementable WI>
		friend constexpr bool operator==(unreachable_sentinel_t, const WI&) noexcept {
			return false;
		}
		template<weakly_incrementable WI>
		friend constexpr bool operator==(const WI&, unreachable_sentinel_t) noexcept {
			return false;
		}

		template<weakly_incrementable WI>
		friend constexpr bool operator!=(unreachable_sentinel_t, const WI&) noexcept {
			return true;
		}
		template<weakly_incrementable WI>
		friend constexpr bool operator!=(const WI&, unreachable_sentinel_t) noexcept {
			return true;
		}
	};

	inline constexpr unreachable_sentinel_t unreachable_sentinel {};
} STL2_CLOSE_NAMESPACE

#endif
