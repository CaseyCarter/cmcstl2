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
#ifndef STL2_VIEW_FILTER_HPP
#define STL2_VIEW_FILTER_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/optional.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template <InputRange R>
		struct begin_iterator_cache_ {
			mutable detail::non_propagating_cache<iterator_t<R>> value_;
			constexpr decltype(auto) get() const noexcept
			{ return (value_); }
		};
		template <InputRange R>
		requires InputRange<const R> && !Same<iterator_t<R>, iterator_t<const R>>
		struct begin_iterator_cache_<R> {
			mutable detail::non_propagating_cache<iterator_t<R>> value_;
			mutable detail::non_propagating_cache<iterator_t<const R>> cvalue_;
			constexpr decltype(auto) get() noexcept
			{ return (value_); }
			constexpr decltype(auto) get() const noexcept
			{ return (cvalue_); }
		};
	} // namespace detail

	namespace ext {
		template </*InputRange*/ class R, IndirectUnaryPredicate<iterator_t<R>> Pred>
		requires View<R>
		class filter_view : view_interface<filter_view<R, Pred>> {
		private:
			R base_;
			detail::semiregular_box<Pred> pred_;
			detail::begin_iterator_cache_<R> cache_;
			template <bool Const>
			class __iterator;
			template <bool Const>
			class __sentinel;
		public:
			filter_view() = default;

			constexpr filter_view(R base, Pred pred)
			: base_(std::move(base)), pred_(std::move(pred)) {}

			template <InputRange O>
			requires (std::is_lvalue_reference_v<O> || View<__f<O>>) &&
				Constructible<R, all_view<O>>
			constexpr filter_view(O&& o, Pred pred)
			: base_(view::all(std::forward<O>(o))), pred_(std::move(pred)) {}

			constexpr R base() const
			{ return base_; }

			using iterator = __iterator<false>;
			using sentinel = __sentinel<false>;
			using const_iterator = __iterator<true>;
			using const_sentinel = __sentinel<true>;

			constexpr iterator begin()
			{
				if(!cache_.get())
					cache_.get() = find_if(base_, std::ref(pred_.get()));
				return iterator{*this, *cache_.get()};
			}

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstR = const R>
			constexpr const_iterator begin() const
			requires InputRange<const R> &&
				IndirectUnaryPredicate<const Pred, iterator_t<ConstR>>
			{
				if(!cache_.get())
					cache_.get() = find_if(base_, std::ref(pred_.get()));
				return const_iterator{*this, *cache_.get()};
			}

			constexpr sentinel end()
			{ return sentinel{*this}; }

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstR = const R>
			constexpr const_sentinel end() const
			requires InputRange<const R> &&
				IndirectUnaryPredicate<const Pred, iterator_t<ConstR>>
			{ return const_sentinel{*this}; }

			constexpr iterator end() requires BoundedRange<R>
			{ return iterator{*this, __stl2::end(base_)}; }

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstR = const R>
			constexpr const_iterator end() const
			requires InputRange<const R> && BoundedRange<ConstR> &&
				IndirectUnaryPredicate<const Pred, iterator_t<ConstR>>
			{ return const_iterator{*this, __stl2::end(base_)}; }
		};

		template <class R, class Pred>
		template <bool Const>
		class filter_view<R, Pred>::__iterator {
		private:
			using Base = __maybe_const<Const, R>;
			using Parent = __maybe_const<Const, filter_view>;
			iterator_t<Base> current_ {};
			Parent* parent_ = nullptr;
			friend __sentinel<Const>;
			friend __iterator<!Const>;
		public:
			using iterator_category =
				meta::if_c<models::BidirectionalIterator<iterator_t<Base>> && !Const,
					__stl2::bidirectional_iterator_tag,
				meta::if_c<models::ForwardIterator<iterator_t<Base>> && !Const,
					__stl2::forward_iterator_tag,
					__stl2::input_iterator_tag>>;
			using value_type = value_type_t<iterator_t<Base>>;
			using difference_type = difference_type_t<iterator_t<Base>>;

			__iterator() = default;

			constexpr __iterator(Parent& parent, iterator_t<Base> current)
			: current_(current), parent_(&parent) {}

			constexpr __iterator(__iterator<!Const> i) requires Const &&
				ConvertibleTo<iterator_t<R>, iterator_t<Base>>
			: current_(i.current_), parent_(i.parent_) {}

			constexpr iterator_t<Base> base() const
			{ return current_; }

			constexpr reference_t<iterator_t<Base>> operator*() const
			{ return *current_; }

			constexpr __iterator& operator++()
			{
				current_ = find_if(
					++current_, __stl2::end(parent_->base_), std::ref(parent_->pred_.get()));
				return *this;
			}

			constexpr void operator++(int)
			{ (void)++*this; }

			constexpr __iterator operator++(int) requires ForwardRange<Base> && !Const
			{
				auto tmp = *this;
				++*this;
				return tmp;
			}

			constexpr __iterator& operator--() requires BidirectionalRange<Base> && !Const
			{
				do
					--current_;
				while(invoke(parent_->pred_.get(), *current_));
				return *this;
			}

			constexpr __iterator operator--(int) requires BidirectionalRange<Base> && !Const
			{
				auto tmp = *this;
				--*this;
				return tmp;
			}

			friend constexpr bool operator==(const __iterator& x, const __iterator& y)
			requires EqualityComparable<iterator_t<Base>>
			{ return x.current_ == y.current_; }

			friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
			requires EqualityComparable<iterator_t<Base>>
			{ return !(x == y); }

			friend constexpr rvalue_reference_t<iterator_t<Base>>
			iter_move(const __iterator& i)
			noexcept(noexcept(__stl2::iter_move(i.current_)))
			{ return __stl2::iter_move(i.current_); }

			friend constexpr void iter_swap(const __iterator& x, const __iterator& y)
			noexcept(noexcept(__stl2::iter_swap(x.current_, y.current_)))
			{ __stl2::iter_swap(x.current_, y.current_); }
		};

		template <class R, class Pred>
		template <bool Const>
		class filter_view<R, Pred>::__sentinel {
		private:
			using Base = __maybe_const<Const, R>;
			using Parent = __maybe_const<Const, filter_view>;
			sentinel_t<Base> end_;
		public:
			__sentinel() = default;
			explicit constexpr __sentinel(Parent& parent)
			: end_(__stl2::end(parent)) {}

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
		};

		template <InputRange R, CopyConstructible Pred>
		requires IndirectUnaryPredicate<Pred, iterator_t<R>> &&
			(std::is_lvalue_reference_v<R> || View<__f<R>>)
		filter_view(R&&, Pred) -> filter_view<all_view<R>, Pred>;
	} // namespace ext

	namespace view {
		namespace __filter {
			struct fn {
				template <InputRange R, IndirectUnaryPredicate<iterator_t<R>> Pred>
				requires std::is_lvalue_reference_v<R> || View<__f<R>>
				constexpr auto operator()(R&& rng, Pred pred) const
				{ return ext::filter_view<ext::all_view<R>, Pred>{std::forward<R>(rng), std::move(pred)}; }

				template <CopyConstructible Pred>
				constexpr auto operator()(Pred pred) const
				{ return detail::view_closure{*this, std::move(pred)}; }
			};
		}

		inline constexpr __filter::fn filter {};
	}
} STL2_CLOSE_NAMESPACE

#endif
