// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-2016
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_IOTA_HPP
#define STL2_VIEW_IOTA_HPP

#include <stl2/iterator.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template <WeaklyIncrementable I>
		struct iota_view_iterator_base {
			using iterator_category = __stl2::input_iterator_tag;
		};
		template <Incrementable I>
		struct iota_view_iterator_base<I> {
			using iterator_category = __stl2::forward_iterator_tag;
		};
		template <ext::Decrementable I>
		struct iota_view_iterator_base<I> {
			using iterator_category = __stl2::bidirectional_iterator_tag;
		};
		template <ext::RandomAccessIncrementable I>
		struct iota_view_iterator_base<I> {
			using iterator_category = __stl2::random_access_iterator_tag;
		};
	}
	namespace ext {
		template <WeaklyIncrementable I, Semiregular Bound = unreachable>
		requires WeaklyEqualityComparable<I, Bound>
		struct iota_view : view_interface<iota_view<I, Bound>> {
		private:
			I value_ {};
			Bound bound_ {};
		public:
			iota_view() = default;
			/// \pre: `Bound{}` is reachable from `value`
			constexpr explicit iota_view(I value)
			: value_(value), bound_{} {}
			template <class II = I, class BB = Bound>
			constexpr iota_view(meta::id_t<II> value, meta::id_t<BB> bound)
			: value_(value), bound_(bound) {}

			struct iterator;
			struct sentinel;

			constexpr iterator begin() const
			{ return iterator{value_}; }
			constexpr sentinel end() const
			{ return sentinel{bound_}; }
			constexpr iterator end() const requires Same<I, Bound>
			{ return iterator{bound_}; }

			constexpr auto size() const
			requires (Same<I, Bound> && ext::RandomAccessIncrementable<I>) ||
				(Integral<I> && Integral<Bound>) ||
				SizedSentinel<Bound, I>
			{ return bound_ - value_; }
		};

		template <WeaklyIncrementable I>
		iota_view(I) -> iota_view<I>;

		template <WeaklyIncrementable I, Semiregular Bound>
		requires WeaklyEqualityComparable<I, Bound> &&
			(!Integral<I> || !Integral<Bound> || std::is_signed_v<I> == std::is_signed_v<Bound>)
		iota_view(I, Bound) -> iota_view<I, Bound>;

		template <class I, class Bound>
		struct iota_view<I, Bound>::iterator
		: detail::iota_view_iterator_base<I> {
		private:
			I value_ {};
		public:
			using value_type = I;
			using difference_type = difference_type_t<I>;

			iterator() = default;
			constexpr explicit iterator(I value)
			: value_(value) {}

			constexpr I operator*() const noexcept(std::is_nothrow_copy_constructible_v<I>)
			{ return value_; }

			constexpr iterator& operator++()
			{
				++value_;
				return *this;
			}
			constexpr void operator++(int)
			{ ++*this; }
			constexpr iterator operator++(int) requires Incrementable<I>
			{
				auto tmp = *this;
				++*this;
				return tmp;
			}

			constexpr iterator& operator--() requires ext::Decrementable<I>
			{
				--value_;
				return *this;
			}
			constexpr iterator operator--(int) requires ext::Decrementable<I>
			{
				auto tmp = *this;
				--*this;
				return tmp;
			}

			constexpr iterator& operator+=(difference_type n)
			requires ext::RandomAccessIncrementable<I>
			{
				value_ += n;
				return *this;
			}
			constexpr iterator& operator-=(difference_type n)
			requires ext::RandomAccessIncrementable<I>
			{
				value_ -= n;
				return *this;
			}
			constexpr I operator[](difference_type n) const
			requires ext::RandomAccessIncrementable<I>
			{ return value_ + n; }

			friend constexpr bool operator==(const iterator& x, const iterator& y)
			requires EqualityComparable<I>
			{ return x.value_ == y.value_; }
			friend constexpr bool operator!=(const iterator& x, const iterator& y)
			requires EqualityComparable<I>
			{ return !(x == y); }

			friend constexpr bool operator<(const iterator& x, const iterator& y)
			requires StrictTotallyOrdered<I>
			{ return x.value_ < y.value_; }
			friend constexpr bool operator>(const iterator& x, const iterator& y)
			requires StrictTotallyOrdered<I>
			{ return y < x; }
			friend constexpr bool operator<=(const iterator& x, const iterator& y)
			requires StrictTotallyOrdered<I>
			{ return !(y < x); }
			friend constexpr bool operator>=(const iterator& x, const iterator& y)
			requires StrictTotallyOrdered<I>
			{ return !(x < y); }

			friend constexpr iterator operator+(iterator i, difference_type n)
			requires ext::RandomAccessIncrementable<I>
			{ return iterator{*i + n}; }
			friend constexpr iterator operator+(difference_type n, iterator i)
			requires ext::RandomAccessIncrementable<I>
			{ return i + n; }

			friend constexpr iterator operator-(iterator i, difference_type n)
			requires ext::RandomAccessIncrementable<I>
			{ return i + -n; }
			friend constexpr difference_type operator-(const iterator& x, const iterator& y)
			requires ext::RandomAccessIncrementable<I>
			{ return *x - *y; }
		};

		template <class I, class Bound>
		struct iota_view<I, Bound>::sentinel {
		private:
			Bound bound_;
		public:
			sentinel() = default;
			constexpr explicit sentinel(Bound bound)
			: bound_(bound) {}

			friend constexpr bool operator==(const iterator& x, const sentinel& y)
			{ return x.value_ == y.bound_; }
			friend constexpr bool operator==(const sentinel& x, const iterator& y)
			{ return y == x; }
			friend constexpr bool operator!=(const iterator& x, const sentinel& y)
			{ return !(x == y); }
			friend constexpr bool operator!=(const sentinel& x, const iterator& y)
			{ return !(y == x); }
		};
	} // namespace ext

	namespace view {
		namespace __iota {
			struct fn {
				template <WeaklyIncrementable I>
				constexpr auto operator()(I value) const {
					return ext::iota_view{value};
				}

				template <WeaklyIncrementable I, Semiregular Bound>
				requires WeaklyEqualityComparable<I, Bound> &&
					(!Integral<I> || !Integral<Bound> ||
						std::is_signed_v<I> == std::is_signed_v<Bound>)
				constexpr auto operator()(I value, Bound bound) const {
					return ext::iota_view{value, bound};
				}
			};
		}

		inline constexpr __iota::fn iota {};
	}
} STL2_CLOSE_NAMESPACE

#endif
