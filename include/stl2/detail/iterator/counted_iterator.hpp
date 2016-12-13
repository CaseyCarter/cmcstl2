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
#ifndef STL2_DETAIL_ITERATOR_COUNTED_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_COUNTED_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/iterator/operations.hpp>

STL2_OPEN_NAMESPACE {
	namespace __counted_iterator {
		Iterator{I} class cursor;

		struct access {
			template <_SpecializationOf<cursor> CC>
			static constexpr decltype(auto) base(CC&& cc) noexcept {
				return __stl2::forward<CC>(cc).get();
			}
			template <_SpecializationOf<cursor> CC>
			static constexpr decltype(auto) count(CC&& cc) noexcept {
				return (__stl2::forward<CC>(cc).count_);
			}
		};

		Iterator{I}
		struct adaptor_box : protected detail::ebo_box<I> {
			adaptor_box() = default;
			using detail::ebo_box<I>::ebo_box;
		};
		InputIterator{I}
		struct adaptor_box<I> : protected detail::ebo_box<I> {
			using value_type = value_type_t<I>;
			using single_pass = meta::bool_<!models::ForwardIterator<I>>;
			using contiguous = meta::bool_<models::ContiguousIterator<I>>;

			adaptor_box() = default;
			using detail::ebo_box<I>::ebo_box;
		};

		Iterator{I}
		class cursor : public adaptor_box<I> {
			friend access;
			using box_t = adaptor_box<I>;
			using box_t::get;
			difference_type_t<I> count_;

		public:
			using difference_type = difference_type_t<I>;

			class mixin : protected detail::ebo_box<cursor> {
				using box_t = detail::ebo_box<cursor>;
			public:
				using iterator_type = I;
				using difference_type = cursor::difference_type;

				using box_t::box_t;

				STL2_CONSTEXPR_EXT I base() const
				noexcept(is_nothrow_copy_constructible<I>::value)
				{
					return box_t::get().get();
				}
				STL2_CONSTEXPR_EXT difference_type count() const noexcept {
					return box_t::get().count_;
				}
			};

			STL2_CONSTEXPR_EXT cursor()
			noexcept(is_nothrow_default_constructible<I>::value)
			: box_t{}, count_{0}
			{}
			STL2_CONSTEXPR_EXT cursor(I&& i, difference_type n)
			noexcept(is_nothrow_move_constructible<I>::value)
			: box_t(__stl2::move(i)), count_{n}
			{
				STL2_EXPECT(0 <= n);
			}
			STL2_CONSTEXPR_EXT cursor(const I& i, difference_type n)
			noexcept(is_nothrow_copy_constructible<I>::value)
			: box_t(i), count_{n}
			{
				STL2_EXPECT(0 <= n);
			}
			template <ConvertibleTo<I> O>
			STL2_CONSTEXPR_EXT cursor(cursor<O>&& that)
			noexcept(is_nothrow_constructible<I, O&&>::value)
			: box_t(access::base(__stl2::move(that))), count_{access::count(that)}
			{}
			template <ConvertibleTo<I> O>
			STL2_CONSTEXPR_EXT cursor(const cursor<O>& that)
			noexcept(is_nothrow_constructible<I, const O&>::value)
			: box_t(access::base(that)), count_{access::count(that)}
			{}

#if 1 // Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69096
			template <class II = I>
			requires
				InputIterator<II>() && Same<I, II>()
			STL2_CONSTEXPR_EXT decltype(auto) read() const
			noexcept(noexcept(*declval<const II&>()))
			{
				return *get();
			}
#else
			STL2_CONSTEXPR_EXT decltype(auto) read() const
			noexcept(noexcept(*declval<const I&>()))
			requires InputIterator<I>()
			{
				return *get();
			}
#endif

			template <class T>
			requires
				!InputIterator<I>() &&
				Writable<I, T&&>()
			STL2_CONSTEXPR_EXT void write(T&& t)
			noexcept(noexcept(*declval<I&>() = __stl2::forward<T>(t)))
			{
				*get() = __stl2::forward<T>(t);
			}

			STL2_CONSTEXPR_EXT bool equal(default_sentinel) const noexcept {
				return count_ == 0;
			}
			STL2_CONSTEXPR_EXT bool equal(
				const cursor<Common<I> >& that) const noexcept
			{
				return count_ == access::count(that);
			}

			STL2_CONSTEXPR_EXT void next()
			noexcept(noexcept(++declval<I&>()))
			{
				STL2_EXPECT(0 < count_);
				++get();
				--count_;
			}

			STL2_CONSTEXPR_EXT void prev()
			noexcept(noexcept(--declval<I&>()))
			requires BidirectionalIterator<I>()
			{
				--get();
				++count_;
			}

			STL2_CONSTEXPR_EXT void advance(difference_type n)
			noexcept(noexcept(declval<I&>() += n))
			requires RandomAccessIterator<I>()
			{
				STL2_EXPECT(n <= count_);
				get() += n;
				count_ -= n;
			}

			STL2_CONSTEXPR_EXT difference_type distance_to(
				default_sentinel) const noexcept
			{
				return count_;
			}
			STL2_CONSTEXPR_EXT difference_type distance_to(
				const cursor<Common<I> >& that) const noexcept
			{
				return count_ - access::count(that);
			}

			// FIXME: test
			// Extension
			STL2_CONSTEXPR_EXT decltype(auto) indirect_move() const
			noexcept(noexcept(__stl2::iter_move(declval<const I&>())))
			requires InputIterator<I>()
			{
				return __stl2::iter_move(get());
			}

			// FIXME: test
			// Extension
			STL2_CONSTEXPR_EXT void indirect_swap(
				const cursor<IndirectlySwappable<I> >& that) const
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_swap(get(), access::base(that))
			)
		};
	}

	// Not to spec:
	// * hooks iter_move and iter_swap
	//   See https://github.com/ericniebler/stl2/245
	Iterator{I}
	using counted_iterator = basic_iterator<__counted_iterator::cursor<I>>;

	template <class I>
	requires
		models::Iterator<__f<I>>
	STL2_CONSTEXPR_EXT auto make_counted_iterator(I&& i, difference_type_t<__f<I>> n)
	STL2_NOEXCEPT_RETURN(
		counted_iterator<__f<I>>{__stl2::forward<I>(i), n}
	)

	Iterator{I}
	STL2_CONSTEXPR_EXT void advance(counted_iterator<I>& i, difference_type_t<I> n)
	noexcept(is_nothrow_copy_constructible<I>::value &&
					 is_nothrow_move_assignable<I>::value &&
					 noexcept(__stl2::next(declval<const I&>(), n)))
	{
		STL2_EXPECT(n <= i.count());
		__counted_iterator::access::base(__stl2::get_cursor(i)) =
			__stl2::next(__counted_iterator::access::base(__stl2::get_cursor(i)), n);
		__counted_iterator::access::count(__stl2::get_cursor(i)) -= n;
	}

	RandomAccessIterator{I}
	STL2_CONSTEXPR_EXT void advance(counted_iterator<I>& i, difference_type_t<I> n)
	STL2_NOEXCEPT_RETURN(
		(STL2_EXPECT(n <= i.count()), i += n)
	)

	namespace ext {
		Iterator{I}
		constexpr auto uncounted(const I& i)
		noexcept(is_nothrow_copy_constructible<I>::value)
		{
			return i;
		}

		constexpr auto uncounted(const counted_iterator<auto>& i)
		STL2_NOEXCEPT_RETURN(
			i.base()
		)

		Iterator{I}
		constexpr auto recounted(const I&, const I& i, difference_type_t<I> = 0)
		noexcept(is_nothrow_copy_constructible<I>::value)
		{
			return i;
		}

		template <class I>
		constexpr auto recounted(const counted_iterator<I>& o, I i)
		STL2_NOEXCEPT_RETURN(
			counted_iterator<I>{__stl2::move(i), o.count()}
		)

		template <class I>
		constexpr auto recounted(
			const counted_iterator<I>& o, I i, difference_type_t<I> n)
		noexcept(noexcept(counted_iterator<I>{__stl2::move(i), o.count() - n}))
		{
			STL2_EXPENSIVE_ASSERT(!models::ForwardIterator<I> ||
				i == __stl2::next(o.base(), n));
			return counted_iterator<I>{__stl2::move(i), o.count() - n};
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
