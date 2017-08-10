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

#include <stl2/detail/ebo_box.hpp>
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
				return (std::forward<C>(c).current_);
			}
			template <_SpecializationOf<move_sentinel> MS>
			static constexpr decltype(auto) sentinel(MS&& ms) noexcept {
				return std::forward<MS>(ms).get();
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

			class mixin : protected basic_mixin<cursor> {
				using base_t = basic_mixin<cursor>;
			public:
				using iterator_type = I;
				using difference_type = cursor::difference_type;
				using value_type = cursor::value_type;
				using iterator_category = input_iterator_tag;
				using reference = rvalue_reference_t<I>;

				constexpr mixin() = default;
				constexpr explicit mixin(I&& i)
				noexcept(std::is_nothrow_move_constructible<I>::value)
				: base_t{cursor{std::move(i)}}
				{}
				constexpr explicit mixin(const I& i)
				noexcept(std::is_nothrow_copy_constructible<I>::value)
				: base_t{cursor{i}}
				{}
				using base_t::base_t;

				constexpr I base() const
				noexcept(is_nothrow_copy_constructible<I>::value)
				{
					return base_t::get().current_;
				}
			};

			constexpr cursor() = default;
			constexpr explicit cursor(I&& i)
			noexcept(is_nothrow_move_constructible<I>::value)
			: current_{std::move(i)}
			{}
			constexpr explicit cursor(const I& i)
			noexcept(is_nothrow_copy_constructible<I>::value)
			: current_{i}
			{}
			template <ConvertibleTo<I> U>
			constexpr cursor(const cursor<U>& u)
			noexcept(is_nothrow_constructible<I, const U&>::value)
			: current_{access::current(u)}
			{}

			constexpr rvalue_reference_t<I> read() const
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_move(current_)
			)

			constexpr void next()
			STL2_NOEXCEPT_RETURN(
				static_cast<void>(++current_)
			)

			// Not to spec
			// Experimental support for move_iterator post-increment
			// BUGBUG doesn't correctly handle when decltype(current_++)
			// is a reference.
			using __postinc_t = std::decay_t<decltype(current_++)>;
			Readable{R}
			struct __proxy {
				using value_type = __stl2::value_type_t<R>;
				R __tmp;
				constexpr decltype(auto) operator*()
				STL2_NOEXCEPT_RETURN(
					__stl2::iter_move(__tmp)
				)
				friend constexpr decltype(auto) iter_move(const __proxy& that)
				STL2_NOEXCEPT_RETURN(
					__stl2::iter_move(that.__tmp)
				)
			};

#if STL2_WORKAROUND_GCC_69096
			template <class = void>
#endif // STL2_WORKAROUND_GCC_69096
			constexpr auto post_increment()
			noexcept(noexcept(__proxy<__postinc_t>{current_++}))
			requires !ForwardIterator<I> && Readable<__postinc_t> {
				return __proxy<__postinc_t>{current_++};
			}

			constexpr void prev()
			noexcept(noexcept(--current_))
			requires BidirectionalIterator<I>
			{
				--current_;
			}

			constexpr void advance(difference_type_t<I> n)
			noexcept(noexcept(current_ += n))
			requires RandomAccessIterator<I>
			{
				current_ += n;
			}

			constexpr bool equal(
				const cursor<EqualityComparableWith<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				current_ == access::current(that)
			)

			constexpr bool equal(
				const move_sentinel<Sentinel<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				current_ == access::sentinel(that)
			)

			constexpr difference_type_t<I>
			distance_to(const cursor<SizedSentinel<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				access::current(that) - current_
			)

			constexpr difference_type_t<I>
			distance_to(const move_sentinel<SizedSentinel<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				access::sentinel(that) - current_
			)

			// Extension
			constexpr decltype(auto) indirect_move() const
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_move(current_)
			)

			// Extension
			constexpr void indirect_swap(
				const cursor<IndirectlySwappable<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_swap(current_, access::current(that))
			)
		};
	}

	// Not to spec:
	// * uses iter_move in operator* (See https://github.com/ericniebler/stl2/issues/244)
	// * hooks iter_move and iter_swap (See https://github.com/ericniebler/stl2/issues/245)
	// * constexpr per P0579
	InputIterator{I}
	using move_iterator = basic_iterator<__move_iterator::cursor<I>>;

	template <class I>
	struct iterator_category<move_iterator<I>> {
		using type = input_iterator_tag;
	};

	// Not to spec: constexpr per P0579
	StrictTotallyOrderedWith{I1, I2}
	constexpr bool
	operator<(const move_iterator<I1>& a, const move_iterator<I2>& b)
	STL2_NOEXCEPT_RETURN(
		__move_iterator::access::current(__stl2::get_cursor(a)) <
			__move_iterator::access::current(__stl2::get_cursor(b))
	)

	// Not to spec: constexpr per P0579
	StrictTotallyOrderedWith{I1, I2}
	constexpr bool
	operator>(const move_iterator<I1>& a, const move_iterator<I2>& b)
	STL2_NOEXCEPT_RETURN(
		b < a
	)

	// Not to spec: constexpr per P0579
	StrictTotallyOrderedWith{I1, I2}
	constexpr bool
	operator<=(const move_iterator<I1>& a, const move_iterator<I2>& b)
	STL2_NOEXCEPT_RETURN(
		!(b < a)
	)

	// Not to spec: constexpr per P0579
	StrictTotallyOrderedWith{I1, I2}
	constexpr bool
	operator>=(const move_iterator<I1>& a, const move_iterator<I2>& b)
	STL2_NOEXCEPT_RETURN(
		!(a < b)
	)

	// Not to spec: constexpr per P0579
	template <class I>
	requires
		InputIterator<__f<I>>
	constexpr auto make_move_iterator(I&& i)
	STL2_NOEXCEPT_RETURN(
		move_iterator<__f<I>>{std::forward<I>(i)}
	)

	// Not to spec: constexpr per P0579
	Semiregular{S}
	class move_sentinel : detail::ebo_box<S, move_sentinel<S>> {
		friend __move_iterator::access;
		using box_t = detail::ebo_box<S, move_sentinel<S>>;
	public:
		constexpr move_sentinel()
		noexcept(is_nothrow_default_constructible<S>::value)
		: box_t{}
		{}
		explicit constexpr move_sentinel(S s)
		noexcept(is_nothrow_move_constructible<S>::value)
		: box_t(std::move(s))
		{}
		template <ConvertibleTo<S> T>
		constexpr move_sentinel(const move_sentinel<T>& s)
		noexcept(is_nothrow_constructible<S, const T&>::value)
		: box_t{__move_iterator::access::sentinel(s)}
		{}

		template <ConvertibleTo<S> T>
		constexpr move_sentinel& operator=(const move_sentinel<T>& s) &
		noexcept(is_nothrow_assignable<S&, const T&>::value)
		{
			box_t::get() = __move_iterator::access::sentinel(s);
			return *this;
		}

		constexpr S base() const
		noexcept(is_nothrow_copy_constructible<S>::value)
		{ return box_t::get(); }
	};

	// Not to spec: constexpr per P0579
	template <class S>
	requires
		Semiregular<__f<S>>
	constexpr auto make_move_sentinel(S&& s)
	STL2_NOEXCEPT_RETURN(
		move_sentinel<__f<S>>(std::forward<S>(s))
	)
} STL2_CLOSE_NAMESPACE

#endif
