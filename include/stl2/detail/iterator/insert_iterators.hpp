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
#include <memory>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template<MemberValueType Container>
		class insert_cursor_base {
		public:
			using difference_type = std::ptrdiff_t;

			constexpr insert_cursor_base() noexcept = default;

			constexpr explicit insert_cursor_base(Container& x) noexcept
			: container_{std::addressof(x)}
			{}
		protected:
			raw_ptr<Container> container_ = nullptr;
		};

		template<class Cursor, class Container>
		struct insert_cursor_mixin
		: protected ebo_box<Cursor, insert_cursor_mixin<Cursor, Container>> {
			using difference_type =
				typename insert_cursor_base<Container>::difference_type;
			using container_type = Container;
			using insert_cursor_mixin::ebo_box::ebo_box;
		};

		template<class T, class C>
		META_CONCEPT BackInsertableInto =
			requires(T&& t, C& c) { c.push_back(static_cast<T&&>(t)); };

		template<MemberValueType Container>
		struct back_insert_cursor : insert_cursor_base<Container> {
			using base_t = insert_cursor_base<Container>;
			using mixin = insert_cursor_mixin<back_insert_cursor, Container>;

			constexpr back_insert_cursor() = default;
			using base_t::base_t;

			template<BackInsertableInto<Container> T>
			constexpr void write(T&& t) {
				base_t::container_->push_back(std::forward<T>(t));
			}
		};
	}

	///////////////////////////////////////////////////////////////////////////
	// back_insert_iterator [back.insert.iterator]
	//
	template<detail::MemberValueType Container>
	using back_insert_iterator =
		basic_iterator<detail::back_insert_cursor<Container>>;

	template<detail::MemberValueType Container>
	constexpr auto back_inserter(Container& c) noexcept {
		return back_insert_iterator<Container>{c};
	}

	namespace detail {
		template<class T, class C>
		META_CONCEPT FrontInsertableInto =
			requires(T&& t, C& c) { c.push_front(static_cast<T&&>(t)); };

		template<MemberValueType Container>
		struct front_insert_cursor : insert_cursor_base<Container> {
			using base_t = insert_cursor_base<Container>;
			using mixin = insert_cursor_mixin<front_insert_cursor, Container>;

			constexpr front_insert_cursor() = default;
			using base_t::base_t;

			template<FrontInsertableInto<Container> T>
			constexpr void write(T&& t) {
				base_t::container_->push_front(std::forward<T>(t));
			}
		};
	}

	///////////////////////////////////////////////////////////////////////////
	// front_insert_iterator [front.insert.iterator]
	//
	template<detail::MemberValueType Container>
	using front_insert_iterator =
		basic_iterator<detail::front_insert_cursor<Container>>;

	template<detail::MemberValueType Container>
	constexpr auto front_inserter(Container& x) noexcept {
		return front_insert_iterator<Container>{x};
	}

	namespace detail {
		template<class T, class C>
		META_CONCEPT InsertableInto = requires(T&& t, C& c, iterator_t<C> i) {
#if STL2_BROKEN_COMPOUND_REQUIREMENT
			c.insert(i, static_cast<T&&>(t));
			requires _IsConvertibleImpl<decltype(c.insert(i, static_cast<T&&>(t))), iterator_t<C>>;
#else
			{ c.insert(i, static_cast<T&&>(t)) } -> iterator_t<C>;
#endif // STL2_BROKEN_COMPOUND_REQUIREMENT
		};
	}

	///////////////////////////////////////////////////////////////////////////
	// insert_iterator [insert.iterator]
	//
	template<detail::MemberValueType Container>
	requires requires { typename iterator_t<Container>; }
	class insert_iterator {
	public:
		using container_type = Container;
		using difference_type = std::ptrdiff_t;

		insert_iterator() = default;
		constexpr insert_iterator(Container& x, iterator_t<Container> i)
		: container(std::addressof(x)), iter(std::move(i)) {}
		constexpr insert_iterator& operator=(const iter_value_t<Container>& value)
		requires detail::InsertableInto<const iter_value_t<Container>&, Container>
		{
			iter = container->insert(iter, value);
			++iter;
			return *this;
		}
		constexpr insert_iterator& operator=(iter_value_t<Container>&& value)
		requires detail::InsertableInto<iter_value_t<Container>&&, Container>
		{
			iter = container->insert(iter, std::move(value));
			++iter;
			return *this;
		}
		constexpr insert_iterator& operator*() {
			return *this;
		}
		constexpr insert_iterator& operator++() {
			return *this;
		}
		constexpr insert_iterator& operator++(int) {
			return *this;
		}
	private:
		detail::raw_ptr<Container> container = nullptr;
		iterator_t<Container> iter = iterator_t<Container>();
	};

	template<detail::MemberValueType Container>
	constexpr auto inserter(Container& x, iterator_t<Container> i)
	STL2_NOEXCEPT_RETURN(
		insert_iterator<Container>{x, std::move(i)}
	)
} STL2_CLOSE_NAMESPACE

#endif
