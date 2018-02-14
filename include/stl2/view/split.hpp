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
#ifndef STL2_VIEW_SPLIT_HPP
#define STL2_VIEW_SPLIT_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/single.hpp>
#include <stl2/detail/view/view_closure.hpp>

#include <type_traits>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <class R>
		concept bool _TinyRange =
			SizedRange<R> && requires {
				requires std::remove_reference_t<R>::size() <= 1;
			};

		template <InputRange Rng, ForwardRange Pattern>
		requires View<Rng> && View<Pattern> &&
			IndirectlyComparable<iterator_t<Rng>, iterator_t<Pattern>> &&
			(ForwardRange<Rng> || _TinyRange<Pattern>)
		struct split_view {
		private:
			Rng base_ {};
			Pattern pattern_ {};
			iterator_t<Rng> current_ {}; // if Input
			template <bool Const> struct __outer_iterator;
			template <bool Const> struct __outer_sentinel;
			template <bool Const> struct __inner_iterator;
			template <bool Const> struct __inner_sentinel;
		public:
			split_view() = default;
			constexpr split_view(Rng base, Pattern pattern)
			: base_(std::move(base)), pattern_(std::move(pattern))
			{}

			template <InputRange O, ForwardRange P>
			requires ext::ViewableRange<O> && ext::ViewableRange<P> &&
				_ConstructibleFromRange<Rng, O> &&
				_ConstructibleFromRange<Pattern, P>
			constexpr split_view(O&& o, P&& p)
			: split_view{view::all(std::forward<O>(o)), view::all(std::forward<P>(p))} {}

			template <InputRange O>
			requires ext::ViewableRange<O> &&
				_ConstructibleFromRange<Rng, O> &&
				Constructible<Pattern, single_view<value_type_t<iterator_t<O>>>>
			constexpr split_view(O&& o, value_type_t<iterator_t<O>> e)
			: split_view{view::all(std::forward<O>(o)), single_view{std::move(e)}} {}

			using iterator = __outer_iterator<models::SimpleView<Rng>>;
			using sentinel = __outer_sentinel<models::SimpleView<Rng>>;
			using const_iterator = __outer_iterator<true>;
			using const_sentinel = __outer_sentinel<true>;

			constexpr iterator begin()
			{
				current_ = __stl2::begin(base_);
				return {*this};
			}

			constexpr iterator begin() requires ForwardRange<Rng>
			{ return {*this, __stl2::begin(base_)}; }

			constexpr const_iterator begin() const requires ForwardRange<const Rng>
			{ return {*this, __stl2::begin(base_)}; }

			constexpr sentinel end()
			{ return sentinel{*this}; }

			constexpr const_sentinel end() const requires ForwardRange<const Rng>
			{ return const_sentinel{*this}; }

			constexpr iterator end() requires ForwardRange<Rng> && BoundedRange<Rng>
			{ return {*this, __stl2::end(base_)}; }

			constexpr const_iterator end() const
			requires ForwardRange<const Rng> && BoundedRange<const Rng>
			{ return {*this, __stl2::end(base_)}; }
		};

		template <class O, class P>
		concept bool SplittableWithRange =
			InputRange<O> && ForwardRange<P> &&
			ext::ViewableRange<O> && ext::ViewableRange<P> &&
			IndirectlyComparable<iterator_t<O>, iterator_t<P>> &&
			(ForwardRange<O> || _TinyRange<P>);

		template <class O>
		concept bool SplittableWithElement =
			InputRange<O> && Semiregular<value_type_t<iterator_t<O>>> &&
			ext::ViewableRange<O> &&
			IndirectlyComparable<iterator_t<O>, const value_type_t<iterator_t<O>>*>;

		SplittableWithRange{Rng, Pattern}
		split_view(Rng&&, Pattern&&) -> split_view<all_view<Rng>, all_view<Pattern>>;

		SplittableWithElement{Rng}
		split_view(Rng&&, value_type_t<iterator_t<Rng>>)
			-> split_view<all_view<Rng>, single_view<value_type_t<iterator_t<Rng>>>>;

		template <class Rng, class Pattern>
		template <bool Const>
		struct split_view<Rng, Pattern>::__outer_iterator {
		private:
			using Base = __maybe_const<Const, Rng>;
			using Parent = __maybe_const<Const, split_view>;
			iterator_t<Base> current_ {}; // if !Input
			Parent* parent_ = nullptr;
			friend __outer_sentinel<Const>;
			friend __outer_iterator<!Const>;
			friend __inner_iterator<Const>;
			constexpr decltype(auto) current() const noexcept
			{ return (parent_->current_); }
			constexpr decltype(auto) current() noexcept requires ForwardRange<Base>
			{ return (current_); }
			constexpr decltype(auto) current() const noexcept requires ForwardRange<Base>
			{ return (current_); }
		public:
			using iterator_category = meta::if_c<models::ForwardRange<Base>,
				ranges::forward_iterator_tag, ranges::input_iterator_tag>;
			using difference_type = difference_type_t<iterator_t<Base>>;
			struct value_type;

			__outer_iterator() = default;
			constexpr __outer_iterator(Parent& parent)
			: parent_(&parent) {}
			constexpr __outer_iterator(Parent& parent, iterator_t<Base> current)
			requires ForwardRange<Base>
			: current_(current), parent_(&parent) {}

			constexpr __outer_iterator(__outer_iterator<!Const> i) requires Const &&
				ConvertibleTo<iterator_t<Rng>, iterator_t<Base>>
			: current_(i.current_), parent_(i.parent_) {}

			constexpr value_type operator*() const
			{ return value_type{*this}; }

			constexpr __outer_iterator& operator++()
			{
				auto& cur = current();
				auto const end = __stl2::end(parent_->base_);
				if (cur == end)
					return *this;
				auto const [pbegin, pend] = subrange{parent_->pattern_};
				do
				{
					auto [b,p] = mismatch(cur, end, pbegin, pend);
					if (p != pend) // The pattern didn't match
						continue;
					// skip the pattern:
					cur = next(b, (int)(models::ForwardRange<Rng> && pbegin == pend), end);
					break;
				} while (++cur != end);
				return *this;
			}
			constexpr void operator++(int)
			{ (void)++*this; }
			constexpr __outer_iterator operator++(int) requires ForwardRange<Base>
			{
				auto tmp = *this;
				++*this;
				return *this;
			}

			friend constexpr bool operator==(
				const __outer_iterator& x, const __outer_iterator& y)
			requires ForwardRange<Base>
			{ return x.current_ == y.current_; }

			friend constexpr bool operator!=(
				const __outer_iterator& x, const __outer_iterator& y)
			requires ForwardRange<Base>
			{ return !(x == y); }
		};

		template <class Rng, class Pattern>
		template <bool Const>
		struct split_view<Rng, Pattern>::__outer_sentinel {
		private:
			using Base = __maybe_const<Const, Rng>;
			using Parent = __maybe_const<Const, split_view>;
			sentinel_t<Base> end_;
		public:
			__outer_sentinel() = default;
			constexpr explicit __outer_sentinel(Parent& parent)
			: end_(__stl2::end(parent.base_)) {}

			friend constexpr bool operator==(const __outer_iterator<Const>& x, const __outer_sentinel& y)
			{ return x.current() == y.end_;	}
			friend constexpr bool operator==(const __outer_sentinel& x, const __outer_iterator<Const>& y)
			{ return y == x; }
			friend constexpr bool operator!=(const __outer_iterator<Const>& x, const __outer_sentinel& y)
			{ return !(x == y); }
			friend constexpr bool operator!=(const __outer_sentinel& x, const __outer_iterator<Const>& y)
			{ return !(y == x);	}
		};

		template <class Rng, class Pattern>
		template <bool Const>
		struct split_view<Rng, Pattern>::__outer_iterator<Const>::value_type {
		private:
			__outer_iterator i_ {};
		public:
			value_type() = default;
			constexpr explicit value_type(__outer_iterator i)
			: i_(i) {}

			using iterator = __inner_iterator<Const>;
			using sentinel = __inner_sentinel<Const>;
			using const_iterator = __inner_iterator<Const>;
			using const_sentinel = __inner_sentinel<Const>;

			constexpr iterator begin() const
			{ return iterator{i_}; }

			constexpr sentinel end() const
			{ return {}; }
		};

		template <class Rng, class Pattern>
		template <bool Const>
		struct split_view<Rng, Pattern>::__inner_iterator {
		private:
			using Base = __maybe_const<Const, Rng>;
			__outer_iterator<Const> i_ {};
			bool zero_ = false;
		public:
			using iterator_category = iterator_category_t<__outer_iterator<Const>>;
			using difference_type = difference_type_t<iterator_t<Base>>;
			using value_type = value_type_t<iterator_t<Base>>;

			__inner_iterator() = default;
			constexpr explicit __inner_iterator(__outer_iterator<Const> i)
			: i_(i) {}

			constexpr decltype(auto) operator*() const
			{ return *i_.current(); }

			constexpr __inner_iterator& operator++()
			{
				++i_.current();
				zero_ = true;
				return *this;
			}

			constexpr void operator++(int)
			{ (void)++*this; }

			constexpr __inner_iterator operator++(int) requires ForwardRange<Base>
			{
				auto tmp = *this;
				++*this;
				return tmp;
			}

			friend constexpr bool operator==(const __inner_iterator& x, const __inner_iterator& y)
			requires ForwardRange<Base>
			{ return x.i_.current_ == y.i_.current_; }
			friend constexpr bool operator!=(const __inner_iterator& x, const __inner_iterator& y)
			requires ForwardRange<Base>
			{ return !(x == y); }

			friend constexpr decltype(auto) iter_move(const __inner_iterator& i)
			noexcept(noexcept(ranges::iter_move(i.i_.current())))
			{ return ranges::iter_move(i.i_.current()); }
			friend constexpr void iter_swap(const __inner_iterator& x, const __inner_iterator& y)
			noexcept(noexcept(ranges::iter_swap(x.i_.current(), y.i_.current())))
			{ ranges::iter_swap(x.i_.current(), y.i_.current()); }
		};

		template <class Rng, class Pattern>
		template <bool Const>
		struct split_view<Rng, Pattern>::__inner_sentinel {
			friend constexpr bool operator==(
				const __inner_iterator<Const>& x, const __inner_sentinel&)
			{
				auto cur = x.i_.current();
				auto end = __stl2::end(x.i_.parent_->base_);
				if (cur == end)
					return true;
				auto [pcur, pend] = subrange{x.i_.parent_->pattern_};
				if (pcur == pend)
					return x.zero_;
				do {
					if (*cur != *pcur)
						return false;
					if (++pcur == pend)
						return true;
				} while (++cur != end);
				return false;
			}
			friend constexpr bool operator==(
				const __inner_sentinel& x, const __inner_iterator<Const>& y)
			{ return y == x; }
			friend constexpr bool operator!=(
				const __inner_iterator<Const>& x, const __inner_sentinel& y)
			{ return !(x == y); }
			friend constexpr bool operator!=(
				const __inner_sentinel& x, const __inner_iterator<Const>& y)
			{ return !(y == x); }
		};
	}

	namespace view {
		namespace __split {
			struct fn {
				ext::SplittableWithRange{Rng, Pattern}
				constexpr auto operator()(Rng&& rng, Pattern&& pattern) const
				{ return ext::split_view{std::forward<Rng>(rng), std::forward<Pattern>(pattern)}; }

				ext::SplittableWithElement{Rng}
				constexpr auto operator()(Rng&& rng, value_type_t<iterator_t<Rng>> value) const
				{ return ext::split_view{std::forward<Rng>(rng), std::move(value)}; }

				template <CopyConstructible T>
				constexpr auto operator()(T&& t) const
				{ return detail::view_closure{*this, std::forward<T>(t)}; }
			};
		} // namespace __split

		inline constexpr __split::fn split {};
	}
} STL2_CLOSE_NAMESPACE

#endif
