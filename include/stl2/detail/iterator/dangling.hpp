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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	////////////////////////////////////////////////////////////////////////////
	// dangling
	//
	struct dangling {
		constexpr dangling() noexcept = default;
		template<class... Args>
		constexpr dangling(Args&&...) noexcept {}
	};

	template<Range R>
	inline constexpr bool __is_forwarding_range = _ForwardingRange<R>;

	template<Range R, class U>
	using __maybe_dangling = meta::if_c<__is_forwarding_range<R>, U, dangling>;

	template<Range R>
	using safe_iterator_t = __maybe_dangling<R, iterator_t<R>>;

} STL2_CLOSE_NAMESPACE

#endif
