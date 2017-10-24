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
	namespace detail {
		template <bool B>
		concept bool True = B;
	}

	namespace ext {
		template <Iterator I, Sentinel<I> S = I, bool Sized = models::SizedSentinel<S, I>>
		struct subrange
		: tagged_tuple<tag::begin(I), tag::end(S)>
		, view_interface<subrange<I, S, Sized>>
		{
			using iterator = I;
			using sentinel = S;

			subrange() = default;

			constexpr subrange(I i, S s)
			: tagged_tuple<tag::begin(I), tag::end(S)>{i, s} {}

			constexpr subrange(I i, S s, difference_type_t<I> n) requires Sized
			: subrange{i, s} {
				STL2_EXPECT(n == s - i);
			}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y, bool B>
			constexpr subrange(subrange<X, Y, B> r)
			: subrange{r.begin(), r.end()} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y, bool B>
			constexpr subrange(subrange<X, Y, B> r, difference_type_t<I> n)
			requires detail::True<Sized> // strange GCC bug, not yet reported
			: subrange{r.begin(), r.end(), n} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr subrange(pair<X, Y> r)
			: subrange{r.first, r.second} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr subrange(pair<X, Y> r, difference_type_t<I> n)
			requires detail::True<Sized> // strange GCC bug, not yet reported
			: subrange{r.first, r.second, n} {}

			template <Range R>
			requires ConvertibleTo<iterator_t<R>, I> && ConvertibleTo<sentinel_t<R>, S>
			constexpr subrange(R& r)
			: subrange{__stl2::begin(r), __stl2::end(r)} {}

			template <Constructible<const I&> X, Constructible<const S&> Y>
			requires ConvertibleTo<const I&, X> && ConvertibleTo<const S&, Y>
			constexpr operator pair<X, Y>() const {
				return pair<X, Y>{this->begin(), this->end()};
			}

			constexpr bool empty() const {
				return this->begin() == this->end();
			}
			constexpr explicit operator bool() const {
				return !empty();
			}
			constexpr bool operator!() const {
				return empty();
			}
			constexpr auto size() const requires Sized {
				return this->end() - this->begin();
			}
		};

		template <Iterator I, Sentinel<I> S>
		requires !SizedSentinel<S, I>
		struct subrange<I, S, true>
		: view_interface<subrange<I, S, true>>
		{
		private:
			subrange<I, S> rng_ {};
			difference_type_t<I> size_ = 0;
		public:
			using iterator = I;
			using sentinel = S;

			subrange() = default;

			constexpr subrange(I i, S s, difference_type_t<I> n)
			: rng_{i, s}, size_{n} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr subrange(subrange<X, Y, true> r)
			: subrange{r.begin(), r.end(), r.size()} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y, bool B>
			constexpr subrange(subrange<X, Y, B> r, difference_type_t<I> n)
			: subrange{r.begin(), r.end(), n} {
				if constexpr (B) {
					STL2_EXPECT(n == r.size());
				}
			}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			constexpr subrange(pair<X, Y> r, difference_type_t<I> n)
			: subrange{r.first, r.second, n} {}

			template <SizedRange R>
			requires Constructible<subrange<I, S>, R&>
			constexpr subrange(R& r)
			: subrange{__stl2::begin(r), __stl2::end(r), __stl2::size(r)} {}

			template <Constructible<const I&> X, Constructible<const S&> Y>
			requires ConvertibleTo<const I&, X> && ConvertibleTo<const S&, Y>
			constexpr operator pair<X, Y>() const {
				return pair<X, Y>{rng_.begin(), rng_.end()};
			}

			template <Constructible<const I&> X, Constructible<const S&> Y, bool B>
			requires ConvertibleTo<const I&, X> && ConvertibleTo<const S&, Y>
			constexpr operator subrange<X, Y, B>() const {
				if constexpr (B) {
					return {rng_, size_};
				} else {
					return rng_;
				}
			}

			constexpr operator subrange<I, S> const &() const & noexcept {
				return rng_;
			}

			constexpr I begin() const {
				return rng_.begin();
			}
			constexpr S end() const {
				return rng_.end();
			}
			constexpr bool empty() const {
				return begin() == end();
			}
			constexpr explicit operator bool() const {
				return !empty();
			}
			constexpr bool operator!() const {
				return empty();
			}
			constexpr difference_type_t<I> size() const noexcept {
				return size_;
			}
		};

		template <Iterator I, Sentinel<I> S>
		using sized_subrange = subrange<I, S, true>;

		template <Iterator I, Sentinel<I> S>
		subrange(I, S) -> subrange<I, S>;

		template <Iterator I, Sentinel<I> S>
		subrange(I, S, difference_type_t<I>) -> subrange<I, S, true>;

		template <Iterator I, Sentinel<I> S>
		subrange(pair<I, S>) -> subrange<I, S>;

		template <Iterator I, Sentinel<I> S>
		subrange(pair<I, S>, difference_type_t<I>) -> subrange<I, S, true>;

		// template <Iterator I, Sentinel<I> S, bool B>
		// subrange(subrange<I, S, B>) -> subrange<I, S, B>;

		template <Iterator I, Sentinel<I> S, bool B>
		subrange(subrange<I, S, B>, difference_type_t<I>) -> subrange<I, S, true>;

		template <Range R>
		subrange(R&) -> subrange<iterator_t<R>, sentinel_t<R>>;

		template <SizedRange R>
		subrange(R&) -> subrange<iterator_t<R>, sentinel_t<R>, true>;

		template <std::size_t N, class I, class S, bool Sized>
		requires N < 2
		constexpr decltype(auto) get(subrange<I, S, Sized>&& r) noexcept(!Sized) {
			if constexpr (N == 0)
				return std::move(r).begin();
			else
				return std::move(r).end();
		}
		template <std::size_t N, class I, class S, bool Sized>
		requires N < 2
		constexpr decltype(auto) get(subrange<I, S, Sized>& r) noexcept(!Sized) {
			if constexpr (N == 0)
				return r.begin();
			else
				return r.end();
		}
		template <std::size_t N, class I, class S, bool Sized>
		requires N < 2
		constexpr decltype(auto) get(const subrange<I, S, Sized>& r) noexcept(!Sized) {
			if constexpr (N == 0)
				return r.begin();
			else
				return r.end();
		}
	}
} STL2_CLOSE_NAMESPACE

namespace std {
	template <class I, class S, bool Sized>
	struct tuple_size<::__stl2::ext::subrange<I, S, Sized>>
	  : std::integral_constant<size_t, 2> {};
	template <class I, class S, bool Sized>
	struct tuple_element<0, ::__stl2::ext::subrange<I, S, Sized>>
	{ using type = I; };
	template <class I, class S, bool Sized>
	struct tuple_element<1, ::__stl2::ext::subrange<I, S, Sized>>
	{ using type = S; };
}

#endif
