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

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/mismatch.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/single.hpp>

STL2_OPEN_NAMESPACE {
	template<class R>
	META_CONCEPT _TinyRange =
		SizedRange<R> &&
		requires {
			typename meta::detail::require_constant<std::remove_reference_t<R>::size()>;
		} &&
		std::remove_reference_t<R>::size() <= 1;

	template<InputRange Rng>
	struct __split_view_base {
		iterator_t<Rng> current_ {};
	};
	template<ForwardRange Rng>
	struct __split_view_base<Rng> {};

	template<InputRange Rng, ForwardRange Pattern>
	requires View<Rng> && View<Pattern> &&
		IndirectlyComparable<iterator_t<Rng>, iterator_t<Pattern>, equal_to> &&
		(ForwardRange<Rng> || _TinyRange<Pattern>)
	struct split_view : private __split_view_base<Rng> {
	private:
		template<bool Const> struct __outer_iterator;
		template<bool Const> struct __inner_iterator;

		Rng base_ {};
		Pattern pattern_ {};
	public:
		split_view() = default;
		constexpr split_view(Rng base, Pattern pattern)
		: base_(std::move(base))
		, pattern_(std::move(pattern)) {}

		constexpr split_view(Rng base, iter_value_t<iterator_t<Rng>> e)
		requires Constructible<Pattern, single_view<iter_value_t<iterator_t<Rng>>>>
		: base_(std::move(base))
		, pattern_(single_view{std::move(e)}) {}

		constexpr auto begin() {
			if constexpr (ForwardRange<Rng>) {
				return __outer_iterator<ext::SimpleView<Rng>>{*this, __stl2::begin(base_)};
			} else {
				this->current_ = __stl2::begin(base_);
				return __outer_iterator<false>{*this};
			}
		}

		constexpr auto begin() const
		requires ForwardRange<Rng> && ForwardRange<const Rng>
		{ return __outer_iterator<true>{*this, __stl2::begin(base_)}; }

		constexpr auto end() requires ForwardRange<Rng> && CommonRange<Rng>
		{ return __outer_iterator<ext::SimpleView<Rng>>{*this, __stl2::end(base_)}; }

		constexpr auto end() const {
			constexpr bool can_bound = ForwardRange<Rng> &&
				ForwardRange<const Rng> && CommonRange<const Rng>;
			if constexpr (can_bound) {
				return __outer_iterator<true>{*this, __stl2::end(base_)};
			} else {
				return default_sentinel{};
			}
		}
	};

	template<class Rng, class Pattern>
	split_view(Rng&&, Pattern&&) -> split_view<all_view<Rng>, all_view<Pattern>>;

	template<InputRange Rng>
	split_view(Rng&&, iter_value_t<iterator_t<Rng>>)
		-> split_view<all_view<Rng>, single_view<iter_value_t<iterator_t<Rng>>>>;

	template<class, bool>
	struct __split_view_outer_base {};
	template<ForwardRange Rng, bool Const>
	struct __split_view_outer_base<Rng, Const> {
		iterator_t<__maybe_const<Const, Rng>> current_ {};
	};

	template<InputRange Rng, ForwardRange Pattern>
	requires View<Rng> && View<Pattern> &&
		IndirectlyComparable<iterator_t<Rng>, iterator_t<Pattern>, equal_to> &&
		(ForwardRange<Rng> || _TinyRange<Pattern>)
	template<bool Const>
	struct split_view<Rng, Pattern>::__outer_iterator
	: private __split_view_outer_base<Rng, Const> {
	private:
		friend __outer_iterator<!Const>;
		friend __inner_iterator<Const>;

		static_assert(ForwardRange<Rng> || !Const);

		using Parent = __maybe_const<Const, split_view>;
		using Base = __maybe_const<Const, Rng>;

		Parent* parent_ = nullptr;

		constexpr iterator_t<Base>& current() noexcept
		requires ForwardRange<Base>
		{ return this->current_; }
		constexpr auto& current() const noexcept {
			if constexpr (ForwardRange<Base>) {
				return this->current_;
			} else {
				return parent_->current_;
			}
		}

		constexpr bool at_end() const {
			return current() == __stl2::end(parent_->base_);
		}
	public:
		using iterator_category = __cond<ForwardRange<Base>,
			__stl2::forward_iterator_tag, __stl2::input_iterator_tag>;
		using difference_type = iter_difference_t<iterator_t<Base>>;
		struct value_type;

		__outer_iterator() = default;

		constexpr explicit  __outer_iterator(Parent& parent)
		requires (!ForwardRange<Base>)
		: parent_(std::addressof(parent)) {}

		constexpr __outer_iterator(Parent& parent, iterator_t<Base> current)
		requires ForwardRange<Base>
		: __split_view_outer_base<Rng, Const>{std::move(current)}
		, parent_(std::addressof(parent)) {}

		constexpr __outer_iterator(__outer_iterator<!Const> i)
		requires Const && ConvertibleTo<iterator_t<Rng>, iterator_t<Base>>
		: __split_view_outer_base<Rng, Const>{i.current_}
		, parent_(i.parent_) {}

		constexpr value_type operator*() const
		{ return value_type{*this}; }

		constexpr __outer_iterator& operator++() {
			auto& cur = current();
			const auto end = __stl2::end(parent_->base_);
			if (cur == end) return *this;
			const auto [pbegin, pend] = subrange{parent_->pattern_};
			if (pbegin == pend) ++cur;
			else {
				do {
					const auto [b, p] = mismatch(cur, end, pbegin, pend);
					if (p == pend) {
						// The pattern matches, skip it
						cur = b;
						break;
					}
				} while (++cur != end);
			}
			return *this;
		}

		constexpr decltype(auto) operator++(int) {
			if constexpr (ForwardRange<Base>) {
				auto tmp = *this;
				++*this;
				return tmp;
			} else {
				++*this;
			}
		}

		friend constexpr bool operator==(const __outer_iterator& x, const __outer_iterator& y)
		requires ForwardRange<Base>
		{ return x.current_ == y.current_; }

		friend constexpr bool operator!=(const __outer_iterator& x, const __outer_iterator& y)
		requires ForwardRange<Base>
		{ return !(x == y); }

		friend constexpr bool operator==(const __outer_iterator& x, default_sentinel)
		{ return x.at_end(); }
		friend constexpr bool operator==(default_sentinel, const __outer_iterator& y)
		{ return y.at_end(); }
		friend constexpr bool operator!=(const __outer_iterator& x, default_sentinel)
		{ return !x.at_end(); }
		friend constexpr bool operator!=(default_sentinel, const __outer_iterator& y)
		{ return !y.at_end(); }
	};

	template<InputRange Rng, ForwardRange Pattern>
	requires View<Rng> && View<Pattern> &&
		IndirectlyComparable<iterator_t<Rng>, iterator_t<Pattern>, equal_to> &&
		(ForwardRange<Rng> || _TinyRange<Pattern>)
	template<bool Const>
	struct split_view<Rng, Pattern>::__outer_iterator<Const>::value_type {
	private:
		__outer_iterator i_ {};
	public:
		value_type() = default;
		constexpr explicit value_type(__outer_iterator i)
		: i_(i) {}

		constexpr __inner_iterator<Const> begin() const
		{ return __inner_iterator<Const>{i_}; }

		constexpr default_sentinel end() const
		{ return default_sentinel{}; }
	};

	template<InputRange Rng, ForwardRange Pattern>
	requires View<Rng> && View<Pattern> &&
		IndirectlyComparable<iterator_t<Rng>, iterator_t<Pattern>, equal_to> &&
		(ForwardRange<Rng> || _TinyRange<Pattern>)
	template<bool Const>
	struct split_view<Rng, Pattern>::__inner_iterator {
	private:
		using Base = __maybe_const<Const, Rng>;

		static_assert(ForwardRange<Rng> || !Const);

		__outer_iterator<Const> i_ {};
		bool zero_ = false;

		constexpr bool at_end() const {
			auto cur = i_.current();
			auto end = __stl2::end(i_.parent_->base_);
			if (cur == end) return true;
			auto [pcur, pend] = subrange{i_.parent_->pattern_};
			if (pcur == pend) return zero_;
			do {
				if (*cur != *pcur) return false;
				if (++pcur == pend) return true;
			} while (++cur != end);
			return false;
		}
		constexpr auto& current() const noexcept { return i_.current(); }
	public:
		using iterator_category = iterator_category_t<__outer_iterator<Const>>;
		using difference_type = iter_difference_t<iterator_t<Base>>;
		using value_type = iter_value_t<iterator_t<Base>>;

		__inner_iterator() = default;
		constexpr explicit __inner_iterator(__outer_iterator<Const> i)
		: i_{i} {}

		constexpr decltype(auto) operator*() const
		{ return *i_.current(); }

		constexpr __inner_iterator& operator++() {
			zero_ = true;
			if constexpr (!ForwardRange<Base>) {
				if constexpr (Pattern::size() == 0) {
					return *this;
				}
			}
			++i_.current();
			return *this;
		}

		constexpr decltype(auto) operator++(int) {
			if constexpr (ForwardRange<Base>) {
				auto tmp = *this;
				++*this;
				return tmp;
			} else
				++*this;
		}

		friend constexpr bool operator==(const __inner_iterator& x, const __inner_iterator& y)
		requires ForwardRange<Base>
		{ return x.i_ == y.i_; }
		friend constexpr bool operator!=(const __inner_iterator& x, const __inner_iterator& y)
		requires ForwardRange<Base>
		{ return !(x == y); }

		friend constexpr bool operator==(const __inner_iterator& x, default_sentinel)
		{ return x.at_end(); }
		friend constexpr bool operator==(default_sentinel, const __inner_iterator& y)
		{ return y.at_end(); }
		friend constexpr bool operator!=(const __inner_iterator& x, default_sentinel)
		{ return !x.at_end(); }
		friend constexpr bool operator!=(default_sentinel, const __inner_iterator& y)
		{ return !y.at_end(); }

		friend constexpr decltype(auto) iter_move(const __inner_iterator& i)
		STL2_NOEXCEPT_RETURN(
			__stl2::iter_move(i.current())
		)

		friend constexpr void iter_swap(const __inner_iterator& x, const __inner_iterator& y)
		noexcept(noexcept(__stl2::iter_swap(x.current(), y.current())))
		requires IndirectlySwappable<iterator_t<Base>>
		{ __stl2::iter_swap(x.current(), y.current()); }
	};

	namespace view {
		struct __split_fn {
			template<class E, class F>
			constexpr auto operator()(E&& e, F&& f) const
#if STL2_WORKAROUND_CLANGC_50
			requires requires(E&& e, F&& f) {
				split_view{static_cast<E&&>(e), static_cast<F&&>(f)};
			} {
				return split_view{static_cast<E&&>(e), static_cast<F&&>(f)};
			}
#else // ^^^ workaround / no workaround vvv
			STL2_REQUIRES_RETURN(
				split_view{static_cast<E&&>(e), static_cast<F&&>(f)}
			)
#endif // STL2_WORKAROUND_CLANGC_50

			template<CopyConstructible T>
			constexpr auto operator()(T&& t) const
			{ return detail::view_closure{*this, std::forward<T>(t)}; }
		};

		inline constexpr __split_fn split {};
	}
} STL2_CLOSE_NAMESPACE

#endif
