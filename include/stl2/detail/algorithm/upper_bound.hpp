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
#ifndef STL2_DETAIL_ALGORITHM_UPPER_BOUND_HPP
#define STL2_DETAIL_ALGORITHM_UPPER_BOUND_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/partition_point.hpp>
#include <stl2/detail/concepts/callable.hpp>

///////////////////////////////////////////////////////////////////////////
// upper_bound [upper.bound]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		template <class C, class T>
		struct __upper_bound_fn {
			__f<C> comp_;
			const T& value_;

			constexpr __upper_bound_fn(C comp, const T& value)
			: comp_(std::ref(comp)), value_(value) {}

			constexpr bool operator()(auto&& i) const {
				return !__stl2::invoke(comp_, value_, i);
			}
		};
	} // namespace detail

	namespace ext {
		template <ForwardIterator I, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<I, Proj>> Comp = ranges::less<>>
		constexpr I upper_bound_n(I&& first, iter_difference_t<I> n, const T& value,
			Comp comp = Comp{}, Proj proj = Proj{})
		{
			return __stl2::ext::partition_point_n(std::forward<I>(first), n,
				detail::__upper_bound_fn<Comp, T>{std::ref(comp), value},
				std::ref(proj));
		}
	} // namespace ext

	class __upper_bound_fn {
		template<ForwardIterator I, Sentinel<I> S, class T, class Proj, class Comp>
		static constexpr I impl(I first, S last, const T& value, Comp comp, Proj proj)
		{
			return __stl2::partition_point(std::move(first), std::move(last),
				detail::__upper_bound_fn<Comp, T>{std::ref(comp), value},
				std::ref(proj));
		}

		template<ForwardIterator I, SizedSentinel<I> S, class T, class Proj, class Comp>
		static constexpr I impl(I first_, S last, const T& value, Comp comp, Proj proj)
		{
			auto first = std::forward<I>(first_);
			auto n = __stl2::distance(first, std::move(last));
			return __stl2::ext::upper_bound_n(std::move(first), n, value, std::ref(comp), std::ref(proj));
		}

		template<ForwardRange R, class T, class Proj, class Comp>
		static constexpr safe_iterator_t<R> impl(R&& r, const T& value, Comp comp, Proj proj)
		{
			return __upper_bound_fn::impl(__stl2::begin(r), __stl2::end(r),
				value, std::ref(comp), std::ref(proj));
		}

		template<ForwardRange R, class T, class Proj, class Comp>
		requires SizedRange<R>
		static constexpr safe_iterator_t<R> impl(R&& r, const T& value, Comp comp, Proj proj)
		{
			return __stl2::ext::upper_bound_n(__stl2::begin(r), __stl2::distance(r),
				value, std::ref(comp), std::ref(proj));
		}
	public:
		template<ForwardIterator I, Sentinel<I> S, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<I, Proj>> Comp = ranges::less<>>
		constexpr I operator()(I first, S last, const T& value, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __upper_bound_fn::impl(std::move(first), std::move(last), value,
				std::ref(comp), std::ref(proj));
		}

		template<ForwardRange R, class T, class Proj = identity,
			IndirectStrictWeakOrder<const T*, projected<iterator_t<R>, Proj>> Comp = ranges::less<>>
		constexpr safe_iterator_t<R>
		operator()(R&& r, const T& value, Comp comp = Comp{}, Proj proj = Proj{}) const
		{
			return __upper_bound_fn::impl(std::forward<R>(r), value, std::ref(comp), std::ref(proj));
		}
	};

	inline constexpr __upper_bound_fn upper_bound {};
} STL2_CLOSE_NAMESPACE

#endif
