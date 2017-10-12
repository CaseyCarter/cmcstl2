// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

#include <type_traits>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template <InputRange Base>
		struct join_view_iterator_base {
			using iterator_category = __stl2::input_iterator_tag;
		};
		template <ForwardRange Base>
		requires std::is_reference_v<reference_t<iterator_t<Base>>> &&
			ForwardRange<reference_t<iterator_t<Base>>>
		struct join_view_iterator_base<Base> {
			using iterator_category = __stl2::forward_iterator_tag;
		};
		template <BidirectionalRange Base>
		requires std::is_reference_v<reference_t<iterator_t<Base>>> &&
			BidirectionalRange<reference_t<iterator_t<Base>>>
		struct join_view_iterator_base<Base> {
			using iterator_category = __stl2::bidirectional_iterator_tag;
		};

		template <InputRange InnerRng>
		struct join_view_base {
			ext::all_view<InnerRng> inner_ {};
		};
		template <InputRange InnerRng>
		requires std::is_reference_v<InnerRng>
		struct join_view_base<InnerRng> {};
	}

	namespace ext {
		template <InputRange Rng>
		requires View<Rng> && InputRange<reference_t<iterator_t<Rng>>> &&
			(std::is_reference_v<reference_t<iterator_t<Rng>>> ||
				View<value_type_t<iterator_t<Rng>>>)
		class join_view
		: view_interface<join_view<Rng>>
		, detail::join_view_base<reference_t<iterator_t<Rng>>> {
		private:
			Rng base_ {};
			using InnerRng = reference_t<iterator_t<Rng>>;
			template <bool Const>
			struct __iterator;
			template <bool Const>
			struct __sentinel;
		public:
			join_view() = default;
			constexpr explicit join_view(Rng base)
			: base_(std::move(base)) {}

			template <InputRange O>
			requires (std::is_lvalue_reference_v<O> || View<__f<O>>) &&
				Constructible<Rng, all_view<O>>
			constexpr explicit join_view(O&& o)
			: base_(view::all(std::forward<O>(o))) {}

			using iterator = __iterator<false>;
			using const_iterator = __iterator<true>;
			using sentinel = __sentinel<false>;
			using const_sentinel = __sentinel<true>;

			constexpr iterator begin()
			{ return {*this, __stl2::begin(base_)}; }

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstRng = const Rng>
			constexpr const_iterator begin() const
			requires InputRange<ConstRng> &&
				std::is_reference_v<reference_t<iterator_t<ConstRng>>>
			{ return {*this, __stl2::begin(base_)}; }

			constexpr sentinel end()
			{ return sentinel{*this}; }

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstRng = const Rng>
			constexpr const_sentinel end() const
			requires InputRange<ConstRng> &&
				std::is_reference_v<reference_t<iterator_t<ConstRng>>>
			{ return const_sentinel{*this}; }

			constexpr iterator end()
			requires ForwardRange<Rng> &&
				std::is_reference_v<InnerRng> &&
				ForwardRange<InnerRng> &&
				BoundedRange<Rng> && BoundedRange<InnerRng>
			{ return {*this, __stl2::end(base_)}; }

			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <class ConstRng = const Rng>
			constexpr const_iterator end() const
			requires ForwardRange<ConstRng> &&
				std::is_reference_v<reference_t<iterator_t<ConstRng>>> &&
				ForwardRange<reference_t<iterator_t<ConstRng>>> &&
				BoundedRange<ConstRng> &&
				BoundedRange<reference_t<iterator_t<ConstRng>>>
			{ return {*this, __stl2::end(base_)}; }
		};

		template <InputRange Rng>
		requires InputRange<reference_t<iterator_t<Rng>>> &&
			(std::is_reference_v<reference_t<iterator_t<Rng>>> ||
				View<value_type_t<iterator_t<Rng>>>)
		explicit join_view(Rng&&) -> join_view<all_view<Rng>>;

		template <class Rng>
		template <bool Const>
		struct join_view<Rng>::__iterator
		: detail::join_view_iterator_base<__maybe_const<Const, Rng>> {
		private:
			using Base = __maybe_const<Const, Rng>;
			using Parent = __maybe_const<Const, join_view>;

			iterator_t<Base> outer_ {};
			iterator_t<reference_t<iterator_t<Base>>> inner_ {};
			Parent* parent_ {};

			friend __iterator<!Const>;
			friend __sentinel<Const>;

			constexpr decltype(auto) update_cache_()
			{
				if constexpr (!std::is_reference_v<reference_t<iterator_t<Base>>>)
				{
					auto&& inner = *outer_;
					return (parent_->inner_ = view::all(inner));
				}
				else
					return *outer_;
			}

			constexpr decltype(auto) inner_rng_()
			{
				if constexpr (!std::is_reference_v<reference_t<iterator_t<Base>>>)
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
				if constexpr (std::is_reference_v<reference_t<iterator_t<Base>>>)
					inner_ = iterator_t<reference_t<iterator_t<Base>>>{};
			}
		public:
			using value_type =
				value_type_t<iterator_t<reference_t<iterator_t<Base>>>>;
			using difference_type =
				__stl2::common_type_t<
					difference_type_t<iterator_t<Base>>,
					difference_type_t<iterator_t<reference_t<iterator_t<Base>>>>>;

			__iterator() = default;

			constexpr __iterator(Parent& parent, iterator_t<Rng> outer)
			: outer_(outer), parent_(&parent)
			{ satisfy_(); }

			constexpr __iterator(__iterator<!Const> i) requires Const &&
				ConvertibleTo<iterator_t<Rng>, iterator_t<Base>> &&
				ConvertibleTo<
					iterator_t<InnerRng>,
					iterator_t<reference_t<iterator_t<Base>>>>
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
			requires std::is_reference_v<reference_t<iterator_t<Base>>> &&
				ForwardRange<Base> &&
				ForwardRange<reference_t<iterator_t<Base>>>
			{
				auto tmp = *this;
				++*this;
				return tmp;
			}

			constexpr __iterator& operator--()
			requires std::is_reference_v<reference_t<iterator_t<Base>>> &&
				BidirectionalRange<Base> &&
				BidirectionalRange<reference_t<iterator_t<Base>>>
			{
				if (outer_ == __stl2::end(parent_->base_))
					inner_ = __stl2::end(*--outer_);
				while (inner_ == __stl2::begin(*outer_))
					inner_ = __stl2::end(*--outer_);
				--inner_;
				return *this;
			}

			constexpr __iterator operator--(int)
			requires std::is_reference_v<reference_t<iterator_t<Base>>> &&
				BidirectionalRange<Base> &&
				BidirectionalRange<reference_t<iterator_t<Base>>>
			{
				auto tmp = *this;
				--*this;
				return tmp;
			}

			friend constexpr bool operator==(const __iterator& x, const __iterator& y)
			requires std::is_reference_v<reference_t<iterator_t<Base>>> &&
				EqualityComparable<iterator_t<Base>> &&
				EqualityComparable<iterator_t<reference_t<iterator_t<Base>>>>
			{ return x.outer_ == y.outer_ && x.inner_ == y.inner_; }

			friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
			requires std::is_reference_v<reference_t<iterator_t<Base>>> &&
				EqualityComparable<iterator_t<Base>> &&
				EqualityComparable<iterator_t<reference_t<iterator_t<Base>>>>
			{ return !(x == y); }

			friend constexpr decltype(auto) iter_move(const __iterator& i)
				noexcept(noexcept(__stl2::iter_move(i.inner_)))
			{ return __stl2::iter_move(i.inner_); }

			friend constexpr void iter_swap(const __iterator& x, const __iterator& y)
				noexcept(noexcept(__stl2::iter_swap(x.inner_, y.inner_)))
			{ __stl2::iter_swap(x.inner_, y.inner_); }
		};

		template <class Rng>
		template <bool Const>
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
	} // namespace ext

	namespace view
	{
		namespace __join
		{
			struct fn
			{
				template <InputRange Rng>
				requires (std::is_lvalue_reference_v<Rng> || View<__f<Rng>>) &&
					InputRange<reference_t<iterator_t<Rng>>> &&
					(std::is_reference_v<reference_t<iterator_t<Rng>>> ||
						View<value_type_t<iterator_t<Rng>>>)
				constexpr auto operator()(Rng&& rng) const
				{
					return ext::join_view{std::forward<Rng>(rng)};
				}

				template <InputRange Rng>
				requires (std::is_lvalue_reference_v<Rng> || View<__f<Rng>>) &&
					InputRange<reference_t<iterator_t<Rng>>> &&
					(std::is_reference_v<reference_t<iterator_t<Rng>>> ||
						View<value_type_t<iterator_t<Rng>>>)
				friend constexpr auto operator|(Rng&& rng, fn join)
				{
					return join(std::forward<Rng>(rng));
				}
			};
		}

		inline constexpr __join::fn join {};
	}
} STL2_CLOSE_NAMESPACE

#endif
