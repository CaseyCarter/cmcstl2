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
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/utility.hpp>

#include <tuple>

STL2_OPEN_NAMESPACE {
	// __uncvref here is needed because of a libstdc++ bug.
	template <class T>
	concept bool _PairLike =
		meta::Integral<std::tuple_size<__uncvref<T>>> &&
		std::tuple_size<__uncvref<T>>::value == 2 &&
		meta::Trait<std::tuple_element<0, __uncvref<T>>> &&
		meta::Trait<std::tuple_element<1, __uncvref<T>>>;

	template <class T, class U, class V>
	concept bool _PairLikeConvertibleTo =
		_PairLike<T> && !Range<T> &&
		ConvertibleTo<meta::_t<std::tuple_element<0, __uncvref<T>>>, U> &&
		ConvertibleTo<meta::_t<std::tuple_element<1, __uncvref<T>>>, V>;

	template <class T, class U, class V>
	concept bool _PairLikeConvertibleFrom =
		_PairLike<T> && !Range<T> &&
		Constructible<T, U, V> &&
		ConvertibleTo<U, meta::_t<std::tuple_element<0, __uncvref<T>>>> &&
		ConvertibleTo<V, meta::_t<std::tuple_element<1, __uncvref<T>>>>;

	template <class T>
	concept bool _IteratorSentinelPair =
		_PairLike<T> && !Range<T> &&
		Sentinel<
			meta::_t<std::tuple_element<1, __uncvref<T>>>,
			meta::_t<std::tuple_element<0, __uncvref<T>>>>;

	namespace ext {
		template <Iterator I, Sentinel<I> S = I, bool Sized = models::SizedSentinel<S, I>>
		struct subrange
		: view_interface<subrange<I, S, Sized>>
		{
		private:
			std::tuple<I, S> rng_ {};
			friend subrange<I, S, true>;
		public:
			using iterator = I;
			using sentinel = S;

			subrange() = default;

			constexpr subrange(I i, S s)
			: rng_{i, s} {}

			constexpr subrange(I i, S s, difference_type_t<I> n) requires Sized
			: subrange{i, s} {
				STL2_EXPECT(n == s - i);
			}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y, bool B>
			constexpr subrange(subrange<X, Y, B> r)
			: subrange{r.begin(), r.end()} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y, bool B>
			constexpr subrange(subrange<X, Y, B> r, difference_type_t<I> n)
			requires __bool<Sized> // strange GCC bug, not yet reported
			: subrange{r.begin(), r.end(), n} {}

			template <_PairLikeConvertibleTo<I, S> PairLike>
			constexpr subrange(PairLike r)
			: subrange{std::get<0>(std::move(r)), std::get<1>(std::move(r))} {}

			template <_PairLikeConvertibleTo<I, S> PairLike>
			constexpr subrange(PairLike r, difference_type_t<I> n)
			requires __bool<Sized> // strange GCC bug, not yet reported
			: subrange{std::get<0>(std::move(r)), std::get<1>(std::move(r)), n} {}

			template <Range R>
			requires ConvertibleTo<iterator_t<R>, I> && ConvertibleTo<sentinel_t<R>, S>
			constexpr subrange(R& r)
			: subrange{__stl2::begin(r), __stl2::end(r)} {}

			template <_PairLikeConvertibleFrom<const I&, const S&> PairLike>
			constexpr operator PairLike() const {
				return PairLike{begin(), end()};
			}

			constexpr I begin() const {
				return std::get<0>(rng_);
			}
			constexpr S end() const {
				return std::get<1>(rng_);
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
			constexpr auto size() const requires Sized {
				return end() - begin();
			}
			[[nodiscard]] constexpr subrange next(difference_type_t<I> n = 1) const {
				auto tmp = *this;
				tmp.advance(n);
				return tmp;
			}
			[[nodiscard]] constexpr subrange prev(difference_type_t<I> n = 1) const {
				auto tmp = *this;
				tmp.advance(-n);
				return tmp;
			}
			constexpr subrange& advance(difference_type_t<I> n) {
				__stl2::advance(rng_.begin(), n, end());
				return *this;
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

			template <_PairLikeConvertibleTo<I, S> PairLike>
			constexpr subrange(PairLike r, difference_type_t<I> n)
			: subrange{std::get<0>(std::move(r)), std::get<1>(std::move(r)), n} {}

			template <SizedRange R>
			requires Constructible<subrange<I, S>, R&>
			constexpr subrange(R& r)
			: subrange{__stl2::begin(r), __stl2::end(r), __stl2::size(r)} {}

			template <_PairLikeConvertibleFrom<const I&, const S&> PairLike>
			constexpr operator PairLike() const {
				return PairLike{rng_.begin(), rng_.end()};
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
			[[nodiscard]] constexpr subrange next(difference_type_t<I> n = 1) const {
				auto tmp = *this;
				tmp.advance(n);
				return tmp;
			}
			[[nodiscard]] constexpr subrange prev(difference_type_t<I> n = 1) const {
				auto tmp = *this;
				tmp.advance(-n);
				return tmp;
			}
			constexpr subrange& advance(difference_type_t<I> n) {
				auto b = begin();
				auto count = n - __stl2::advance(b, n, end());
				std::get<0>(rng_.rng_) = b;
				size_ -= count;
				return *this;
			}
		};

		template <Iterator I, Sentinel<I> S>
		using sized_subrange = subrange<I, S, true>;

		template <Range R>
		using safe_subrange_t =	meta::if_<
			std::is_lvalue_reference<R>,
			ext::subrange<iterator_t<R>>,
			tagged_pair<tag::begin(safe_iterator_t<R>), tag::end(safe_iterator_t<R>)>>;

		template <Iterator I, Sentinel<I> S>
		subrange(I, S, difference_type_t<I>) -> subrange<I, S, true>;

		template <_IteratorSentinelPair P>
		subrange(P) ->
			subrange<std::tuple_element_t<0, P>, std::tuple_element_t<1, P>>;

		template <_IteratorSentinelPair P>
		subrange(P, difference_type_t<std::tuple_element_t<0, P>>) ->
			subrange<std::tuple_element_t<0, P>, std::tuple_element_t<1, P>, true>;

		template <Iterator I, Sentinel<I> S, bool B>
		subrange(subrange<I, S, B>, difference_type_t<I>) -> subrange<I, S, true>;

		template <Range R>
		subrange(R&) -> subrange<iterator_t<R>, sentinel_t<R>>;

		template <SizedRange R>
		subrange(R&) -> subrange<iterator_t<R>, sentinel_t<R>, true>;

		template <std::size_t N, class I, class S, bool Sized>
		requires N < 2
		constexpr auto get(const subrange<I, S, Sized>& r) {
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
