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

#include <stl2/detail/cached_position.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template<input_range V, indirect_unary_predicate<iterator_t<V>> Pred>
	requires view<V>
	class filter_view : public view_interface<filter_view<V, Pred>> {
	private:
		class __iterator;
		class __sentinel;

		V base_;
		detail::semiregular_box<Pred> pred_;
		detail::cached_position<V> begin_;

	public:
		filter_view() = default;

		constexpr filter_view(V base, Pred pred)
		: base_(std::move(base)), pred_(std::move(pred)) {}

		constexpr V base() const
		{ return base_; }

		constexpr __iterator begin()
		{
			auto cached = static_cast<bool>(begin_);
			iterator_t<V> first = cached
				? begin_.get(base_)
				: find_if(base_, __stl2::ref(pred_.get()));
			if(!cached)
				begin_.set(base_, first);
			return __iterator{*this, std::move(first)};
		}

		constexpr __sentinel end()
		{ return __sentinel{*this}; }

		constexpr __iterator end() requires common_range<V>
		{ return __iterator{*this, __stl2::end(base_)}; }
	};

	template<input_range V, indirect_unary_predicate<iterator_t<V>> Pred>
	requires view<V>
	class filter_view<V, Pred>::__iterator {
	private:
		iterator_t<V> current_{};
		filter_view* parent_ = nullptr;
		friend __sentinel;
	public:
		using iterator_category =
			meta::if_c<bidirectional_iterator<iterator_t<V>>,
				__stl2::bidirectional_iterator_tag,
			meta::if_c<forward_iterator<iterator_t<V>>,
				__stl2::forward_iterator_tag,
				__stl2::input_iterator_tag>>;
		using value_type = iter_value_t<iterator_t<V>>;
		using difference_type = iter_difference_t<iterator_t<V>>;

		__iterator() = default;

		constexpr __iterator(filter_view& parent, iterator_t<V> current)
		: current_(current), parent_(&parent) {}

		constexpr iterator_t<V> base() const
		{ return current_; }

		constexpr iter_reference_t<iterator_t<V>> operator*() const
		{ return *current_; }

		// Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		template<class II = iterator_t<V>>
		constexpr iterator_t<V> operator->() const
		requires std::is_pointer_v<iterator_t<V>> ||
			requires(II i) { i.operator->(); }
		{
			return current_;
		}

		constexpr __iterator& operator++()
		{
			const auto last = __stl2::end(parent_->base_);
			STL2_ASSERT(current_ != last);
			current_ = find_if(++current_, last, __stl2::ref(parent_->pred_.get()));
			return *this;
		}

		constexpr void operator++(int)
		{ (void)++*this; }

		constexpr __iterator operator++(int) requires forward_range<V>
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		constexpr __iterator& operator--() requires bidirectional_range<V>
		{
			do
				--current_;
			while(!__stl2::invoke(parent_->pred_.get(), *current_));
			return *this;
		}

		constexpr __iterator operator--(int) requires bidirectional_range<V>
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		friend constexpr bool operator==(const __iterator& x, const __iterator& y)
		requires equality_comparable<iterator_t<V>>
		{ return x.current_ == y.current_; }

		friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
		requires equality_comparable<iterator_t<V>>
		{ return !(x == y); }

		friend constexpr iter_rvalue_reference_t<iterator_t<V>>
		iter_move(const __iterator& i)
		noexcept(noexcept(__stl2::iter_move(i.current_)))
		{ return __stl2::iter_move(i.current_); }

		friend constexpr void iter_swap(const __iterator& x, const __iterator& y)
		noexcept(noexcept(__stl2::iter_swap(x.current_, y.current_)))
		{ __stl2::iter_swap(x.current_, y.current_); }
	};

	template<input_range V, indirect_unary_predicate<iterator_t<V>> Pred>
	requires view<V>
	class filter_view<V, Pred>::__sentinel {
	private:
		sentinel_t<V> end_;

		constexpr bool equal(const __iterator& i) const {
			return i.current_ == end_;
		}
	public:
		__sentinel() = default;
		explicit constexpr __sentinel(filter_view& parent)
		: end_(__stl2::end(parent.base_)) {}

		constexpr sentinel_t<V> base() const
		{ return end_; }

		friend constexpr bool operator==(const __iterator& x, const __sentinel& y)
		{ return y.equal(x); }
		friend constexpr bool operator==(const __sentinel& x, const __iterator& y)
		{ return x.equal(y); }
		friend constexpr bool operator!=(const __iterator& x, const __sentinel& y)
		{ return !y.equal(x); }
		friend constexpr bool operator!=(const __sentinel& x, const __iterator& y)
		{ return !x.equal(y); }
	};

	template<class R, class Pred>
	filter_view(R&&, Pred) -> filter_view<all_view<R>, Pred>;

	namespace views {
		struct __filter_fn {
			template<input_range R, indirect_unary_predicate<iterator_t<R>> Pred>
			requires viewable_range<R>
			constexpr auto operator()(R&& rng, Pred pred) const
			STL2_REQUIRES_RETURN(
				filter_view<all_view<R>, Pred>{std::forward<R>(rng), std::move(pred)}
			)

			template<copy_constructible Pred>
			constexpr auto operator()(Pred pred) const {
				return detail::view_closure{*this, std::move(pred)};
			}
		};

		inline constexpr __filter_fn filter{};
	}
} STL2_CLOSE_NAMESPACE

#endif
