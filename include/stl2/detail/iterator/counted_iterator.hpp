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
#include <stl2/detail/compressed_pair.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/iterator/operations.hpp>

STL2_OPEN_NAMESPACE {
	Iterator{I}
	class counted_iterator {
		Iterator{O} friend class counted_iterator;
		Iterator{O} friend STL2_CONSTEXPR_EXT_FWD
		void advance(counted_iterator<O>& i, difference_type_t<O> n);

		ext::compressed_pair<I, difference_type_t<I>> data_{};

		I& current() noexcept { return data_.first(); };
		const I& current() const noexcept { return data_.first(); };
		difference_type_t<I>& cnt() noexcept { return data_.second(); }
		const difference_type_t<I>& cnt() const noexcept {
			return data_.second();
		}
	public:
		using iterator_type = I;
		using difference_type = difference_type_t<I>;

		counted_iterator() = default;
		counted_iterator(I x, difference_type_t<I> n)
		: data_{__stl2::move(x), n} {
			STL2_EXPECT(n >= 0);
		}
		template <ConvertibleTo<I> U>
		counted_iterator(const counted_iterator<U>& i)
		: data_{i.current(), i.count()} {}
		template <ConvertibleTo<I> U>
		counted_iterator& operator=(const counted_iterator<U>& i) {
			current() = i.current();
			cnt() = i.cnt();
			return *this;
		}
		I base() const {
			return current();
		}
		difference_type_t<I> count() const {
			return cnt();
		}
		decltype(auto) operator*() noexcept(noexcept(*std::declval<I&>())) {
			return *current();
		}
		decltype(auto) operator*() const
			noexcept(noexcept(*std::declval<const I&>())) {
			return *current();
		}
		counted_iterator& operator++() {
			STL2_EXPECT(cnt() > 0);
			++current();
			--cnt();
			return *this;
		}
		// Not to spec:
		// https://github.com/ericniebler/stl2/issues/232
		decltype(auto) operator++(int) {
			STL2_EXPECT(cnt() > 0);
			--cnt();
			return current()++;
		}
		counted_iterator operator++(int) requires ForwardIterator<I>() {
			STL2_EXPECT(cnt() > 0);
			auto tmp(*this);
			++*this;
			return tmp;
		}
		counted_iterator& operator--() requires BidirectionalIterator<I>() {
			--current();
			++cnt();
			return *this;
		}
		counted_iterator operator--(int) requires BidirectionalIterator<I>() {
			auto tmp(*this);
			--*this;
			return tmp;
		}
		counted_iterator operator+(difference_type n) const
			requires RandomAccessIterator<I>() {
			STL2_EXPECT(n <= cnt());
			return counted_iterator(current() + n, cnt() - n);
		}
		counted_iterator& operator+=(difference_type n)
			requires RandomAccessIterator<I>() {
			STL2_EXPECT(n <= cnt());
			current() += n;
			cnt() -= n;
			return *this;
		}
		counted_iterator operator- (difference_type n) const
			requires RandomAccessIterator<I>() {
			STL2_EXPECT(-n <= cnt());
			return counted_iterator(current() - n, cnt() + n);
		}
		counted_iterator& operator-=(difference_type n)
			requires RandomAccessIterator<I>() {
			STL2_EXPECT(-n <= cnt());
			current() -= n;
			cnt() += n;
			return *this;
		}
		decltype(auto) operator[](difference_type n) const
			requires RandomAccessIterator<I>() {
			STL2_EXPECT(n <= cnt());
			return current()[n];
		}

		// Not to spec
		// https://github.com/ericniebler/stl2/issues/245
		friend constexpr rvalue_reference_t<I>
		iter_move(const counted_iterator& i)
			noexcept(noexcept(__stl2::iter_move(i.current())))
			requires InputIterator<I>() {
			return __stl2::iter_move(i.current());
		}
		template <IndirectlySwappable<I> I2>
		friend void iter_swap(
			const counted_iterator& x, const counted_iterator<I2>& y)
			noexcept(noexcept(__stl2::iter_swap(x.current(), y.current()))) {
			__stl2::iter_swap(x.current(), y.current());
		}
	};

	template <Readable I>
	struct value_type<counted_iterator<I>> {
		using type = value_type_t<I>;
	};
	template <InputIterator I>
	struct iterator_category<counted_iterator<I>> {
		using type = iterator_category_t<I>;
	};

	template <class I1, class I2>
		requires Common<I1, I2>()
	bool operator==(
		const counted_iterator<I1>& x, const counted_iterator<I2>& y) {
		return x.count() == y.count();
	}
	bool operator==(
		const counted_iterator<auto>& x, default_sentinel) {
		return x.count() == 0;
	}
	bool operator==(
		default_sentinel, const counted_iterator<auto>& x) {
		return x.count() == 0;
	}
	template <class I1, class I2>
		requires Common<I1, I2>()
	bool operator!=(
		const counted_iterator<I1>& x, const counted_iterator<I2>& y) {
		return !(x == y);
	}
	bool operator!=(
		const counted_iterator<auto>& x, default_sentinel y) {
		return !(x == y);
	}
	bool operator!=(
		default_sentinel x, const counted_iterator<auto>& y) {
		return !(x == y);
	}
	template <class I1, class I2>
		requires Common<I1, I2>()
	bool operator<(
		const counted_iterator<I1>& x, const counted_iterator<I2>& y) {
		return y.count() < x.count();
	}
	template <class I1, class I2>
		requires Common<I1, I2>()
	bool operator<=(
		const counted_iterator<I1>& x, const counted_iterator<I2>& y) {
		return !(y < x);
	}
	template <class I1, class I2>
		requires Common<I1, I2>()
	bool operator>(
		const counted_iterator<I1>& x, const counted_iterator<I2>& y) {
		return y < x;
	}
	template <class I1, class I2>
		requires Common<I1, I2>()
	bool operator>=(
		const counted_iterator<I1>& x, const counted_iterator<I2>& y) {
		return !(x < y);
	}
	template <class I1, class I2>
		requires Common<I1, I2>()
	difference_type_t<I2> operator-(
		const counted_iterator<I1>& x, const counted_iterator<I2>& y) {
		return y.count() - x.count();
	}
	template <class I>
	difference_type_t<I> operator-(
		const counted_iterator<I>& x, default_sentinel) {
		return -x.count();
	}
	template <class I>
	difference_type_t<I> operator-(
		default_sentinel, const counted_iterator<I>& y) {
		return y.count();
	}
	template <RandomAccessIterator I>
	counted_iterator<I>
	operator+(difference_type_t<I> n, const counted_iterator<I>& x) {
		STL2_EXPECT(n <= x.count());
		return x + n;
	}

	template <Iterator I>
	STL2_CONSTEXPR_EXT auto make_counted_iterator(I i, difference_type_t<I> n)
	STL2_NOEXCEPT_RETURN(
		counted_iterator<I>{__stl2::move(i), n}
	)

	Iterator{I}
	STL2_CONSTEXPR_EXT void advance(counted_iterator<I>& i, difference_type_t<I> n)	{
		STL2_EXPECT(n <= i.count());
		__stl2::advance(i.current(), n);
		i.cnt() -= n;
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
