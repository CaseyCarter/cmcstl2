// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013-2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_COPY_BACKWARD_HPP
#define STL2_DETAIL_ALGORITHM_COPY_BACKWARD_HPP

#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

///////////////////////////////////////////////////////////////////////////
// copy_backward [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using copy_backward_result = __in_out_result<I, O>;

	struct __copy_backward_fn : private __niebloid {
		template<bidirectional_iterator I1, sentinel_for<I1> S1, bidirectional_iterator I2>
		requires indirectly_copyable<I1, I2>
		constexpr copy_backward_result<I1, I2>
		operator()(I1 first, S1 sent, I2 out) const {
			auto last = next(first, static_cast<S1&&>(sent));
			auto i = last;
			while (i != first) {
				*--out = *--i;
			}
			return {static_cast<I1&&>(last), static_cast<I2&&>(out)};
		}

		template<bidirectional_range R, bidirectional_iterator I>
		constexpr copy_backward_result<safe_iterator_t<R>, I>
		operator()(R&& r, I result) const {
			return (*this)(begin(r), end(r), static_cast<I&&>(result));
		}
	};

	inline constexpr __copy_backward_fn copy_backward{};
} STL2_CLOSE_NAMESPACE

#endif
