// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef RANGES_V3_ALGORITHM_SAMPLE_HPP
#define RANGES_V3_ALGORITHM_SAMPLE_HPP

#include <stl2/iterator.hpp>
#include <stl2/random.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/randutils.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	namespace __sample {
		template <class I, class S, class O, class Gen>
		concept bool constraint =
			InputIterator<I> && Sentinel<S, I> && WeaklyIncrementable<O> &&
			IndirectlyCopyable<I, O> &&
			UniformRandomNumberGenerator<remove_reference_t<Gen>> &&
			ConvertibleTo<result_of_t<Gen&()>, difference_type_t<I>>;

		template <class I, class S, class O, class Gen>
		requires
			constraint<I, S, O, Gen>
		tagged_pair<tag::in(I), tag::out(O)>
		sized_impl(I first, S last, difference_type_t<I> pop_size,
			O out, difference_type_t<I> n, Gen& gen)
		{
			__stl2::uniform_int_distribution<difference_type_t<I>> dist;
			using param_t = typename decltype(dist)::param_type;
			if (n > pop_size) {
				n = pop_size;
			}
			for (; n > 0 && first != last; ++first) {
				if (dist(gen, param_t{0, --pop_size}) < n) {
					--n;
					*out = *first;
					++out;
				}
			}
			return {__stl2::move(first), __stl2::move(out)};
		}
	}

	template <class I, class S, class O,
		class Gen = detail::default_random_engine&>
	requires
		(ForwardIterator<I> || SizedSentinel<S, I>) &&
		__sample::constraint<I, S, O, Gen>
	tagged_pair<tag::in(I), tag::out(O)>
	inline sample(I first, S last, O out, difference_type_t<I> n,
		Gen&& gen = detail::get_random_engine())
	{
		auto k = __stl2::distance(first, last);
		return __sample::sized_impl(__stl2::move(first), __stl2::move(last),
			k, __stl2::move(out), n, gen);
	}

	template <class I, class S, RandomAccessIterator O,
		class Gen = detail::default_random_engine&>
	requires
		!(ForwardIterator<I> || SizedSentinel<S, I>) &&
		__sample::constraint<I, S, O, Gen>
	tagged_pair<tag::in(I), tag::out(O)>
	sample(I first, S last, O out, difference_type_t<I> n,
		Gen&& gen = detail::get_random_engine())
	{
		if (n <= 0) {
			goto done;
		}
		for (difference_type_t<I> i = 0; i < n; (void)++i, ++first) {
			if (first == last) {
				goto done;
			}
			out[i] = *first;
		}
		__stl2::uniform_int_distribution<difference_type_t<I>> dist;
		using param_t = typename decltype(dist)::param_type;
		for (auto pop_size = n; first != last; (void)++first, ++pop_size) {
			auto const i = dist(gen, param_t{0, pop_size});
			if (i < n) {
				out[i] = *first;
			}
		}
		out += n;
	done:
		return {__stl2::move(first), __stl2::move(out)};
	}

	template <class I, class S, class ORng,
		class Gen = detail::default_random_engine&>
	requires
		(ForwardIterator<I> || SizedSentinel<S, I>) &&
		(ForwardRange<ORng> || SizedRange<ORng>) &&
		__sample::constraint<I, S, iterator_t<ORng>, Gen>
	inline tagged_pair<tag::in(I), tag::out(safe_iterator_t<ORng>)>
	sample(I first, S last, ORng&& out,
		Gen&& gen = detail::get_random_engine())
	{
		auto k = __stl2::distance(first, last);
		return __sample::sized_impl(__stl2::move(first), __stl2::move(last),
			k, __stl2::begin(out), __stl2::distance(out), gen);
	}

	template <class I, class S, class ORng,
		class Gen = detail::default_random_engine&>
	requires
		RandomAccessIterator<iterator_t<ORng>> &&
		!(ForwardIterator<I> || SizedSentinel<S, I>) &&
		(ForwardRange<ORng> || SizedRange<ORng>) &&
		__sample::constraint<I, S, iterator_t<ORng>, Gen>
	inline tagged_pair<tag::in(I), tag::out(safe_iterator_t<ORng>)>
	sample(I first, S last, ORng&& out,
		Gen&& gen = detail::get_random_engine())
	{
		return __stl2::sample(__stl2::move(first), __stl2::move(last),
			__stl2::begin(out), __stl2::distance(out), __stl2::forward<Gen>(gen));
	}

	template <class Rng, RandomAccessIterator O,
		class Gen = detail::default_random_engine&>
	requires
		!(ForwardRange<Rng> || SizedRange<Rng>) &&
		__sample::constraint<iterator_t<Rng>, sentinel_t<Rng>, O, Gen>
	inline tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
	sample(Rng&& rng, O out, difference_type_t<iterator_t<Rng>> n,
		Gen&& gen = detail::get_random_engine())
	{
		return __stl2::sample(__stl2::begin(rng), __stl2::end(rng),
			__stl2::move(out), n, __stl2::forward<Gen>(gen));
	}

	template <class Rng, class O, class Gen = detail::default_random_engine&>
	requires
		(ForwardRange<Rng> || SizedRange<Rng>) &&
		__sample::constraint<iterator_t<Rng>, sentinel_t<Rng>, O, Gen>
	inline tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(O)>
	sample(Rng&& rng, O out, difference_type_t<iterator_t<Rng>> n,
		Gen&& gen = detail::get_random_engine())
	{
		return __sample::sized_impl(__stl2::begin(rng), __stl2::end(rng),
			__stl2::distance(rng), __stl2::move(out), n, __stl2::forward<Gen>(gen));
	}

	template <class IRng, class ORng, class Gen = detail::default_random_engine&>
	requires
		RandomAccessIterator<iterator_t<ORng>> &&
		!(ForwardRange<IRng> || SizedRange<IRng>) &&
		(ForwardRange<ORng> || SizedRange<ORng>) &&
		__sample::constraint<iterator_t<IRng>, sentinel_t<IRng>, iterator_t<ORng>, Gen>
	inline tagged_pair<
		tag::in(safe_iterator_t<IRng>),
		tag::out(safe_iterator_t<ORng>)>
	sample(IRng&& rng, ORng&& out, Gen&& gen = detail::get_random_engine())
	{
		return __stl2::sample(__stl2::begin(rng), __stl2::end(rng),
			__stl2::begin(out), __stl2::distance(out), __stl2::forward<Gen>(gen));
	}

	template <class IRng, class ORng, class Gen = detail::default_random_engine&>
	requires
		(ForwardRange<IRng> || SizedRange<IRng>) &&
		(ForwardRange<ORng> || SizedRange<ORng>) &&
		__sample::constraint<iterator_t<IRng>, sentinel_t<IRng>,
			iterator_t<ORng>, Gen>
	inline tagged_pair<
		tag::in(safe_iterator_t<IRng>),
		tag::out(safe_iterator_t<ORng>)>
	sample(IRng&& rng, ORng&& out, Gen&& gen = detail::get_random_engine())
	{
		return __sample::sized_impl(__stl2::begin(rng), __stl2::end(rng),
			__stl2::distance(rng), __stl2::begin(out), __stl2::distance(out),
			__stl2::forward<Gen>(gen));
	}
} STL2_CLOSE_NAMESPACE

#endif
