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
#ifndef STL2_VIEW_SUBRANGE_HPP
#define STL2_VIEW_SUBRANGE_HPP

#include <tuple>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/primitives.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template<class From, class To>
	STL2_CONCEPT _ConvertibleToNotSlicing =
		ConvertibleTo<From, To> &&
		// A conversion is a slicing conversion if the source and the destination
		// are both pointers, and if the pointed-to types differ after removing
		// cv qualifiers.
		!(_Is<std::decay_t<From>, std::is_pointer> &&
		  _Is<std::decay_t<To>, std::is_pointer> &&
		  _NotSameAs<std::remove_pointer_t<std::decay_t<From>>,
		             std::remove_pointer_t<std::decay_t<To>>>);

	template<class T>
	STL2_CONCEPT _PairLikeGCCBugs = requires(T t) {
		{ std::get<0>(t) } -> const std::tuple_element_t<0, T>&;
		{ std::get<1>(t) } -> const std::tuple_element_t<1, T>&;
	};

	template<class T>
	STL2_CONCEPT _PairLike =
		!std::is_reference_v<T> && requires {
			typename std::tuple_size<T>::type;
			requires DerivedFrom<std::tuple_size<T>, std::integral_constant<std::size_t, 2>>;
			typename std::tuple_element_t<0, remove_const_t<T>>;
			typename std::tuple_element_t<1, remove_const_t<T>>;
			requires _PairLikeGCCBugs<T>; // Separate named concept to avoid
			                              // premature substitution.
		};

	template<class T, class U, class V>
	STL2_CONCEPT _PairLikeConvertibleFromGCCBugs =
		!std::is_reference_v<std::tuple_element_t<0, T>> &&
		!std::is_reference_v<std::tuple_element_t<1, T>> &&
		_ConvertibleToNotSlicing<U, std::tuple_element_t<0, T>> &&
		ConvertibleTo<V, std::tuple_element_t<1, T>>;

	template<class T, class U, class V>
	STL2_CONCEPT _PairLikeConvertibleFrom =
		!Range<T> && _PairLike<T> && Constructible<T, U, V> &&
		_PairLikeConvertibleFromGCCBugs<T, U, V>; // Separate named concept to avoid
		                                          // premature substitution.

	enum class subrange_kind : bool { unsized, sized };

	template<Iterator I, Sentinel<I> S = I,
		subrange_kind K = static_cast<subrange_kind>(SizedSentinel<S, I>)>
	requires K == subrange_kind::sized || !SizedSentinel<S, I>
	class subrange;

	namespace __subrange_detail {
		struct __adl_hook {};

		// Not to spec: these should be hidden friends
		template<class I, class S, subrange_kind K>
		constexpr I begin(subrange<I, S, K> r)
		noexcept(std::is_nothrow_copy_constructible_v<I>) {
			return r.begin();
		}
		template<class I, class S, subrange_kind K>
		constexpr S end(subrange<I, S, K> r)
		noexcept(std::is_nothrow_copy_constructible_v<S>) {
			return r.end();
		}
	}

	template<Iterator I, Sentinel<I> S, subrange_kind K>
	requires K == subrange_kind::sized || !SizedSentinel<S, I>
	class subrange
	: private __subrange_detail::__adl_hook
	, public view_interface<subrange<I, S, K>>
	{
		static constexpr bool StoreSize =
			K == subrange_kind::sized && !SizedSentinel<S, I>;

		meta::if_c<StoreSize,
			std::tuple<I, S, iter_difference_t<I>>,
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
		constexpr iter_difference_t<I>& size_() noexcept requires StoreSize {
			return std::get<2>(data_);
		}
		constexpr const iter_difference_t<I>& size_() const noexcept requires StoreSize {
			return std::get<2>(data_);
		}
	public:
		using iterator = I;
		using sentinel = S;

		subrange() = default;

		template <_ConvertibleToNotSlicing<I> I2>
		constexpr subrange(I2&& i, S s)
			requires (!StoreSize)
		: data_{std::forward<I2>(i), std::move(s)} {}

		template <_ConvertibleToNotSlicing<I> I2>
		constexpr subrange(I2&& i, S s, iter_difference_t<I> n)
			requires (StoreSize)
		: data_{std::forward<I2>(i), std::move(s), n} {
			if constexpr (RandomAccessIterator<I>) {
				STL2_EXPECT(first_() + n == last_());
			}
		}
		template <_ConvertibleToNotSlicing<I> I2>
		constexpr subrange(I2&& i, S s, iter_difference_t<I> n)
		requires SizedSentinel<S, I>
		: data_{std::forward<I2>(i), std::move(s)} {
			STL2_EXPECT(last_() - first_() == n);
		}

		template<_NotSameAs<subrange> R>
		requires _ForwardingRange<R> &&
			_ConvertibleToNotSlicing<iterator_t<R>, I> &&
			ConvertibleTo<sentinel_t<R>, S>
		constexpr subrange(R&& r) requires (!StoreSize)
		: subrange{__stl2::begin(r), __stl2::end(r)} {}

		template<_NotSameAs<subrange> R>
		requires _ForwardingRange<R> &&
			_ConvertibleToNotSlicing<iterator_t<R>, I> &&
			ConvertibleTo<sentinel_t<R>, S>
		constexpr subrange(R&& r) requires StoreSize && SizedRange<R>
		: subrange{__stl2::begin(r), __stl2::end(r), distance(r)} {}

		template<_ForwardingRange R>
		requires
			_ConvertibleToNotSlicing<iterator_t<R>, I> &&
			ConvertibleTo<sentinel_t<R>, S>
		constexpr subrange(R&& r, iter_difference_t<I> n)
			requires (K == subrange_kind::sized)
		: subrange{__stl2::begin(r), __stl2::end(r), n} {
			if constexpr (SizedRange<R>) {
				STL2_EXPECT(n == distance(r));
			}
		}

		template<_NotSameAs<subrange> PairLike>
		requires _PairLikeConvertibleFrom<PairLike, const I&, const S&>
		constexpr operator PairLike() const {
			return PairLike(first_(), last_());
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

		constexpr iter_difference_t<I> size() const requires K == subrange_kind::sized {
			if constexpr (StoreSize) {
				return size_();
			} else {
				return last_() - first_();
			}
		}

		[[nodiscard]] constexpr subrange next(iter_difference_t<I> n = 1) const {
			auto tmp = *this;
			tmp.advance(n);
			return tmp;
		}
		[[nodiscard]] constexpr subrange prev(iter_difference_t<I> n = 1) const
		requires BidirectionalIterator<I> {
			auto tmp = *this;
			tmp.advance(-n);
			return tmp;
		}
		constexpr subrange& advance(iter_difference_t<I> n) {
			auto remainder = __stl2::advance(first_(), n, last_());
			(void)remainder;
			if constexpr (StoreSize) {
				size_() -= n - remainder;
			}
			return *this;
		}
	};

	template<class I, class S>
	subrange(I, S) -> subrange<I, S>;

	template<Iterator I, class S>
	subrange(I, S, iter_difference_t<I>) -> subrange<I, S, subrange_kind::sized>;

	template<_ForwardingRange R>
	subrange(R&&) -> subrange<iterator_t<R>, sentinel_t<R>,
		(SizedRange<R> || SizedSentinel<sentinel_t<R>, iterator_t<R>>)
			? subrange_kind::sized : subrange_kind::unsized>;

	template<_ForwardingRange R>
	subrange(R&&, iter_difference_t<iterator_t<R>>) ->
		subrange<iterator_t<R>, sentinel_t<R>, subrange_kind::sized>;

	namespace ext {
		template<Iterator I, Sentinel<I> S = I>
		using sized_subrange = subrange<I, S, subrange_kind::sized>;
	}

	template<std::size_t N, class I, class S, subrange_kind K>
	requires N < 2
	constexpr auto get(const subrange<I, S, K>& r) {
		if constexpr (N == 0) {
			return r.begin();
		} else {
			return r.end();
		}
	}

	template<Range R>
	using safe_subrange_t =	__maybe_dangling<R, subrange<iterator_t<R>>>;
} STL2_CLOSE_NAMESPACE

namespace std {
	template<class I, class S, ::__stl2::subrange_kind K>
	struct tuple_size<::__stl2::subrange<I, S, K>>
	  : std::integral_constant<size_t, 2> {};
	template<class I, class S, ::__stl2::subrange_kind K>
	struct tuple_element<0, ::__stl2::subrange<I, S, K>>
	{ using type = I; };
	template<class I, class S, ::__stl2::subrange_kind K>
	struct tuple_element<1, ::__stl2::subrange<I, S, K>>
	{ using type = S; };
}

#endif
