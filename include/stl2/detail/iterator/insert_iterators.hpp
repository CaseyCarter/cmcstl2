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
#ifndef STL2_DETAIL_ITERATOR_INSERT_ITERATORS_HPP
#define STL2_DETAIL_ITERATOR_INSERT_ITERATORS_HPP

#include <cstddef>
#include <stl2/memory.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template <MemberValueType Container>
		class insert_cursor_base {
		public:
			using difference_type = std::ptrdiff_t;

			constexpr insert_cursor_base() noexcept = default;

			STL2_CONSTEXPR_EXT explicit
			insert_cursor_base(Container& x) noexcept
			: container_{__stl2::addressof(x)}
			{}
		protected:
			raw_ptr<Container> container_{};
		};

		template <class Cursor, class Container>
		struct insert_cursor_mixin : protected ebo_box<Cursor> {
			using difference_type =
				typename insert_cursor_base<Container>::difference_type;
			using container_type = Container;
			using iterator_category = output_iterator_tag;
			insert_cursor_mixin() = default;
			using ebo_box<Cursor>::ebo_box;
		};

		template <class T, class C>
		concept bool BackInsertableInto =
			requires (T&& t, C& c) {
				c.push_back((T&&)t);
			};

		template <MemberValueType Container>
		struct back_insert_cursor : insert_cursor_base<Container> {
			using base_t = insert_cursor_base<Container>;
			using mixin = insert_cursor_mixin<back_insert_cursor, Container>;

			constexpr back_insert_cursor() = default;
			using base_t::base_t;

			template <BackInsertableInto<Container> T>
			void write(T&& t) {
				base_t::container_->push_back(__stl2::forward<T>(t));
			}
		};
	}

	///////////////////////////////////////////////////////////////////////////
	// back_insert_iterator [back.insert.iterator]
	//
	template <detail::MemberValueType Container>
	using back_insert_iterator =
		basic_iterator<detail::back_insert_cursor<Container>>;

	template <detail::MemberValueType Container>
	STL2_CONSTEXPR_EXT auto back_inserter(Container& c) noexcept {
		return back_insert_iterator<Container>{c};
	}

	namespace detail {
		template <class T, class C>
		concept bool FrontInsertableInto =
			requires (T&& t, C& c) {
				c.push_front((T&&)t);
			};

		template <MemberValueType Container>
		struct front_insert_cursor : insert_cursor_base<Container> {
			using base_t = insert_cursor_base<Container>;
			using mixin = insert_cursor_mixin<front_insert_cursor, Container>;

			constexpr front_insert_cursor() = default;
			using base_t::base_t;

			template <FrontInsertableInto<Container> T>
			void write(T&& t) {
				base_t::container_->push_front(__stl2::forward<T>(t));
			}
		};
	}

	///////////////////////////////////////////////////////////////////////////
	// front_insert_iterator [front.insert.iterator]
	//
	template <detail::MemberValueType Container>
	using front_insert_iterator =
		basic_iterator<detail::front_insert_cursor<Container>>;

	template <detail::MemberValueType Container>
	STL2_CONSTEXPR_EXT auto front_inserter(Container& x) noexcept {
		return front_insert_iterator<Container>{x};
	}

	namespace detail {
		template <class T, class C>
		concept bool InsertableInto =
			requires (T&& t, C& c, typename C::iterator i) {
				{  c.insert(i, (T&&)t) } -> typename C::iterator;
			};

		template <MemberValueType Container>
			requires requires { typename Container::iterator; }
		class insert_cursor : public insert_cursor_base<Container> {
			using base_t = insert_cursor_base<Container>;
			using I = typename Container::iterator;

			using mixin = insert_cursor_mixin<insert_cursor, Container>;

			constexpr insert_cursor() = default;
			STL2_CONSTEXPR_EXT explicit insert_cursor(Container& x, I i)
				noexcept(is_nothrow_move_constructible<I>::value) :
				base_t{x}, iter_{__stl2::move(i)} {}

			template <InsertableInto<Container> T>
			void write(T&& t) {
				iter_ = base_t::container_->insert(iter_, __stl2::forward<T>(t));
				++iter_;
			}

		private:
			I iter_{};
		};
	}

	///////////////////////////////////////////////////////////////////////////
	// insert_iterator [insert.iterator]
	//
	template <detail::MemberValueType Container,
		class I = typename Container::iterator>
	using insert_iterator =
		basic_iterator<detail::insert_cursor<Container>>;

	template <detail::MemberValueType Container>
	STL2_CONSTEXPR_EXT auto inserter(Container& x, iterator_t<Container> i)
	STL2_NOEXCEPT_RETURN(
		insert_iterator<Container>{x, __stl2::move(i)}
	)
} STL2_CLOSE_NAMESPACE

#endif
