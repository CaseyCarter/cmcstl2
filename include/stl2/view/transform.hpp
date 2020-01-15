// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_TRANSFORM_HPP
#define STL2_VIEW_TRANSFORM_HPP

#include <functional>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template<input_range V, copy_constructible F>
	requires view<V> && std::is_object_v<F> &&
		regular_invocable<F&, iter_reference_t<iterator_t<V>>>
	class transform_view : public view_interface<transform_view<V, F>> {
	private:
		template<bool> class __iterator;
		template<bool> class __sentinel;

		V base_ = V();
		detail::semiregular_box<F> fun_;

	public:
		transform_view() = default;

		constexpr transform_view(V base, F fun)
		: base_(std::move(base)), fun_(std::move(fun)) {}

		constexpr V base() const { return base_; }

		constexpr __iterator<false> begin()
		{ return {*this, __stl2::begin(base_)}; }

		// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		template<class ConstV = const V>
		constexpr __iterator<true> begin() const requires range<ConstV> &&
			regular_invocable<const F&, iter_reference_t<iterator_t<ConstV>>>
		{ return {*this, __stl2::begin(base_)}; }

		constexpr auto end() {
			if constexpr (common_range<V>) {
				return __iterator<false>{*this, __stl2::end(base_)};
			} else {
				return __sentinel<false>{__stl2::end(base_)};
			}
		}

		// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		template<class ConstV = const V>
		constexpr auto end() const requires range<ConstV> &&
			regular_invocable<const F&, iter_reference_t<iterator_t<ConstV>>>
		{
			if constexpr (common_range<const V>) {
				return __iterator<true>{*this, __stl2::end(base_)};
			} else {
				return __sentinel<true>{__stl2::end(base_)};
			}
		}

		constexpr auto size() requires sized_range<V>
		{ return __stl2::size(base_); }

		constexpr auto size() const requires sized_range<const V>
		{ return __stl2::size(base_); }
	};

	template<class R, class F>
	transform_view(R&& r, F fun) -> transform_view<all_view<R>, F>;

	template<input_range V, copy_constructible F>
	requires view<V> && std::is_object_v<F> &&
		regular_invocable<F&, iter_reference_t<iterator_t<V>>>
	template<bool Const>
	class transform_view<V, F>::__iterator {
	private:
		using Parent = __maybe_const<Const, transform_view>;
		using Base = __maybe_const<Const, V>;
		iterator_t<Base> current_ {};
		Parent* parent_ = nullptr;
		friend __iterator<!Const>;
		friend __sentinel<Const>;
	public:
		using iterator_category = iterator_category_t<iterator_t<Base>>;
		using value_type =
			__uncvref<invoke_result_t<F&, iter_reference_t<iterator_t<Base>>>>;
		using difference_type = iter_difference_t<iterator_t<Base>>;

		__iterator() = default;

		constexpr __iterator(Parent& parent, iterator_t<Base> current)
		: current_(current), parent_(&parent) {}

		constexpr __iterator(__iterator<!Const> i)
		requires Const && convertible_to<iterator_t<V>, iterator_t<Base>>
		: current_(std::move(i.current_)), parent_(i.parent_) {}

		constexpr iterator_t<Base> base() const
		{ return current_; }

		constexpr decltype(auto) operator*() const
		noexcept(noexcept(invoke(parent_->fun_.get(), *current_)))
		{ return invoke(parent_->fun_.get(), *current_); }

		constexpr __iterator& operator++()
		{
			++current_;
			return *this;
		}
		constexpr void operator++(int)
		{ ++current_; }
		constexpr __iterator operator++(int) requires forward_range<Base>
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		constexpr __iterator& operator--() requires bidirectional_range<Base>
		{
			--current_;
			return *this;
		}
		constexpr __iterator operator--(int) requires bidirectional_range<Base>
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		constexpr __iterator& operator+=(difference_type n)
		requires random_access_range<Base>
		{
			current_ += n;
			return *this;
		}
		constexpr __iterator& operator-=(difference_type n)
		requires random_access_range<Base>
		{
			current_ -= n;
			return *this;
		}
		constexpr decltype(auto) operator[](difference_type n) const
		requires random_access_range<Base>
		{ return invoke(parent_->fun_.get(), current_[n]); }

		friend constexpr bool operator==(const __iterator& x, const __iterator& y)
		requires equality_comparable<iterator_t<Base>>
		{ return x.current_ == y.current_; }

		friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
		requires equality_comparable<iterator_t<Base>>
		{ return !(x == y); }

		friend constexpr bool operator<(const __iterator& x, const __iterator& y)
		requires random_access_range<Base>
		{ return x.current_ < y.current_; }

		friend constexpr bool operator>(const __iterator& x, const __iterator& y)
		requires random_access_range<Base>
		{ return y < x; }

		friend constexpr bool operator<=(const __iterator& x, const __iterator& y)
		requires random_access_range<Base>
		{ return !(y < x); }

		friend constexpr bool operator>=(const __iterator& x, const __iterator& y)
		requires random_access_range<Base>
		{ return !(x < y); }

		friend constexpr __iterator operator+(__iterator i, difference_type n)
		requires random_access_range<Base>
		{ return __iterator{*i.parent_, i.current_ + n}; }

		friend constexpr __iterator operator+(difference_type n, __iterator i)
		requires random_access_range<Base>
		{ return __iterator{*i.parent_, i.current_ + n}; }

		friend constexpr __iterator operator-(__iterator i, difference_type n)
		requires random_access_range<Base>
		{ return __iterator{*i.parent_, i.current_ - n}; }

		friend constexpr difference_type operator-(const __iterator& x, const __iterator& y)
		requires random_access_range<Base>
		{ return x.current_ - y.current_; }

		friend constexpr decltype(auto) iter_move(const __iterator& i)
			noexcept(noexcept(*i))
		{
			if constexpr(std::is_lvalue_reference_v<decltype(*i)>)
				return std::move(*i);
			else
				return *i;
		}

		friend constexpr void iter_swap(const __iterator& x, const __iterator& y)
			noexcept(noexcept(__stl2::iter_swap(x.current_, y.current_)))
		{ __stl2::iter_swap(x.current_, y.current_); }
	};

	template<input_range V, copy_constructible F>
	requires view<V> && std::is_object_v<F> &&
		regular_invocable<F&, iter_reference_t<iterator_t<V>>>
	template<bool Const>
	class transform_view<V, F>::__sentinel {
	private:
		using Parent = __maybe_const<Const, transform_view>;
		using Base = __maybe_const<Const, V>;
		sentinel_t<Base> end_ {};
		friend __sentinel<!Const>;

		constexpr bool equal(const __iterator<Const>& i) const {
			return i.current_ == end_;
		}
		constexpr iter_difference_t<iterator_t<Base>>
		distance(const __iterator<Const>& i) const {
			return i.current_ - end_;
		}
	public:
		__sentinel() = default;
		explicit constexpr __sentinel(sentinel_t<Base> end)
		: end_(end) {}
		constexpr __sentinel(__sentinel<!Const> i)
		requires Const && convertible_to<sentinel_t<V>, sentinel_t<Base>>
		: end_(std::move(i.end_)) {}

		constexpr sentinel_t<Base> base() const
		{ return end_; }

		friend constexpr bool operator==(const __iterator<Const>& x, const __sentinel& y)
		{ return y.equal(x); }

		friend constexpr bool operator==(const __sentinel& x, const __iterator<Const>& y)
		{ return x.equal(y); }

		friend constexpr bool operator!=(const __iterator<Const>& x, const __sentinel& y)
		{ return !y.equal(x); }

		friend constexpr bool operator!=(const __sentinel& x, const __iterator<Const>& y)
		{ return !x.equal(y); }

		friend constexpr iter_difference_t<iterator_t<Base>>
		operator-(const __iterator<Const>& x, const __sentinel& y)
		requires sized_sentinel_for<sentinel_t<Base>, iterator_t<Base>>
		{ return -y.distance(x); }

		friend constexpr iter_difference_t<iterator_t<Base>>
		operator-(const __sentinel& y, const __iterator<Const>& x)
		requires sized_sentinel_for<sentinel_t<Base>, iterator_t<Base>>
		{ return y.distance(x); }
	};

	namespace views {
		struct __transform_fn {
			template<input_range Rng, copy_constructible F>
			requires
				viewable_range<Rng> && invocable<F&, iter_reference_t<iterator_t<Rng>>>
			constexpr auto operator()(Rng&& rng, F fun) const {
				return transform_view{std::forward<Rng>(rng), std::move(fun)};
			}

			template<copy_constructible F>
			constexpr auto operator()(F fun) const {
				return detail::view_closure{*this, std::move(fun)};
			}
		};

		inline constexpr __transform_fn transform;
	} // namespace views
} STL2_CLOSE_NAMESPACE

#endif
