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
#ifndef STL2_DETAIL_ITERATOR_MOVE_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_MOVE_ITERATOR_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
	Semiregular{S} class move_sentinel;

	namespace __move_iterator {
		InputIterator{I} class cursor;

		struct access {
			template <_SpecializationOf<cursor> C>
			static constexpr decltype(auto) current(C&& c) noexcept {
				return (__stl2::forward<C>(c).current_);
			}
			template <_SpecializationOf<move_sentinel> MS>
			static constexpr decltype(auto) sentinel(MS&& ms) noexcept {
				return __stl2::forward<MS>(ms).get();
			}
		};

		InputIterator{I}
		class cursor {
			friend access;
			I current_{};
		public:
			using difference_type = difference_type_t<I>;
			using value_type = value_type_t<I>;
			using single_pass = meta::bool_<!models::ForwardIterator<I>>;
			using contiguous = meta::bool_<models::ContiguousIterator<I>>;

			class mixin : protected detail::ebo_box<cursor> {
				using box_t = detail::ebo_box<cursor>;
			public:
				using iterator_type = I;
				using difference_type = cursor::difference_type;
				using value_type = cursor::value_type;
				using iterator_category = input_iterator_tag;
				using reference = rvalue_reference_t<I>;

				using box_t::box_t;

				STL2_CONSTEXPR_EXT I base() const
				noexcept(is_nothrow_copy_constructible<I>::value)
				{
					return box_t::get().current_;
				}
			};

			cursor() = default;
			explicit STL2_CONSTEXPR_EXT cursor(I&& i)
			noexcept(is_nothrow_move_constructible<I>::value)
			: current_{__stl2::move(i)}
			{}
			explicit STL2_CONSTEXPR_EXT cursor(const I& i)
			noexcept(is_nothrow_copy_constructible<I>::value)
			: current_{i}
			{}
			template <ConvertibleTo<I> U>
			STL2_CONSTEXPR_EXT cursor(const cursor<U>& u)
			noexcept(is_nothrow_constructible<I, const U&>::value)
			: current_{access::current(u)}
			{}

			STL2_CONSTEXPR_EXT rvalue_reference_t<I> read() const
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_move(current_)
			)

			STL2_CONSTEXPR_EXT void next()
			STL2_NOEXCEPT_RETURN(
				static_cast<void>(++current_)
			)

			STL2_CONSTEXPR_EXT void prev()
			noexcept(noexcept(--current_))
			requires BidirectionalIterator<I>()
			{
				--current_;
			}

			STL2_CONSTEXPR_EXT void advance(difference_type_t<I> n)
			noexcept(noexcept(current_ += n))
			requires RandomAccessIterator<I>()
			{
				current_ += n;
			}

			STL2_CONSTEXPR_EXT bool equal(
				const cursor<EqualityComparable<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				current_ == access::current(that)
			)

			STL2_CONSTEXPR_EXT bool equal(
				const move_sentinel<Sentinel<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				current_ == access::sentinel(that)
			)

			STL2_CONSTEXPR_EXT difference_type_t<I>
			distance_to(const cursor<SizedSentinel<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				access::current(that) - current_
			)

			STL2_CONSTEXPR_EXT difference_type_t<I>
			distance_to(const move_sentinel<SizedSentinel<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				access::sentinel(that) - current_
			)

			// Extension
			STL2_CONSTEXPR_EXT decltype(auto) indirect_move() const
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_move(current_)
			)

			// Extension
			STL2_CONSTEXPR_EXT void indirect_swap(
				const cursor<IndirectlySwappable<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_swap(current_, access::current(that))
			)
		};
	}

	// Not to spec:
	// * uses iter_move in operator* (See https://github.com/ericniebler/stl2/issues/244)
	// * hooks iter_move and iter_swap (See https://github.com/ericniebler/stl2/issues/245)
	InputIterator{I}
	using move_iterator = basic_iterator<__move_iterator::cursor<I>>;

	template <class I>
	struct iterator_category<move_iterator<I>> {
		using type = input_iterator_tag;
	};

	StrictTotallyOrdered{I1, I2}
	STL2_CONSTEXPR_EXT bool
	operator<(const move_iterator<I1>& a, const move_iterator<I2>& b)
	STL2_NOEXCEPT_RETURN(
		__move_iterator::access::current(__stl2::get_cursor(a)) <
			__move_iterator::access::current(__stl2::get_cursor(b))
	)

	StrictTotallyOrdered{I1, I2}
	STL2_CONSTEXPR_EXT bool
	operator>(const move_iterator<I1>& a, const move_iterator<I2>& b)
	STL2_NOEXCEPT_RETURN(
		b < a
	)

	StrictTotallyOrdered{I1, I2}
	STL2_CONSTEXPR_EXT bool
	operator<=(const move_iterator<I1>& a, const move_iterator<I2>& b)
	STL2_NOEXCEPT_RETURN(
		!(b < a)
	)

	StrictTotallyOrdered{I1, I2}
	STL2_CONSTEXPR_EXT bool
	operator>=(const move_iterator<I1>& a, const move_iterator<I2>& b)
	STL2_NOEXCEPT_RETURN(
		!(a < b)
	)

	template <class I>
	requires
		InputIterator<__f<I>>()
	STL2_CONSTEXPR_EXT auto make_move_iterator(I&& i)
	STL2_NOEXCEPT_RETURN(
		move_iterator<__f<I>>{__stl2::forward<I>(i)}
	)

	Semiregular{S}
	class move_sentinel : detail::ebo_box<S> {
		friend __move_iterator::access;
		using box_t = detail::ebo_box<S>;
	public:
		constexpr move_sentinel()
		noexcept(is_nothrow_default_constructible<S>::value)
		: box_t{}
		{}
		explicit STL2_CONSTEXPR_EXT move_sentinel(S s)
		noexcept(is_nothrow_move_constructible<S>::value)
		: box_t(__stl2::move(s))
		{}
		template <ConvertibleTo<S> T>
		STL2_CONSTEXPR_EXT move_sentinel(const move_sentinel<T>& s)
		noexcept(is_nothrow_constructible<S, const T&>::value)
		: box_t{__move_iterator::access::sentinel(s)}
		{}

		template <ConvertibleTo<S> T>
		STL2_CONSTEXPR_EXT move_sentinel& operator=(const move_sentinel<T>& s) &
		noexcept(is_nothrow_assignable<S&, const T&>::value)
		{ box_t::get() = __move_iterator::access::sentinel(s); }

		STL2_CONSTEXPR_EXT S base() const
		noexcept(is_nothrow_copy_constructible<S>::value)
		{ return box_t::get(); }
	};

	template <class S>
	requires
		models::Semiregular<__f<S>>
	STL2_CONSTEXPR_EXT auto make_move_sentinel(S&& s)
	STL2_NOEXCEPT_RETURN(
		move_sentinel<__f<S>>(__stl2::forward<S>(s))
	)
} STL2_CLOSE_NAMESPACE

#endif
