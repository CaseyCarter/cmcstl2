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
	template<input_or_output_iterator> class counted_iterator;

	namespace __counted_iterator {
		struct access {
			template<_SpecializationOf<counted_iterator> CI>
			static constexpr auto&& current(CI&& ci) noexcept {
				return std::forward<CI>(ci).current_;
			}
			template<_SpecializationOf<counted_iterator> CI>
			static constexpr auto&& count(CI&& ci) noexcept {
				return std::forward<CI>(ci).length_;
			}
		};

#if STL2_WORKAROUND_CLANG_37556
		template<class I>
		constexpr iter_rvalue_reference_t<I>
		iter_move(const counted_iterator<I>& i)
			noexcept(noexcept(__stl2::iter_move(access::current(i))))
			requires input_iterator<I> {
			return __stl2::iter_move(access::current(i));
		}
		template<class I, indirectly_swappable<I> I2>
		constexpr void iter_swap(
			const counted_iterator<I>& x, const counted_iterator<I2>& y)
			noexcept(noexcept(__stl2::iter_swap(
				std::declval<const I&>(),
				std::declval<const I2&>()))) {
			__stl2::iter_swap(access::current(x), access::current(y));
		}
#endif // STL2_WORKAROUND_CLANG_37556
	}

	// counted_iterator [counted.iterator]
	template<input_or_output_iterator I>
	class counted_iterator
#if STL2_WORKAROUND_CLANG_37556
	: __counted_iterator::access
