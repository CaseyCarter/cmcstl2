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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/optional.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/view/view_closure.hpp>

#include <functional>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <InputRange R, CopyConstructible F>
		requires View<R> && Invocable<F&, reference_t<iterator_t<R>>>
		class transform_view : public view_interface<transform_view<R, F>> {
		private:
			R base_;
			detail::semiregular_box<F> fun_;
			template <bool Const> struct __iterator;
			template <bool Const> struct __sentinel;
		public:
			transform_view() = default;

			constexpr transform_view(R base, F fun)
			: base_(std::move(base)), fun_(std::move(fun)) {}

			template <InputRange O>
			requires (std::is_lvalue_reference_v<O> || View<__f<O>>) &&
				Constructible<R, all_view<O>>
			constexpr transform_view(O&& o, F fun)
			: base_(view::all(std::forward<O>(o))), fun_(std::move(fun)) {}

			using iterator = __iterator<false>;
			using const_iterator = __iterator<true>;
			using sentinel = __sentinel<false>;
			using const_sentinel = __sentinel<true>;

			constexpr R base() const
			{ return base_; }

			constexpr iterator begin()
			{ return {*this, __stl2::begin(base_)}; }

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstR = const R>
			constexpr const_iterator begin() const requires Range<ConstR> &&
				Invocable<const F&, reference_t<iterator_t<ConstR>>>
			{ return {*this, __stl2::begin(base_)}; }

			constexpr sentinel end()
			{ return sentinel{__stl2::end(base_)}; }

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstR = const R>
			constexpr const_sentinel end() const requires Range<ConstR> &&
				Invocable<const F&, reference_t<iterator_t<ConstR>>>
			{ return const_sentinel{__stl2::end(base_)}; }

			constexpr iterator end() requires BoundedRange<R>
			{ return {*this, __stl2::end(base_)}; }

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstR = const R>
			constexpr const_iterator end() const requires BoundedRange<ConstR> &&
				Invocable<const F&, reference_t<iterator_t<ConstR>>>
			{ return {*this, __stl2::end(base_)}; }

			constexpr auto size() requires SizedRange<R>
			{ return __stl2::size(base_); }

			constexpr auto size() const requires SizedRange<const R>
			{ return __stl2::size(base_); }
		};

		template <class R, class F>
		transform_view(R&& r, F fun) -> transform_view<all_view<R>, F>;

		template <class R, class F>
		template <bool Const>
		class transform_view<R, F>::__iterator {
		private:
			using Parent = __maybe_const<Const, transform_view>;
			using Base = __maybe_const<Const, R>;
			iterator_t<Base> current_ {};
			Parent* parent_ = nullptr;
			friend __iterator<!Const>;
			friend __sentinel<Const>;
		public:
			using iterator_category = iterator_category_t<iterator_t<Base>>;
			using value_type =
				__uncvref<std::invoke_result_t<F&, reference_t<iterator_t<Base>>>>;
			using difference_type = difference_type_t<iterator_t<Base>>;

			__iterator() = default;

			constexpr __iterator(Parent& parent, iterator_t<Base> current)
			: current_(current), parent_(&parent) {}

			constexpr __iterator(__iterator<!Const> i)
			requires Const && ConvertibleTo<iterator_t<R>, iterator_t<Base>>
			: current_(i.current_), parent_(i.parent_) {}

			constexpr iterator_t<Base> base() const
			{ return current_; }
			constexpr decltype(auto) operator*() const
			{ return invoke(parent_->fun_.get(), *current_); }

			constexpr __iterator& operator++()
			{
				++current_;
				return *this;
			}
			constexpr void operator++(int)
			{ ++current_; }
			constexpr __iterator operator++(int) requires ForwardRange<Base>
			{
				auto tmp = *this;
				++*this;
				return tmp;
			}

			constexpr __iterator& operator--() requires BidirectionalRange<Base>
			{
				--current_;
				return *this;
			}
			constexpr __iterator operator--(int) requires BidirectionalRange<Base>
			{
				auto tmp = *this;
				--*this;
				return tmp;
			}

			constexpr __iterator& operator+=(difference_type n)
			requires RandomAccessRange<Base>
			{
				current_ += n;
				return *this;
			}
			constexpr __iterator& operator-=(difference_type n)
			requires RandomAccessRange<Base>
			{
				current_ -= n;
				return *this;
			}
			constexpr decltype(auto) operator[](difference_type n) const
			requires RandomAccessRange<Base>
			{ return invoke(parent_->fun_.get(), current_[n]); }

			friend constexpr bool operator==(const __iterator& x, const __iterator& y)
			requires EqualityComparable<iterator_t<Base>>
			{ return x.current_ == y.current_; }

			friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
			requires EqualityComparable<iterator_t<Base>>
			{ return !(x == y); }

			friend constexpr bool operator<(const __iterator& x, const __iterator& y)
			requires RandomAccessRange<Base>
			{ return x.current_ < y.current_; }

			friend constexpr bool operator>(const __iterator& x, const __iterator& y)
			requires RandomAccessRange<Base>
			{ return y < x; }

			friend constexpr bool operator<=(const __iterator& x, const __iterator& y)
			requires RandomAccessRange<Base>
			{ return !(y < x); }

			friend constexpr bool operator>=(const __iterator& x, const __iterator& y)
			requires RandomAccessRange<Base>
			{ return !(x < y); }

			friend constexpr __iterator operator+(__iterator i, difference_type n)
			requires RandomAccessRange<Base>
			{ return __iterator{*i.parent_, i.current_ + n}; }

			friend constexpr __iterator operator+(difference_type n, __iterator i)
			requires RandomAccessRange<Base>
			{ return __iterator{*i.parent_, i.current_ + n}; }

			friend constexpr __iterator operator-(__iterator i, difference_type n)
			requires RandomAccessRange<Base>
			{ return __iterator{*i.parent_, i.current_ - n}; }

			friend constexpr difference_type operator-(const __iterator& x, const __iterator& y)
			requires RandomAccessRange<Base>
			{ return x.current_ - y.current_; }

			friend constexpr decltype(auto) iter_move(const __iterator& i)
				noexcept(noexcept(invoke(i.parent_->fun_.get(), *i.current_)))
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

		template <class R, class F>
		template <bool Const>
		class transform_view<R, F>::__sentinel {
		private:
			using Parent = meta::if_c<Const, const transform_view, transform_view>;
			using Base = meta::if_c<Const, const R, R>;
			sentinel_t<Base> end_ {};
			friend __sentinel<!Const>;
		public:
			__sentinel() = default;
			explicit constexpr __sentinel(sentinel_t<Base> end)
			: end_(end) {}
			constexpr __sentinel(__sentinel<!Const> i)
			requires Const && ConvertibleTo<sentinel_t<R>, sentinel_t<const R>>
			: end_(i.end_) {}

			constexpr sentinel_t<Base> base() const
			{ return end_; }

			friend constexpr bool operator==(const __iterator<Const>& x, const __sentinel& y)
			{ return x.current_ == y.end_; }

			friend constexpr bool operator==(const __sentinel& x, const __iterator<Const>& y)
			{ return y == x; }

			friend constexpr bool operator!=(const __iterator<Const>& x, const __sentinel& y)
			{ return !(x == y); }

			friend constexpr bool operator!=(const __sentinel& x, const __iterator<Const>& y)
			{ return !(y == x); }

			friend constexpr difference_type_t<iterator_t<Base>>
			operator-(const __iterator<Const>& x, const __sentinel& y)
			requires SizedSentinel<sentinel_t<Base>, iterator_t<Base>>
			{ return x.current_ - y.end_; }

			friend constexpr difference_type_t<iterator_t<Base>>
			operator-(const __sentinel& y, const __iterator<Const>& x)
			requires SizedSentinel<sentinel_t<Base>, iterator_t<Base>>
			{ return x.end_ - y.current_; }
		};
	} // namespace ext

	namespace view {
		namespace __transform {
			struct fn {
				template <InputRange Rng, CopyConstructible F>
				requires Invocable<F&, reference_t<iterator_t<Rng>>> &&
					(std::is_lvalue_reference_v<Rng> || View<__f<Rng>>)
				constexpr auto operator()(Rng&& rng, F fun) const {
					return ext::transform_view{std::forward<Rng>(rng), std::move(fun)};
				}

				template <CopyConstructible F>
				constexpr auto operator()(F fun) const {
					return detail::view_closure{*this, std::move(fun)};
				}
			};
		}

		inline constexpr __transform::fn transform {};
	}
} STL2_CLOSE_NAMESPACE

#endif
