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
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/algorithm/search.hpp>
#include <stl2/detail/concepts/algorithm.hpp>
#include <stl2/detail/concepts/object.hpp>
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
			SizedRange<R> && std::remove_reference_t<R>::size() <= 1;

		template <InputRange Rng>
		struct __split_view_base {
			iterator_t<Rng> current_ {};
		};
		template <ForwardRange Rng>
		struct __split_view_base<Rng> {};

		template <InputRange Rng, ForwardRange Pattern>
		requires View<Rng> && View<Pattern> &&
			IndirectlyComparable<iterator_t<Rng>, iterator_t<Pattern>, equal_to<>> &&
			(ForwardRange<Rng> || _TinyRange<Pattern>)
		struct split_view : private __split_view_base<Rng> {
		private:
			template <bool Const> struct __outer_iterator;
			struct __inner_iterator;

			Rng base_ {};
			Pattern pattern_ {};
		public:
			split_view() = default;
			constexpr split_view(Rng base, Pattern pattern)
			: base_(std::move(base))
			, pattern_(std::move(pattern)) {}

			template <InputRange O, ForwardRange P>
			requires
				_ConstructibleFromRange<Rng, O> &&
				_ConstructibleFromRange<Pattern, P>
			constexpr split_view(O&& o, P&& p)
			: base_(view::all(std::forward<O>(o)))
			, pattern_(view::all(std::forward<P>(p))) {}

			template <InputRange O>
			requires
				_ConstructibleFromRange<Rng, O> &&
				Constructible<Pattern, single_view<iter_value_t<iterator_t<O>>>>
			constexpr split_view(O&& o, iter_value_t<iterator_t<O>> e)
			: base_(view::all(std::forward<O>(o)))
			, pattern_(single_view{std::move(e)}) {}

			constexpr auto begin() {
				if constexpr (ForwardRange<Rng>) {
					return __outer_iterator<SimpleView<Rng>>{*this, __stl2::begin(base_)};
				} else {
					this->current_ = __stl2::begin(base_);
					return __outer_iterator<false>{*this};
				}
			}

			constexpr auto begin() const
			requires ForwardRange<Rng> && ForwardRange<const Rng>
			{ return __outer_iterator<true>{*this, __stl2::begin(base_)}; }

			constexpr auto end() requires ForwardRange<Rng> && CommonRange<Rng>
			{ return __outer_iterator<SimpleView<Rng>>{*this, __stl2::end(base_)}; }

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

		template <class Rng, class Pattern>
		split_view(Rng&&, Pattern&&) -> split_view<all_view<Rng>, all_view<Pattern>>;

		template <InputRange Rng>
		split_view(Rng&&, iter_value_t<iterator_t<Rng>>)
			-> split_view<all_view<Rng>, single_view<iter_value_t<iterator_t<Rng>>>>;

		template <class, bool>
		struct __split_view_outer_base {};
		template <ForwardRange Rng, bool Const>
		struct __split_view_outer_base<Rng, Const> {
			iterator_t<__maybe_const<Const, Rng>> current_ {};
			subrange<iterator_t<__maybe_const<Const, Rng>>> delimiter_ {};
		};

		template <class Rng, class Pattern>
		template <bool Const>
		struct split_view<Rng, Pattern>::__outer_iterator
		: private __split_view_outer_base<Rng, Const> {
		private:
			friend __outer_iterator<!Const>;
			friend __inner_iterator;

			static_assert(ForwardRange<Rng> || !Const);

			using Parent = __maybe_const<Const, split_view>;
			using Base = __maybe_const<Const, Rng>;

			// To avoid a dependency cycle, we must delay checking the
			// constraints of subrange<__inner_iterator, default_sentinel>.
			struct __value_type
			: public subrange<__inner_iterator, default_sentinel> {
				using __value_type::subrange::subrange;
			};

			Parent* parent_ = nullptr;

			constexpr iterator_t<Base>& current() const noexcept
			{ return parent_->current_; }
			constexpr iterator_t<Base>& current() noexcept
				requires ForwardRange<Base>
			{ return this->current_; }
			constexpr const iterator_t<Base>& current() const noexcept
				requires ForwardRange<Base>
			{ return this->current_; }
		public:
			using iterator_category = meta::if_c<ForwardRange<Base>,
				__stl2::forward_iterator_tag, __stl2::input_iterator_tag>;
			using difference_type = iter_difference_t<iterator_t<Base>>;
			using value_type = meta::if_c<ForwardRange<Base>,
				subrange<iterator_t<Base>>, __value_type>;

			__outer_iterator() = default;

			constexpr explicit  __outer_iterator(Parent& parent)
			requires !ForwardRange<Base>
			: parent_(detail::addressof(parent)) {}

			constexpr __outer_iterator(Parent& parent, iterator_t<Base> current)
			requires ForwardRange<Base>
			: __split_view_outer_base<Rng, Const>{std::move(current)}
			, parent_(detail::addressof(parent)) {
				this->delimiter_ = __stl2::search(
					subrange{this->current_, __stl2::end(parent_->base_)},
					parent_->pattern_);
			}

			constexpr __outer_iterator(__outer_iterator<!Const> i)
			requires Const && ConvertibleTo<iterator_t<Rng>, iterator_t<const Rng>>
			: __split_view_outer_base<Rng, Const>{i.current_, i.delimiter_}
			, parent_(i.parent_) {}

			constexpr value_type operator*() const {
				if constexpr (ForwardRange<Base>) {
					auto end = this->delimiter_.begin();
					if (__stl2::empty(parent_->pattern_)) ++end;
					return subrange<iterator_t<Base>>{current(), std::move(end)};
				} else {
					return __value_type{__inner_iterator{parent_}, {}};
				}
			}

			constexpr __outer_iterator& operator++() {
				auto& cur = current();
				const auto end = __stl2::end(parent_->base_);
				STL2_EXPECT(cur != end ||
					(!ForwardRange<Base> && __stl2::distance(parent_->pattern_) == 1));
				if constexpr (ForwardRange<Base>) {
					const auto [pbegin, pend] = subrange{parent_->pattern_};
					if (pbegin == pend) ++cur;
					else cur = this->delimiter_.end();
					this->delimiter_ = __stl2::search(cur, end, pbegin, pend);
				} else {
					if constexpr (Pattern::size() == 1) {
						if (cur == end) return *this;
						auto pred = [pbegin = __stl2::begin(parent_->pattern_)](const auto& e) {
							return e == *pbegin;
						};
						cur = __stl2::find_if(cur, end, pred);
					}
					++cur;
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
			{ return x.current() == __stl2::end(x.parent_->base_); }
			friend constexpr bool operator==(default_sentinel x, const __outer_iterator& y)
			{ return y == x; }
			friend constexpr bool operator!=(const __outer_iterator& x, default_sentinel y)
			{ return !(x == y); }
			friend constexpr bool operator!=(default_sentinel x, const __outer_iterator& y)
			{ return !(y == x);	}
		};

		template <class Pattern>
		struct __split_view_inner_base {};
		template <class Pattern>
		requires Pattern::size() == 0
		struct __split_view_inner_base<Pattern> {
			bool at_end_ = false;
		};

		template <class Rng, class Pattern>
		struct split_view<Rng, Pattern>::__inner_iterator
		: private __split_view_inner_base<Pattern> {
		private:
			static_assert(!ForwardRange<Rng>);
			static_assert(_TinyRange<Pattern>);

			using Parent = split_view<Rng, Pattern>;
			Parent* parent_ = nullptr;
		public:
			using iterator_category = __stl2::input_iterator_tag;
			using difference_type = iter_difference_t<iterator_t<Rng>>;
			using value_type = iter_value_t<iterator_t<Rng>>;

			__inner_iterator() = default;
			constexpr explicit __inner_iterator(Parent* parent)
			: parent_{parent} {}

			constexpr decltype(auto) operator*() const
			{ return *parent_->current_; }

			constexpr __inner_iterator& operator++() {
				if constexpr (Pattern::size() == 0) this->at_end_ = true;
				else ++parent_->current_;
				return *this;
			}
			constexpr void operator++(int)
			{ ++*this; }

			friend constexpr bool operator==(const __inner_iterator& x, default_sentinel) {
				if constexpr (Pattern::size() == 0) return x.at_end_;
				else {
					static_assert(Pattern::size() == 1);
					const auto& cur = x.parent_->current_;
					if (cur == __stl2::end(x.parent_->base_)) return true;
					return *cur == *__stl2::begin(x.parent_->pattern_);
				}
			}
			friend constexpr bool operator==(default_sentinel x, const __inner_iterator& y)
			{ return y == x; }
			friend constexpr bool operator!=(const __inner_iterator& x, default_sentinel y)
			{ return !(x == y); }
			friend constexpr bool operator!=(default_sentinel x, const __inner_iterator& y)
			{ return !(y == x); }

			friend constexpr decltype(auto) iter_move(const __inner_iterator& i)
			STL2_NOEXCEPT_RETURN(
				__stl2::iter_move(i.parent_->current_)
			)
			friend constexpr void iter_swap(const __inner_iterator& x, const __inner_iterator& y)
			noexcept(noexcept(__stl2::iter_swap(x.parent_->current_, y.parent_->current_)))
			requires IndirectlySwappable<iterator_t<Rng>>
			{ __stl2::iter_swap(x.parent_->current_, y.parent_->current_); }
		};
	} // namespace ext

	namespace view {
		struct __split_fn {
			template <class E, class F>
			requires requires(E&& e, F&& f) {
				ext::split_view{static_cast<E&&>(e), static_cast<F&&>(f)};
			}
			constexpr auto operator()(E&& e, F&& f) const
			STL2_NOEXCEPT_RETURN(
				ext::split_view{static_cast<E&&>(e), static_cast<F&&>(f)}
			)

			template <CopyConstructible T>
			constexpr auto operator()(T&& t) const
			{ return detail::view_closure{*this, std::forward<T>(t)}; }
		};

		inline constexpr __split_fn split {};
	}
} STL2_CLOSE_NAMESPACE

#endif