#endif // STL2_WORKAROUND_CLANG_37556
	{
		friend __counted_iterator::access;

		I current_;
		iter_difference_t<I> length_;

	public:
		using iterator_type = I;

		constexpr counted_iterator() = default;
		constexpr counted_iterator(I x, iter_difference_t<I> n)
		noexcept(std::is_nothrow_move_constructible_v<I>) // strengthened
		: current_(std::move(x)), length_{n} {
			STL2_EXPECT(n >= 0);
		}
		template<class I2>
		requires convertible_to<const I2&, I>
		constexpr counted_iterator(const counted_iterator<I2>& i)
		noexcept(std::is_nothrow_copy_constructible_v<I>) // strengthened
		: current_(__counted_iterator::access::current(i))
		, length_{__counted_iterator::access::count(i)} {}
		template<class I2>
		requires assignable_from<I&, const I2&>
		constexpr counted_iterator& operator=(const counted_iterator<I2>& i)
		noexcept(std::is_nothrow_assignable_v<I&, const I2&>) { // strengthened
			current_ = __counted_iterator::access::current(i);
			length_ = __counted_iterator::access::count(i);
			return *this;
		}
		constexpr I base() const
		noexcept(std::is_nothrow_copy_constructible_v<I>) { // strengthened
			return current_;
		}
		constexpr iter_difference_t<I> count() const noexcept {
			return length_;
		}
		constexpr decltype(auto) operator*()
		noexcept(noexcept(*std::declval<I&>())) { // strengthened
			return *current_;
		}
		constexpr decltype(auto) operator*() const
		noexcept(noexcept(*std::declval<const I&>())) // strengthened
		requires __dereferenceable<const I> {
			return *current_;
		}
		constexpr counted_iterator& operator++() {
			STL2_EXPECT(length_ > 0);
			++current_;
			--length_;
			return *this;
		}
		/* STL2_CXX20_CONSTEXPR */ decltype(auto) operator++(int) {
			STL2_EXPECT(length_ > 0);
			--length_;
			try {
				return current_++;
			} catch(...) {
				++length_;
				throw;
			}
		}
		constexpr counted_iterator operator++(int) requires forward_iterator<I> {
			auto tmp = *this;
			++*this;
			return tmp;
		}
		constexpr counted_iterator& operator--()
		requires bidirectional_iterator<I> {
			--current_;
			++length_;
			return *this;
		}
		constexpr counted_iterator operator--(int)
		requires bidirectional_iterator<I> {
			auto tmp = *this;
			--*this;
			return tmp;
		}
		constexpr counted_iterator operator+(iter_difference_t<I> n) const
		requires random_access_iterator<I> {
			STL2_EXPECT(n <= length_);
			return counted_iterator(current_ + n, length_ - n);
		}
		friend constexpr counted_iterator
		operator+(iter_difference_t<I> n, const counted_iterator& x)
		requires random_access_iterator<I> {
			return x + n;
		}
		constexpr counted_iterator& operator+=(iter_difference_t<I> n)
		requires random_access_iterator<I> {
			STL2_EXPECT(n <= length_);
			current_ += n;
			length_ -= n;
			return *this;
		}
		constexpr counted_iterator operator-(iter_difference_t<I> n) const
		requires random_access_iterator<I> {
			STL2_EXPECT(-n <= length_);
			return counted_iterator(current_ - n, length_ + n);
		}
		template<common_with<I> I2>
		friend constexpr iter_difference_t<I2>
		operator-(const counted_iterator& x, const counted_iterator<I2>& y)
		noexcept { // strengthened
			return y.count() - x.length_;
		}
		friend constexpr iter_difference_t<I>
		operator-(const counted_iterator& x, default_sentinel_t)
		noexcept { // strengthened
			return -x.length_;
		}
		friend constexpr iter_difference_t<I>
		operator-(default_sentinel_t, const counted_iterator& y)
		noexcept { // strengthened
			return y.length_;
		}
		constexpr counted_iterator& operator-=(iter_difference_t<I> n)
		requires random_access_iterator<I> {
			STL2_EXPECT(-n <= length_);
			current_ -= n;
			length_ += n;
			return *this;
		}
		constexpr decltype(auto) operator[](iter_difference_t<I> n) const
		noexcept(noexcept(std::declval<const I&>()[n])) // strengthened
		requires random_access_iterator<I> {
			STL2_EXPECT(n <= length_);
			return current_[n];
		}

		template<common_with<I> I2>
		friend constexpr bool
		operator==(const counted_iterator& x, const counted_iterator<I2>& y)
		noexcept { // strengthened
			return x.length_ == y.count();
		}
		friend constexpr bool
		operator==(const counted_iterator& x, default_sentinel_t)
		noexcept { // strengthened
			return x.length_ == 0;
		}
		friend constexpr bool
		operator==(default_sentinel_t, const counted_iterator& y)
		noexcept { // strengthened
			return y.length_ == 0;
		}
		template<common_with<I> I2>
		friend constexpr bool
		operator!=(const counted_iterator& x, const counted_iterator<I2>& y)
		 noexcept { // strengthened
			return !(x == y);
		}
		friend constexpr bool
		operator!=(const counted_iterator& x, default_sentinel_t y)
		 noexcept { // strengthened
			return !(x == y);
		}
		friend constexpr bool
		operator!=(default_sentinel_t x, const counted_iterator& y)
		 noexcept { // strengthened
			return !(x == y);
		}

		template<common_with<I> I2>
		friend constexpr bool
		operator<(const counted_iterator& x, const counted_iterator<I2>& y)
		noexcept { // strengthened
			return y.count() < x.length_;
		}
		template<common_with<I> I2>
		friend constexpr bool
		operator>(const counted_iterator& x, const counted_iterator<I2>& y)
		noexcept { // strengthened
			return y < x;
		}
		template<common_with<I> I2>
		friend constexpr bool
		operator<=(const counted_iterator& x, const counted_iterator<I2>& y)
		noexcept { // strengthened
			return !(y < x);
		}
		template<common_with<I> I2>
		friend constexpr bool
		operator>=(const counted_iterator& x, const counted_iterator<I2>& y)
		noexcept { // strengthened
			return !(x < y);
		}

#if !STL2_WORKAROUND_CLANG_37556
		friend constexpr iter_rvalue_reference_t<I>
		iter_move(const counted_iterator& i)
		noexcept(noexcept(__stl2::iter_move(i.current_)))
		requires input_iterator<I> {
			return __stl2::iter_move(i.current_);
		}
		template<indirectly_swappable<I> I2>
		friend constexpr void
		iter_swap(const counted_iterator& x, const counted_iterator<I2>& y)
		STL2_NOEXCEPT_RETURN(
			__stl2::iter_swap(x.current_,
				__counted_iterator::access::current(y))
		)
#endif // !STL2_WORKAROUND_CLANG_37556
	};

	template<class I>
	struct incrementable_traits<counted_iterator<I>> {
		using difference_type = iter_difference_t<I>;
	};

	template<readable I>
	struct readable_traits<counted_iterator<I>> {
		using value_type = iter_value_t<I>;
	};
	template<input_iterator I>
	struct iterator_category<counted_iterator<I>> {
		using type = iterator_category_t<I>;
	};

	template<input_or_output_iterator I>
	constexpr void __advance_fn::operator()(
		counted_iterator<I>& i, iter_difference_t<I> n) const
	{
		if constexpr (random_access_iterator<I>) {
			i += n;
		} else {
			auto& length = __counted_iterator::access::count(i);
			STL2_EXPECT(n <= length);
			(*this)(__counted_iterator::access::current(i), n);
			length -= n;
		}
	}

	namespace ext {
		template<input_or_output_iterator I>
		constexpr auto uncounted(const I& i)
		noexcept(std::is_nothrow_copy_constructible<I>::value) {
			return i;
		}

		template<class I>
		constexpr auto uncounted(const counted_iterator<I>& i)
		STL2_NOEXCEPT_RETURN(
			i.base()
		)

		template<input_or_output_iterator I>
		constexpr auto recounted(const I&, const I& i, iter_difference_t<I> = 0)
		noexcept(std::is_nothrow_copy_constructible<I>::value)
		{
			return i;
		}

		template<class I>
		constexpr auto recounted(const counted_iterator<I>& o, I i)
		STL2_NOEXCEPT_RETURN(
			counted_iterator<I>{std::move(i), o.count()}
		)

		template<class I>
		constexpr auto recounted(
			const counted_iterator<I>& o, I i, iter_difference_t<I> n)
		noexcept(noexcept(counted_iterator<I>{std::move(i), o.count() - n}))
		{
			STL2_EXPENSIVE_ASSERT(!forward_iterator<I> ||
				i == next(o.base(), n));
			return counted_iterator<I>{std::move(i), o.count() - n};
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
