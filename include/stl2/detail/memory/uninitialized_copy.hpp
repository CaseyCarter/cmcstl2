// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt r2 copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_MEMR2Y_UNINITIALIZED_COPY_HPP
#define STL2_DETAIL_MEMR2Y_UNINITIALIZED_COPY_HPP

#include <new>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/return_types.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/memory/construct_at.hpp>
#include <stl2/detail/memory/destroy.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	template<class I, class O>
	struct uninitialized_copy_result : __in_out_result<I, O> {};

	struct __uninitialized_copy_fn {
		template <InputIterator I, Sentinel<I> S1, __NoThrowForwardIterator O, __NoThrowSentinel<O> S2>
		requires Constructible<iter_value_t<O>, iter_reference_t<I>>
		uninitialized_copy_result<I, O> operator()(I ifirst, S1 ilast, O ofirst, S2 olast) const
		{
			auto guard = detail::destroy_guard<O>{ofirst};
			for (; ifirst != ilast && ofirst != olast; ++ofirst, (void)++ifirst) {
				__stl2::__construct_at(*ofirst, *ifirst);
			}
			guard.release();
			return {std::move(ifirst), std::move(ofirst)};
		}

		template <InputRange R1, __NoThrowForwardRange R2>
		requires Constructible<iter_value_t<iterator_t<R2>>, iter_reference_t<iterator_t<R1>>>
		uninitialized_copy_result<safe_iterator_t<R1>, safe_iterator_t<R2>>
		operator()(R1&& r1, R2&& r2) const
		{
			return (*this)(__stl2::begin(r1), __stl2::end(r1), __stl2::begin(r2), __stl2::end(r2));
		}
	};

	inline constexpr __uninitialized_copy_fn uninitialized_copy {};

	template<class I, class O>
	using uninitialized_copy_n_result = uninitialized_copy_result<I, O>;

	struct __uninitialized_copy_n_fn {
		template <InputIterator I, __NoThrowForwardIterator O, __NoThrowSentinel<O> S>
		requires Constructible<iter_value_t<O>, iter_reference_t<I>>
		uninitialized_copy_n_result<I, O>
		operator()(I first, iter_difference_t<I> n, O ofirst, S olast) const
		{
			auto result = __stl2::uninitialized_copy(
				__stl2::make_counted_iterator(first, n),
				default_sentinel{}, std::move(ofirst), std::move(olast));
			return {result.in.base(), result.out};
		}
	};

	inline constexpr __uninitialized_copy_n_fn uninitialized_copy_n {};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMR2Y_UNINITIALIZED_COPY_HPP
