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
#include <stl2/detail/closure.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

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
			// Implements the PR for https://github.com/ericniebler/stl2#604
			mutable all_view<InnerRng> inner_ = all_view<InnerRng>();
		};
		template<InputRange InnerRng>
		requires std::is_reference_v<InnerRng>
		struct join_view_base<InnerRng> {};
	}

	template<InputRange V>
	requires View<V> && InputRange<iter_reference_t<iterator_t<V>>> &&
		(detail::_GLvalueRange<V> || View<iter_value_t<iterator_t<V>>>) // TODO: file LWG issue
	class STL2_EMPTY_BASES join_view
	: public view_interface<join_view<V>>
	, detail::join_view_base<iter_reference_t<iterator_t<V>>> {
	private:
		using InnerRng = iter_reference_t<iterator_t<V>>;
		template<bool> struct __iterator;
		template<bool> struct __sentinel;

		V base_ = V();

	public:
		join_view() = default;
		constexpr explicit join_view(V base)
		: base_(std::move(base)) {}

		constexpr auto begin() {
			return __iterator<ext::SimpleView<V>>{*this, __stl2::begin(base_)};
		}

		// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		template<class ConstV = const V>
		constexpr auto begin() const
		requires InputRange<ConstV> && detail::_GLvalueRange<ConstV>
		{ return __iterator<true>{*this, __stl2::begin(base_)}; }

		constexpr auto end() {
			if constexpr (ForwardRange<V> && detail::_GLvalueRange<V> &&
			              ForwardRange<InnerRng> && CommonRange<V> &&
			              CommonRange<InnerRng>) {
				return __iterator<ext::SimpleView<V>>{*this, __stl2::end(base_)};
			} else {
				return __sentinel<ext::SimpleView<V>>{*this};
			}
		}

		// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		template<class ConstV = const V>
		constexpr auto end() const
		requires InputRange<ConstV> &&
			std::is_reference_v<iter_reference_t<iterator_t<ConstV>>>
		{
			if constexpr (ForwardRange<ConstV> &&
			              // std::is_reference_v<iter_reference_t<iterator_t<ConstV>>> && // TODO: file LWG issue
			              ForwardRange<iter_reference_t<iterator_t<ConstV>>> &&
			              CommonRange<ConstV> &&
			              CommonRange<iter_reference_t<iterator_t<ConstV>>>) {
				return __iterator<true>{*this, __stl2::end(base_)};
			} else {
				return __sentinel<true>{*this};
			}
		}
	};

	template<class R>
	explicit join_view(R&&) -> join_view<all_view<R>>;

	template<InputRange V> // TODO: file LWG issue
	requires View<V> && InputRange<iter_reference_t<iterator_t<V>>> &&
		(detail::_GLvalueRange<V> || View<iter_value_t<iterator_t<V>>>)
	template<bool Const>
	struct join_view<V>::__iterator
	: detail::join_view_iterator_base<__maybe_const<Const, V>> {
	private:
		using Base = __maybe_const<Const, V>;
		using Parent = __maybe_const<Const, join_view>;

		static constexpr bool ref_is_glvalue = detail::_GLvalueRange<Base>;

		iterator_t<Base> outer_ = iterator_t<Base>();
		iterator_t<iter_reference_t<iterator_t<Base>>> inner_ =
			iterator_t<iter_reference_t<iterator_t<Base>>>();
		Parent* parent_ = nullptr;

		friend __iterator<true>;
		friend __sentinel<Const>;

		constexpr void satisfy_() {
			auto update_inner_range =
				[this](iter_reference_t<iterator_t<Base>> x) -> auto& {
				if constexpr (ref_is_glvalue) return (x);
				else return (parent_->inner_ = view::all(std::move(x)));
			};

			for (; outer_ != __stl2::end(parent_->base_); ++outer_) {
				auto& inner_range = update_inner_range(*outer_);
				inner_ = __stl2::begin(inner_range);
				if (inner_ != __stl2::end(inner_range)) return;
			}
			// needed for symmetric iterator comparison:
			if constexpr (ref_is_glvalue) {
				inner_ = iterator_t<iter_reference_t<iterator_t<Base>>>();
			}
		}
	public:
		using value_type =
			iter_value_t<iterator_t<iter_reference_t<iterator_t<Base>>>>;
		using difference_type = common_type_t<
			iter_difference_t<iterator_t<Base>>,
			iter_difference_t<iterator_t<iter_reference_t<iterator_t<Base>>>>>;

		__iterator() = default;

		constexpr __iterator(Parent& parent, iterator_t<V> outer)
		: outer_(std::move(outer)), parent_(std::addressof(parent)) {
			satisfy_();
		}

		constexpr __iterator(__iterator<!Const> i) requires Const &&
			ConvertibleTo<iterator_t<V>, iterator_t<Base>> &&
			ConvertibleTo<
				iterator_t<InnerRng>,
				iterator_t<iter_reference_t<iterator_t<Base>>>>
		: outer_(std::move(i.outer_))
		, inner_(std::move(i.inner_))
		, parent_(i.parent_) {}

		constexpr decltype(auto) operator*() const { return *inner_; }

		constexpr iterator_t<Base> operator->() const
		requires __has_arrow<iterator_t<Base>> {
			return inner_;
		}

		constexpr __iterator& operator++() {
			auto&& inner_rng = [this]() -> auto&& {
				if constexpr (ref_is_glvalue) return *outer_;
				else return parent_->inner_;
			}();

			if (++inner_ == __stl2::end(inner_rng)) {
				++outer_;
				satisfy_();
			}
			return *this;
		}

		constexpr void operator++(int) { ++*this; }

		constexpr __iterator operator++(int)
		requires ref_is_glvalue && ForwardRange<Base> &&
			ForwardRange<iter_reference_t<iterator_t<Base>>>
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		constexpr __iterator& operator--()
		requires ref_is_glvalue && BidirectionalRange<Base> &&
			BidirectionalRange<iter_reference_t<iterator_t<Base>>> &&
			CommonRange<iter_reference_t<iterator_t<Base>>>
		{
			if (outer_ == __stl2::end(parent_->base_)) {
				inner_ = __stl2::end(*--outer_);
			}
			while (inner_ == __stl2::begin(*outer_)) {
				inner_ = __stl2::end(*--outer_);
			}
			--inner_;
			return *this;
		}

		constexpr __iterator operator--(int)
		requires ref_is_glvalue && BidirectionalRange<Base> &&
			BidirectionalRange<iter_reference_t<iterator_t<Base>>> &&
			CommonRange<iter_reference_t<iterator_t<Base>>>
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		friend constexpr bool operator==(const __iterator& x, const __iterator& y)
		requires ref_is_glvalue && EqualityComparable<iterator_t<Base>> &&
			EqualityComparable<iterator_t<iter_reference_t<iterator_t<Base>>>>
		{ return bool(x.outer_ == y.outer_) && bool(x.inner_ == y.inner_); } // TODO: file LWG issue

		friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
		requires ref_is_glvalue && EqualityComparable<iterator_t<Base>> &&
			EqualityComparable<iterator_t<iter_reference_t<iterator_t<Base>>>>
		{ return !(x == y); }

		friend constexpr decltype(auto) iter_move(const __iterator& i)
		noexcept(noexcept(__stl2::iter_move(i.inner_)))
		{ return __stl2::iter_move(i.inner_); }

		friend constexpr void iter_swap(const __iterator& x, const __iterator& y)
		noexcept(noexcept(__stl2::iter_swap(x.inner_, y.inner_)))
		{ __stl2::iter_swap(x.inner_, y.inner_); }
	};

	template<InputRange V>
	requires View<V> && InputRange<iter_reference_t<iterator_t<V>>> &&
		(detail::_GLvalueRange<V> || View<iter_value_t<iterator_t<V>>>)
	template<bool Const>
	struct join_view<V>::__sentinel {
	private:
		friend __sentinel<true>;
		using Base = __maybe_const<Const, V>;
		using Parent = __maybe_const<Const, join_view>;

		sentinel_t<Base> end_ = sentinel_t<Base>();

		constexpr bool equal(const __iterator<Const>& i) const {
			return i.outer_ == end_;
		}
	public:
		__sentinel() = default;

		constexpr explicit __sentinel(Parent& parent)
		: end_(__stl2::end(parent.base_)) {}

		constexpr __sentinel(__sentinel<!Const> s) requires Const &&
			ConvertibleTo<sentinel_t<V>, sentinel_t<Base>>
		: end_(std::move(s.end_)) {}

		friend constexpr bool operator==(const __iterator<Const>& x, const __sentinel& y)
		{ return y.equal(x); }
		friend constexpr bool operator==(const __sentinel& x, const __iterator<Const>& y)
		{ return x.equal(y); }
		friend constexpr bool operator!=(const __iterator<Const>& x, const __sentinel& y)
		{ return !y.equal(x); }
		friend constexpr bool operator!=(const __sentinel& x, const __iterator<Const>& y)
		{ return !x.equal(y); }
	};

	namespace view {
		struct __join_fn : detail::__pipeable<__join_fn> {
			template<class R>
			constexpr auto operator()(R&& r) const
#if STL2_WORKAROUND_CLANGC_50
			requires requires(R&& r) { join_view{static_cast<R&&>(r)}; } {
				return join_view{static_cast<R&&>(r)};
			}
#else // ^^^ workaround / no workaround vvv
			STL2_REQUIRES_RETURN(
				join_view{static_cast<R&&>(r)}
			)
#endif // STL2_WORKAROUND_CLANGC_50
		};

		inline constexpr __join_fn join {};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif
