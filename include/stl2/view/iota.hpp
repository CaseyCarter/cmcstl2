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
		template<weakly_incrementable I>
		struct iota_view_iterator_base {
			using iterator_category = __stl2::input_iterator_tag;
		};
		template<incrementable I>
		struct iota_view_iterator_base<I> {
			using iterator_category = __stl2::forward_iterator_tag;
		};
		template<ext::Decrementable I>
		struct iota_view_iterator_base<I> {
			using iterator_category = __stl2::bidirectional_iterator_tag;
		};
		template<ext::RandomAccessIncrementable I>
		struct iota_view_iterator_base<I> {
			using iterator_category = __stl2::random_access_iterator_tag;
		};
	}

	template<weakly_incrementable I, semiregular Bound = unreachable_sentinel_t>
	requires WeaklyEqualityComparable<I, Bound>
	struct iota_view;

	namespace __iota_view_detail {
		struct __adl_hook {};

		// Extension: iota_view models forwarding-range, as suggested by
		// https://github.com/ericniebler/stl2/issues/575
		template<class I, class S>
		constexpr iterator_t<iota_view<I, S>> begin(iota_view<I, S> r) {
			return r.begin();
		}
		template<class I, class S>
		constexpr sentinel_t<iota_view<I, S>> end(iota_view<I, S> r) {
			return r.end();
		}
	}

	template<weakly_incrementable I, semiregular Bound>
	requires WeaklyEqualityComparable<I, Bound>
	struct STL2_EMPTY_BASES iota_view
	: private __iota_view_detail::__adl_hook
	, view_interface<iota_view<I, Bound>> {
	private:
		struct __iterator;
		struct __sentinel;

		I value_ {};
		Bound bound_ {};
	public:
		iota_view() = default;
		/// \pre: `Bound{}` is reachable from `value`
		constexpr explicit iota_view(I value)
		: value_(value), bound_{} {}
		template<class II = I, class BB = Bound>
		constexpr iota_view(meta::id_t<II> value, meta::id_t<BB> bound)
		: value_(value), bound_(bound) {}

		constexpr __iterator begin() const
		{ return __iterator{value_}; }
		constexpr __sentinel end() const
		{ return __sentinel{bound_}; }
		constexpr __iterator end() const requires same_as<I, Bound>
		{ return __iterator{bound_}; }

		template<class II = I, class BB = Bound> // gcc_bugs_bugs_bugs
		constexpr auto size() const
		requires (same_as<II, BB> && ext::RandomAccessIncrementable<II>) ||
			(integral<II> && integral<BB>) ||
			sized_sentinel_for<BB, II>
		{ return bound_ - value_; }
	};

	template<weakly_incrementable I, semiregular Bound>
	requires WeaklyEqualityComparable<I, Bound> &&
		(!integral<I> || !integral<Bound> || std::is_signed_v<I> == std::is_signed_v<Bound>)
	iota_view(I, Bound) -> iota_view<I, Bound>;

	template<weakly_incrementable I, semiregular Bound>
	requires WeaklyEqualityComparable<I, Bound>
	struct iota_view<I, Bound>::__iterator
	: detail::iota_view_iterator_base<I> {
		friend __sentinel;

		using value_type = I;
		using difference_type = iter_difference_t<I>;

		__iterator() = default;
		constexpr explicit __iterator(I value)
		: value_(value) {}

		constexpr I operator*() const noexcept(std::is_nothrow_copy_constructible_v<I>)
		{ return value_; }

		constexpr __iterator& operator++()
		{
			++value_;
			return *this;
		}
		constexpr void operator++(int)
		{ ++*this; }
		constexpr __iterator operator++(int) requires incrementable<I>
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		constexpr __iterator& operator--() requires ext::Decrementable<I>
		{
			--value_;
			return *this;
		}
		constexpr __iterator operator--(int) requires ext::Decrementable<I>
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		constexpr __iterator& operator+=(difference_type n)
		requires ext::RandomAccessIncrementable<I>
		{
			value_ += n;
			return *this;
		}
		constexpr __iterator& operator-=(difference_type n)
		requires ext::RandomAccessIncrementable<I>
		{
			value_ -= n;
			return *this;
		}
		constexpr I operator[](difference_type n) const
		requires ext::RandomAccessIncrementable<I>
		{ return value_ + n; }

		friend constexpr bool operator==(const __iterator& x, const __iterator& y)
		requires equality_comparable<I>
		{ return x.value_ == y.value_; }
		friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
		requires equality_comparable<I>
		{ return !(x == y); }

		friend constexpr bool operator<(const __iterator& x, const __iterator& y)
		requires totally_ordered<I>
		{ return x.value_ < y.value_; }
		friend constexpr bool operator>(const __iterator& x, const __iterator& y)
		requires totally_ordered<I>
		{ return y < x; }
		friend constexpr bool operator<=(const __iterator& x, const __iterator& y)
		requires totally_ordered<I>
		{ return !(y < x); }
		friend constexpr bool operator>=(const __iterator& x, const __iterator& y)
		requires totally_ordered<I>
		{ return !(x < y); }

		friend constexpr __iterator operator+(__iterator i, difference_type n)
		requires ext::RandomAccessIncrementable<I>
		{ return __iterator{*i + n}; }
		friend constexpr __iterator operator+(difference_type n, __iterator i)
		requires ext::RandomAccessIncrementable<I>
		{ return i + n; }

		friend constexpr __iterator operator-(__iterator i, difference_type n)
		requires ext::RandomAccessIncrementable<I>
		{ return i + -n; }
		friend constexpr difference_type operator-(const __iterator& x, const __iterator& y)
		requires ext::RandomAccessIncrementable<I>
		{ return *x - *y; }

	private:
		I value_ {};
	};

	template<weakly_incrementable I, semiregular Bound>
	requires WeaklyEqualityComparable<I, Bound>
	struct iota_view<I, Bound>::__sentinel {
		__sentinel() = default;
		constexpr explicit __sentinel(Bound bound)
		: bound_(bound) {}

		friend constexpr bool operator==(const __iterator& x, const __sentinel& y)
		{ return y.equal(x); }
		friend constexpr bool operator==(const __sentinel& x, const __iterator& y)
		{ return x.equal(y); }
		friend constexpr bool operator!=(const __iterator& x, const __sentinel& y)
		{ return !y.equal(x); }
		friend constexpr bool operator!=(const __sentinel& x, const __iterator& y)
		{ return !x.equal(y); }

	private:
		constexpr bool equal(const __iterator& i) const {
			return i.value_ == bound_;
		}

		Bound bound_;
	};

	namespace views {
		struct __iota_fn {
			template<weakly_incrementable I>
			constexpr auto operator()(I value) const {
				return iota_view{value};
			}

			template<weakly_incrementable I, semiregular Bound>
			requires WeaklyEqualityComparable<I, Bound> &&
				(!integral<I> || !integral<Bound> ||
					std::is_signed_v<I> == std::is_signed_v<Bound>)
			constexpr auto operator()(I value, Bound bound) const {
				return iota_view{value, bound};
			}
		};

		inline constexpr __iota_fn iota {};
	}
} STL2_CLOSE_NAMESPACE

#endif
