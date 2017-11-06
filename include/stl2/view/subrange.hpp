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
	template <class T>
	concept bool _PairLike =
		meta::Integral<std::tuple_size<T>> &&
		std::tuple_size<T>::value == 2 &&
		meta::Trait<std::tuple_element<0, T>> &&
		meta::Trait<std::tuple_element<1, T>> &&
		requires(T t) {
			{ std::get<0>(t) } -> const meta::_t<std::tuple_element<0, T>>&;
			{ std::get<1>(t) } -> const meta::_t<std::tuple_element<1, T>>&;
		};

	template <class T, class U, class V>
	concept bool _PairLikeConvertibleTo =
		!Range<T> && _PairLike<__uncvref<T>> &&
		requires(T&& t) {
			{ std::get<0>(static_cast<T&&>(t)) } -> ConvertibleTo<U>;
			{ std::get<1>(static_cast<T&&>(t)) } -> ConvertibleTo<V>;
		};

	template <class T, class U, class V>
	concept bool _PairLikeConvertibleFrom =
		!Range<T> && Same<T, __uncvref<T>> && _PairLike<T> &&
		Constructible<T, U, V>;

	template <class T>
	concept bool _IteratorSentinelPair =
		!Range<T> && Same<T, __uncvref<T>> && _PairLike<T> &&
		Sentinel<
			meta::_t<std::tuple_element<1, T>>,
			meta::_t<std::tuple_element<0, T>>>;

	namespace ext {
		enum subrange_kind : bool { unsized, sized };

		template <Iterator I, Sentinel<I> S = I,
			subrange_kind K = static_cast<subrange_kind>(models::SizedSentinel<S, I>)>
		requires K == subrange_kind::sized || !SizedSentinel<S, I>
		class subrange
		: public view_interface<subrange<I, S, K>>
		{
			template <class, class, subrange_kind>
			friend class subrange;

			static constexpr bool StoreSize =
				K == subrange_kind::sized && !models::SizedSentinel<S, I>;

			meta::if_c<StoreSize,
				std::tuple<I, S, difference_type_t<I>>,
				std::tuple<I, S>> data_;

			constexpr I& first_() noexcept {
				return std::get<0>(data_);
			}
			constexpr const I& first_() const noexcept {
				return std::get<0>(data_);
			}
			constexpr S& last_() noexcept {
				return std::get<1>(data_);
			}
			constexpr const S& last_() const noexcept {
				return std::get<1>(data_);
			}
			constexpr difference_type_t<I>& size_() noexcept requires StoreSize {
				return std::get<2>(data_);
			}
			constexpr const difference_type_t<I>& size_() const noexcept requires StoreSize {
				return std::get<2>(data_);
			}
		public:
			using iterator = I;
			using sentinel = S;

			subrange() = default;

			constexpr subrange(I i, S s) requires !StoreSize
			: data_{std::move(i), std::move(s)} {}

			constexpr subrange(I i, S s, difference_type_t<I> n)
			requires StoreSize
			: data_{std::move(i), std::move(s), n} {
				if constexpr (models::RandomAccessIterator<I>) {
					STL2_ASSERT(first_() + n == last_());
				}
			}
			constexpr subrange(I i, S s, difference_type_t<I> n)
			requires SizedSentinel<S, I>
			: data_{std::move(i), std::move(s)} {
				STL2_ASSERT(last_() - first_() == n);
			}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y, subrange_kind Z>
			requires !StoreSize
			constexpr subrange(subrange<X, Y, Z> r)
			: subrange{r.first_(), r.last_()} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
			requires StoreSize
			constexpr subrange(subrange<X, Y, subrange_kind::sized> r)
			: subrange{r.first_(), r.last_(), r.size()} {}

			template <ConvertibleTo<I> X, ConvertibleTo<S> Y, subrange_kind Z>
			requires K == subrange_kind::sized
			constexpr subrange(subrange<X, Y, Z> r, difference_type_t<I> n)
			: subrange{r.first_(), r.last_(), n} {
				if constexpr (Z == subrange_kind::sized) {
					STL2_ASSERT(n == r.size());
				}
			}

			template <_PairLikeConvertibleTo<I, S> PairLike>
			requires !StoreSize
			constexpr subrange(PairLike&& r)
			: subrange{std::get<0>(static_cast<PairLike&&>(r)),
				std::get<1>(static_cast<PairLike&&>(r))}
			{}

			template <_PairLikeConvertibleTo<I, S> PairLike>
			requires K == subrange_kind::sized
			constexpr subrange(PairLike&& r, difference_type_t<I> n)
			: subrange{std::get<0>(static_cast<PairLike&&>(r)),
				std::get<1>(static_cast<PairLike&&>(r)), n}
			{}

			template <Range R>
			requires !StoreSize &&
				ConvertibleTo<iterator_t<R>, I> && ConvertibleTo<sentinel_t<R>, S>
			constexpr subrange(R& r)
			: subrange{__stl2::begin(r), __stl2::end(r)} {}

			template <SizedRange R>
			requires StoreSize &&
				ConvertibleTo<iterator_t<R>, I> && ConvertibleTo<sentinel_t<R>, S>
			constexpr subrange(R& r)
			: subrange{__stl2::begin(r), __stl2::end(r), __stl2::distance(r)} {}

			template <_PairLikeConvertibleFrom<const I&, const S&> PairLike>
			constexpr operator PairLike() const {
				return PairLike{first_(), last_()};
			}

			constexpr I begin() const noexcept(std::is_nothrow_copy_constructible_v<I>) {
				return first_();
			}
			constexpr S end() const noexcept(std::is_nothrow_copy_constructible_v<S>) {
				return last_();
			}
			constexpr bool empty() const {
				return first_() == last_();
			}

			constexpr difference_type_t<I> size() const requires K == subrange_kind::sized {
				if constexpr (StoreSize) {
					return size_();
				} else {
					return last_() - first_();
				}
			}

			[[nodiscard]] constexpr subrange next(difference_type_t<I> n = 1) const
			requires ForwardIterator<I> {
				auto tmp = *this;
				tmp.advance(n);
				return tmp;
			}
			[[nodiscard]] constexpr subrange prev(difference_type_t<I> n = 1) const
			requires BidirectionalIterator<I> {
				auto tmp = *this;
				tmp.advance(-n);
				return tmp;
			}
			constexpr subrange& advance(difference_type_t<I> n) {
				auto remainder = __stl2::advance(first_(), n, last_());
				if constexpr (StoreSize) {
					size_() -= n - remainder;
				}
				return *this;
			}
		};

		template <Iterator I, Sentinel<I> S>
		using sized_subrange = subrange<I, S, subrange_kind::sized>;

		template <Range R>
		using safe_subrange_t =	meta::if_<
			std::is_lvalue_reference<R>,
			ext::subrange<iterator_t<R>>,
			tagged_pair<tag::begin(safe_iterator_t<R>), tag::end(safe_iterator_t<R>)>>;

		template <Iterator I, Sentinel<I> S>
		subrange(I, S, difference_type_t<I>) -> subrange<I, S, subrange_kind::sized>;

		template <_IteratorSentinelPair P>
		subrange(P) ->
			subrange<std::tuple_element_t<0, P>, std::tuple_element_t<1, P>>;

		template <_IteratorSentinelPair P>
		subrange(P, difference_type_t<std::tuple_element_t<0, P>>) ->
			subrange<std::tuple_element_t<0, P>, std::tuple_element_t<1, P>, subrange_kind::sized>;

		template <Iterator I, Sentinel<I> S, subrange_kind K>
		subrange(subrange<I, S, K>, difference_type_t<I>) -> subrange<I, S, subrange_kind::sized>;

		template <Range R>
		subrange(R&) -> subrange<iterator_t<R>, sentinel_t<R>>;

		template <SizedRange R>
		subrange(R&) -> subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;

		template <std::size_t N, class I, class S, subrange_kind K>
		requires N < 2
		constexpr auto get(const subrange<I, S, K>& r) {
			if constexpr (N == 0) {
				return r.begin();
			} else {
				return r.end();
			}
		}
	}
} STL2_CLOSE_NAMESPACE

namespace std {
	template <class I, class S, ::__stl2::ext::subrange_kind K>
	struct tuple_size<::__stl2::ext::subrange<I, S, K>>
	  : std::integral_constant<size_t, 2> {};
	template <class I, class S, ::__stl2::ext::subrange_kind K>
	struct tuple_element<0, ::__stl2::ext::subrange<I, S, K>>
	{ using type = I; };
	template <class I, class S, ::__stl2::ext::subrange_kind K>
	struct tuple_element<1, ::__stl2::ext::subrange<I, S, K>>
	{ using type = S; };
}

#endif
