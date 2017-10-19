// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_ITERATOR_RANGE_HPP
#define STL2_VIEW_ITERATOR_RANGE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/utility.hpp>

#include <tuple>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <Iterator I, Sentinel<I> S = I>
		struct iterator_range
			: tagged_pair<tag::begin(I), tag::end(S)>
			, view_interface<iterator_range<I, S>>
		{
			using iterator = I;
			using sentinel = S;

			iterator_range() = default;

			constexpr iterator_range(I i, S s)
			: tagged_pair<tag::begin(I), tag::end(S)>{i, s} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr iterator_range(iterator_range<X, Y> r)
			: iterator_range{r.begin(), r.end()} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr iterator_range(pair<X, Y> r)
			: iterator_range{r.first, r.second} {}

			template <Range R>
			requires ConvertibleTo<iterator_t<R>, I> && ConvertibleTo<sentinel_t<R>, S>
			constexpr iterator_range(R& r)
			: iterator_range{__stl2::begin(r), __stl2::end(r)} {}

			template <Constructible<const I&> X, Constructible<const S&> Y>
			constexpr operator pair<X, Y>() const {
				return pair<X, Y>{this->first, this->second};
			}

			constexpr bool empty() const {
				return this->first == this->second;
			}
		};

		template <Range R>
		iterator_range(R&) -> iterator_range<iterator_t<R>, sentinel_t<R>>;

		template <Iterator I, Sentinel<I> S = I>
		struct sized_iterator_range;
	} // namespace ext

	namespace detail {
		template <Iterator I, Sentinel<I> S>
		struct sized_iterator_range_base
		: ext::view_interface<ext::sized_iterator_range<I, S>>
		{
			sized_iterator_range_base() = default;

			constexpr sized_iterator_range_base(I i, S s, difference_type_t<I> n)
			: rng_{i, s}, size_{n} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr sized_iterator_range_base(sized_iterator_range_base<X, Y> r)
			: sized_iterator_range_base{r.begin(), r.end(), r.size()} {}

			template <SizedRange R>
			requires Constructible<ext::iterator_range<I, S>, R&>
			constexpr sized_iterator_range_base(R& r)
			: sized_iterator_range_base{__stl2::begin(r), __stl2::end(r), __stl2::size(r)} {}

			constexpr difference_type_t<I> size() const noexcept {
				return size_;
			}
		protected:
			ext::iterator_range<I, S> rng_;
		private:
			difference_type_t<I> size_;
		};

		template <Iterator I, SizedSentinel<I> S>
		struct sized_iterator_range_base<I, S>
		: ext::view_interface<ext::sized_iterator_range<I, S>>
		{
			sized_iterator_range_base() = default;

			constexpr sized_iterator_range_base(I i, S s, difference_type_t<I> n)
			: rng_{i, s} {
				STL2_EXPECT(n == s - i);
			}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr sized_iterator_range_base(sized_iterator_range_base<X, Y> r)
			: rng_{r.begin(), r.end()} {}

			template <SizedRange R>
			requires Constructible<ext::iterator_range<I, S>, R&>
			constexpr sized_iterator_range_base(R& r)
			: rng_{__stl2::begin(r), __stl2::end(r)} {}

			constexpr difference_type_t<I> size() const {
				return __stl2::distance(rng_);
			}
		protected:
			ext::iterator_range<I, S> rng_;
		};
	} // namespace detail

	namespace ext {
		template <Iterator I, Sentinel<I> S>
		struct sized_iterator_range
		: detail::sized_iterator_range_base<I, S>
		{
			using iterator = I;
			using sentinel = S;

			sized_iterator_range() = default;

			using detail::sized_iterator_range_base<I, S>::sized_iterator_range_base;

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr sized_iterator_range(pair<X, Y> r, difference_type_t<I> n)
			: sized_iterator_range{r.first, r.second, n} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr sized_iterator_range(iterator_range<X, Y> r, difference_type_t<I> n)
			: sized_iterator_range{r.begin(), r.end(), n} {}

			template <Constructible<const I&> X, Constructible<const S&> Y>
			constexpr operator pair<X, Y>() const {
				return pair<X, Y>{this->rng_.begin(), this->rng_.end()};
			}

			template <Constructible<const I&> X, Constructible<const S&> Y>
			constexpr operator iterator_range<X, Y>() const {
				return this->rng_;
			}

			constexpr operator iterator_range<I, S> const &() const & noexcept {
				return this->rng_;
			}

			constexpr I begin() const {
				return this->rng_.begin();
			}

			constexpr S end() const {
				return this->rng_.end();
			}
		};

		template <Iterator I, Sentinel<I> S>
		sized_iterator_range(I, S, difference_type_t<I>) -> sized_iterator_range<I, S>;

		template <Iterator I, Sentinel<I> S>
		sized_iterator_range(pair<I, S>, difference_type_t<I>) -> sized_iterator_range<I, S>;

		template <Iterator I, Sentinel<I> S>
		sized_iterator_range(iterator_range<I, S>, difference_type_t<I>) -> sized_iterator_range<I, S>;

		template <Iterator I, SizedSentinel<I> S>
		sized_iterator_range(pair<I, S>) -> sized_iterator_range<I, S>;

		template <SizedRange R>
		sized_iterator_range(R&) -> sized_iterator_range<iterator_t<R>, sentinel_t<R>>;

		template <std::size_t N, class I, class S>
		requires N < 2
		constexpr decltype(auto) get(iterator_range<I, S>&& r) noexcept {
			if constexpr (N == 0)
				return std::move(r).begin();
			else
				return std::move(r).end();
		}
		template <std::size_t N, class I, class S>
		requires N < 2
		constexpr decltype(auto) get(iterator_range<I, S>& r) noexcept {
			if constexpr (N == 0)
				return r.begin();
			else
				return r.end();
		}
		template <std::size_t N, class I, class S>
		requires N < 2
		constexpr decltype(auto) get(const iterator_range<I, S>& r) noexcept {
			if constexpr (N == 0)
				return r.begin();
			else
				return r.end();
		}
		template <std::size_t N, class I, class S>
		requires N < 3
		constexpr auto get(const sized_iterator_range<I, S>& r) noexcept {
			if constexpr (N == 0)
				return r.begin();
			else if constexpr (N == 1)
				return r.end();
			else
				return r.size();
		}
	}
} STL2_CLOSE_NAMESPACE

namespace std {
	template <class I, class S>
	struct tuple_size<::__stl2::ext::iterator_range<I, S>>
	  : std::integral_constant<size_t, 2> {};
	template <class I, class S>
	struct tuple_element<0, ::__stl2::ext::iterator_range<I, S>>
	{ using type = I; };
	template <class I, class S>
	struct tuple_element<1, ::__stl2::ext::iterator_range<I, S>>
	{ using type = S; };

	template <class I, class S>
	struct tuple_size<::__stl2::ext::sized_iterator_range<I, S>>
	  : std::integral_constant<size_t, 3> {};
	template <class I, class S>
	struct tuple_element<0, ::__stl2::ext::sized_iterator_range<I, S>>
	{ using type = I; };
	template <class I, class S>
	struct tuple_element<1, ::__stl2::ext::sized_iterator_range<I, S>>
	{ using type = S; };
	template <class I, class S>
	struct tuple_element<2, ::__stl2::ext::sized_iterator_range<I, S>>
	{ using type = ::__stl2::difference_type_t<I>; };
}

#endif
