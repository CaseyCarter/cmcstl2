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
#ifndef STL2_DETAIL_ITERATOR_REVERSE_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_REVERSE_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

///////////////////////////////////////////////////////////////////////////
// reverse_iterator [reverse.iterator]
//
STL2_OPEN_NAMESPACE {
	namespace __reverse_iterator {
		template<BidirectionalIterator> class cursor;

		struct access {
			template<_SpecializationOf<cursor> C>
			static constexpr decltype(auto) current(C&& c) noexcept {
				return (std::forward<C>(c).current_);
			}
		};

		template<BidirectionalIterator I>
		class cursor {
			friend access;
			I current_{};

		public:
			using difference_type = iter_difference_t<I>;
			using value_type = iter_value_t<I>;

			class mixin : protected basic_mixin<cursor> {
				using base_t = basic_mixin<cursor>;
			public:
				using iterator_type = I;
				using difference_type = cursor::difference_type;
				using value_type = cursor::value_type;
				using iterator_category = iterator_category_t<I>;
				using reference = iter_reference_t<I>;
				using pointer = I;

				constexpr mixin() = default;
				constexpr explicit mixin(I x)
				noexcept(std::is_nothrow_move_constructible<I>::value)
				: base_t{cursor{std::move(x)}}
				{}
				using base_t::base_t;

				constexpr I base() const
				noexcept(is_nothrow_copy_constructible<I>::value)
				{ return base_t::get().current_; }
			};

			constexpr cursor() = default;
			constexpr explicit cursor(I x)
			noexcept(is_nothrow_move_constructible<I>::value)
			: current_{std::move(x)}
			{}
			template<ConvertibleTo<I> U>
			constexpr cursor(const cursor<U>& u)
			noexcept(is_nothrow_constructible<I, const U&>::value)
			: current_{access::current(u)}
			{}

			constexpr iter_reference_t<I> read() const
			STL2_NOEXCEPT_RETURN(
				*__stl2::prev(current_)
			)

			constexpr I arrow() const
			STL2_NOEXCEPT_RETURN(
				__stl2::prev(current_)
			)

			constexpr void next()
			STL2_NOEXCEPT_RETURN(
				(void)--current_
			)

			constexpr void prev()
			STL2_NOEXCEPT_RETURN(
				(void)++current_
			)

			constexpr void advance(iter_difference_t<I> n)
			noexcept(noexcept(current_ -= n))
			requires RandomAccessIterator<I>
			{
				current_ -= n;
			}

			template<EqualityComparableWith<I> J>
			constexpr bool equal(const cursor<J>& that) const
			STL2_NOEXCEPT_RETURN(
				current_ == access::current(that)
			)

			template<SizedSentinel<I> J>
			constexpr iter_difference_t<I> distance_to(const cursor<J>& that) const
			STL2_NOEXCEPT_RETURN(
				-(access::current(that) - current_)
			)

			constexpr decltype(auto) indirect_move() const
			STL2_NOEXCEPT_RETURN(
				iter_move(__stl2::prev(current_))
			)

			template<IndirectlySwappable<I> J>
			constexpr void indirect_swap(const cursor<J>& that) const
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_swap(
					__stl2::prev(current_), __stl2::prev(access::current(that)))
			)
		};
	}

	template<class I>
	using reverse_iterator = basic_iterator<__reverse_iterator::cursor<I>>;

	template<class I1, StrictTotallyOrderedWith<I1> I2>
	constexpr bool operator<(
		const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
	STL2_NOEXCEPT_RETURN(
		__reverse_iterator::access::current(__stl2::get_cursor(x)) >
			__reverse_iterator::access::current(__stl2::get_cursor(y))
	)

	template<class I1, StrictTotallyOrderedWith<I1> I2>
	constexpr bool operator>(
		const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
	STL2_NOEXCEPT_RETURN(
		y < x
	)

	template<class I1, StrictTotallyOrderedWith<I1> I2>
	constexpr bool operator<=(
		const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
	STL2_NOEXCEPT_RETURN(
		!(y < x)
	)

	template<class I1, StrictTotallyOrderedWith<I1> I2>
	constexpr bool operator>=(
		const reverse_iterator<I1>& x, const reverse_iterator<I2>& y)
	STL2_NOEXCEPT_RETURN(
		!(x < y)
	)

	template<class I>
	requires BidirectionalIterator<__f<I>>
	constexpr auto make_reverse_iterator(I&& i)
	STL2_NOEXCEPT_RETURN(
		reverse_iterator<__f<I>>{std::forward<I>(i)}
	)
} STL2_CLOSE_NAMESPACE

#endif
