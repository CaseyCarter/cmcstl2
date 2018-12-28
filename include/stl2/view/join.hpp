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
#ifndef STL2_VIEW_JOIN_HPP
#define STL2_VIEW_JOIN_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template<class R>
		META_CONCEPT _GLvalueRange = Range<R> &&
			std::is_reference_v<iter_reference_t<iterator_t<R>>>;

		template<InputRange Base>
		struct join_view_iterator_base {
			using iterator_category = __stl2::input_iterator_tag;
		};
		template<ForwardRange Base>
		requires _GLvalueRange<Base> &&
			ForwardRange<iter_reference_t<iterator_t<Base>>>
		struct join_view_iterator_base<Base> {
			using iterator_category = __stl2::forward_iterator_tag;
		};
		template<BidirectionalRange Base>
		requires _GLvalueRange<Base> &&
			BidirectionalRange<iter_reference_t<iterator_t<Base>>>
		struct join_view_iterator_base<Base> {
			using iterator_category = __stl2::bidirectional_iterator_tag;
		};

		template<InputRange InnerRng>
		struct join_view_base {
			all_view<InnerRng> inner_ {};
		};
		template<InputRange InnerRng>
		requires std::is_reference_v<InnerRng>
		struct join_view_base<InnerRng> {};
	}

	template<InputRange Rng>
	requires View<Rng> && InputRange<iter_reference_t<iterator_t<Rng>>> &&
		(detail::_GLvalueRange<Rng> || View<iter_value_t<iterator_t<Rng>>>)
	class join_view
	: public view_interface<join_view<Rng>>
	, detail::join_view_base<iter_reference_t<iterator_t<Rng>>> {
	private:
		Rng base_ {};
		using InnerRng = iter_reference_t<iterator_t<Rng>>;
		template<bool Const>
		struct __iterator;
		template<bool Const>
		struct __sentinel;
	public:
		join_view() = default;
		constexpr explicit join_view(Rng base)
		: base_(std::move(base)) {}

		using iterator = __iterator<ext::SimpleView<Rng>>;
		using sentinel = __sentinel<ext::SimpleView<Rng>>;
		using const_iterator = __iterator<true>;
		using const_sentinel = __sentinel<true>;

		constexpr iterator begin()
		{ return {*this, __stl2::begin(base_)}; }

		// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		template<class ConstRng = const Rng>
		constexpr const_iterator begin() const
		requires InputRange<ConstRng> &&
			std::is_reference_v<iter_reference_t<iterator_t<ConstRng>>>
		{ return {*this, __stl2::begin(base_)}; }

		constexpr auto end()
		{
			if constexpr (ForwardRange<Rng> && std::is_reference_v<InnerRng> &&
			              ForwardRange<InnerRng> && CommonRange<Rng> &&
			              CommonRange<InnerRng>) {
				return iterator{*this, __stl2::end(base_)};
			} else
				return sentinel{*this};
		}

		// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		template<class ConstRng = const Rng>
		constexpr auto end() const
		requires InputRange<ConstRng> &&
			std::is_reference_v<iter_reference_t<iterator_t<ConstRng>>>
		{
			if constexpr (ForwardRange<ConstRng> &&
			ForwardRange<iter_reference_t<iterator_t<ConstRng>>> &&
			CommonRange<ConstRng> &&
			CommonRange<iter_reference_t<iterator_t<ConstRng>>>) {
				return const_iterator{*this, __stl2::end(base_)};
			} else
				return const_sentinel{*this};
		}
	};

	template<InputRange Rng>
	requires InputRange<iter_reference_t<iterator_t<Rng>>> &&
		(detail::_GLvalueRange<Rng> || View<iter_value_t<iterator_t<Rng>>>)
	explicit join_view(Rng&&) -> join_view<all_view<Rng>>;

	template<InputRange Rng>
	requires View<Rng> && InputRange<iter_reference_t<iterator_t<Rng>>> &&
		(detail::_GLvalueRange<Rng> || View<iter_value_t<iterator_t<Rng>>>)
	template<bool Const>
	struct join_view<Rng>::__iterator
	: detail::join_view_iterator_base<__maybe_const<Const, Rng>> {
	private:
		using Base = __maybe_const<Const, Rng>;
		using Parent = __maybe_const<Const, join_view>;

		iterator_t<Base> outer_ {};
		iterator_t<iter_reference_t<iterator_t<Base>>> inner_ {};
		Parent* parent_ {};

		friend __iterator<!Const>;
		friend __sentinel<Const>;

		constexpr decltype(auto) update_cache_()
		{
			if constexpr (!detail::_GLvalueRange<Base>)
			{
				auto&& inner = *outer_;
				return (parent_->inner_ = view::all(inner));
			}
			else
				return *outer_;
		}

		constexpr decltype(auto) inner_rng_()
		{
			if constexpr (!detail::_GLvalueRange<Base>)
				return (parent_->inner_);
			else
				return *outer_;
		}

		constexpr void satisfy_()
		{
			for (; outer_ != __stl2::end(parent_->base_); ++outer_) {
				auto&& inner = update_cache_();
				inner_ = __stl2::begin(inner);
				if (inner_ != __stl2::end(inner))
					return;
			}
			// needed for symmetric iterator comparison:
			if constexpr (detail::_GLvalueRange<Base>)
				inner_ = iterator_t<iter_reference_t<iterator_t<Base>>>{};
		}
	public:
		using value_type =
			iter_value_t<iterator_t<iter_reference_t<iterator_t<Base>>>>;
		using difference_type =
			__stl2::common_type_t<
				iter_difference_t<iterator_t<Base>>,
				iter_difference_t<iterator_t<iter_reference_t<iterator_t<Base>>>>>;

		__iterator() = default;

		constexpr __iterator(Parent& parent, iterator_t<Rng> outer)
		: outer_(outer), parent_(&parent)
		{ satisfy_(); }

		constexpr __iterator(__iterator<!Const> i) requires Const &&
			ConvertibleTo<iterator_t<Rng>, iterator_t<Base>> &&
			ConvertibleTo<
				iterator_t<InnerRng>,
				iterator_t<iter_reference_t<iterator_t<Base>>>>
		: outer_(i.outer_), inner_(i.inner_), parent_(i.parent) {}

		constexpr decltype(auto) operator*() const
		{ return *inner_; }

		constexpr __iterator& operator++()
		{
			if (++inner_ == __stl2::end(inner_rng_()))
			{
				++outer_;
				satisfy_();
			}
			return *this;
		}

		constexpr void operator++(int)
		{ ++*this; }

		constexpr __iterator operator++(int)
		requires std::is_reference_v<iter_reference_t<iterator_t<Base>>> &&
			ForwardRange<Base> &&
			ForwardRange<iter_reference_t<iterator_t<Base>>>
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		constexpr __iterator& operator--()
		requires std::is_reference_v<iter_reference_t<iterator_t<Base>>> &&
			BidirectionalRange<Base> &&
			BidirectionalRange<iter_reference_t<iterator_t<Base>>>
		{
			if (outer_ == __stl2::end(parent_->base_))
				inner_ = __stl2::end(*--outer_);
			while (inner_ == __stl2::begin(*outer_))
				inner_ = __stl2::end(*--outer_);
			--inner_;
			return *this;
		}

		constexpr __iterator operator--(int)
		requires std::is_reference_v<iter_reference_t<iterator_t<Base>>> &&
			BidirectionalRange<Base> &&
			BidirectionalRange<iter_reference_t<iterator_t<Base>>>
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		friend constexpr bool operator==(const __iterator& x, const __iterator& y)
		requires std::is_reference_v<iter_reference_t<iterator_t<Base>>> &&
			EqualityComparable<iterator_t<Base>> &&
			EqualityComparable<iterator_t<iter_reference_t<iterator_t<Base>>>>
		{ return x.outer_ == y.outer_ && x.inner_ == y.inner_; }

		friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
		requires std::is_reference_v<iter_reference_t<iterator_t<Base>>> &&
			EqualityComparable<iterator_t<Base>> &&
			EqualityComparable<iterator_t<iter_reference_t<iterator_t<Base>>>>
		{ return !(x == y); }

		friend constexpr decltype(auto) iter_move(const __iterator& i)
			noexcept(noexcept(__stl2::iter_move(i.inner_)))
		{ return __stl2::iter_move(i.inner_); }

		friend constexpr void iter_swap(const __iterator& x, const __iterator& y)
			noexcept(noexcept(__stl2::iter_swap(x.inner_, y.inner_)))
		{ __stl2::iter_swap(x.inner_, y.inner_); }
	};

	template<InputRange Rng>
	requires View<Rng> && InputRange<iter_reference_t<iterator_t<Rng>>> &&
		(detail::_GLvalueRange<Rng> || View<iter_value_t<iterator_t<Rng>>>)
	template<bool Const>
	struct join_view<Rng>::__sentinel {
	private:
		using Base = __maybe_const<Const, Rng>;
		using Parent = __maybe_const<Const, join_view>;
		sentinel_t<Base> end_ {};
		friend __sentinel<!Const>;
	public:
		__sentinel() = default;

		constexpr explicit __sentinel(Parent& parent)
		: end_(__stl2::end(parent.base_)) {}

		constexpr __sentinel(__sentinel<!Const> s) requires Const &&
			ConvertibleTo<sentinel_t<Rng>, sentinel_t<Base>>
		: end_(s.end_) {}

		friend constexpr bool operator==(const __iterator<Const>& x, const __sentinel& y)
		{ return x.outer_ == y.end_; }

		friend constexpr bool operator==(const __sentinel& x, const __iterator<Const>& y)
		{ return y == x; }

		friend constexpr bool operator!=(const __iterator<Const>& x, const __sentinel& y)
		{ return !(x == y); }

		friend constexpr bool operator!=(const __sentinel& x, const __iterator<Const>& y)
		{ return !(y == x); }
	};

	namespace view {
		struct __join_fn : detail::__pipeable<__join_fn> {
			template<class Rng>
			constexpr auto operator()(Rng&& rng) const
			STL2_REQUIRES_RETURN(
				join_view{std::forward<Rng>(rng)}
			)
		};

		inline constexpr __join_fn join {};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif
