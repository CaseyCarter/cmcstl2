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
#include <stl2/detail/non_propagating_cache.hpp>
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
	namespace ext {
		template <InputRange R, IndirectUnaryPredicate<iterator_t<R>> Pred>
		requires View<R>
		class filter_view : public view_interface<filter_view<R, Pred>> {
		private:
			R base_;
			detail::semiregular_box<Pred> pred_;
			detail::non_propagating_cache<iterator_t<R>> begin_;
			class iterator;
			class sentinel;
		public:
			filter_view() = default;

			constexpr filter_view(R base, Pred pred)
			: base_(std::move(base)), pred_(std::move(pred)) {}

			template <InputRange O>
			requires ext::ViewableRange<O> && _ConstructibleFromRange<R, O>
			constexpr filter_view(O&& o, Pred pred)
			: base_(view::all(std::forward<O>(o))), pred_(std::move(pred)) {}

			constexpr R base() const
			{ return base_; }

			constexpr iterator begin()
			{
				if(!begin_)
					begin_ = __stl2::find_if(base_, std::ref(pred_.get()));
				return iterator{*this, *begin_};
			}

			constexpr sentinel end()
			{ return sentinel{*this}; }

			constexpr iterator end() requires BoundedRange<R>
			{ return iterator{*this, __stl2::end(base_)}; }
		};

		template <class R, class Pred>
		class filter_view<R, Pred>::iterator {
		private:
			iterator_t<R> current_ {};
			filter_view* parent_ = nullptr;
			friend sentinel;
		public:
			using iterator_category =
				meta::if_c<models::BidirectionalIterator<iterator_t<R>>,
					__stl2::bidirectional_iterator_tag,
				meta::if_c<models::ForwardIterator<iterator_t<R>>,
					__stl2::forward_iterator_tag,
					__stl2::input_iterator_tag>>;
			using value_type = value_type_t<iterator_t<R>>;
			using difference_type = difference_type_t<iterator_t<R>>;

			iterator() = default;

			constexpr iterator(filter_view& parent, iterator_t<R> current)
			: current_(current), parent_(&parent) {}

			constexpr iterator_t<R> base() const
			{ return current_; }

			constexpr reference_t<iterator_t<R>> operator*() const
			{ return *current_; }

			// Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template<class II = iterator_t<R>>
			constexpr iterator_t<R> operator->() const
			requires is_pointer_v<iterator_t<R>> ||
				requires(II i) { i.operator->(); }
			{
				return current_;
			}

			constexpr iterator& operator++()
			{
				const auto last = __stl2::end(parent_->base_);
				STL2_ASSERT(current_ != last);
				current_ = __stl2::find_if(++current_, last, std::ref(parent_->pred_.get()));
				return *this;
			}

			constexpr void operator++(int)
			{ (void)++*this; }

			constexpr iterator operator++(int) requires ForwardRange<R>
			{
				auto tmp = *this;
				++*this;
				return tmp;
			}

			constexpr iterator& operator--() requires BidirectionalRange<R>
			{
				do
					--current_;
				while(__stl2::invoke(parent_->pred_.get(), *current_));
				return *this;
			}

			constexpr iterator operator--(int) requires BidirectionalRange<R>
			{
				auto tmp = *this;
				--*this;
				return tmp;
			}

			friend constexpr bool operator==(const iterator& x, const iterator& y)
			requires EqualityComparable<iterator_t<R>>
			{ return x.current_ == y.current_; }

			friend constexpr bool operator!=(const iterator& x, const iterator& y)
			requires EqualityComparable<iterator_t<R>>
			{ return !(x == y); }

			friend constexpr rvalue_reference_t<iterator_t<R>>
			iter_move(const iterator& i)
			noexcept(noexcept(__stl2::iter_move(i.current_)))
			{ return __stl2::iter_move(i.current_); }

			friend constexpr void iter_swap(const iterator& x, const iterator& y)
			noexcept(noexcept(__stl2::iter_swap(x.current_, y.current_)))
			{ __stl2::iter_swap(x.current_, y.current_); }
		};

		template <class R, class Pred>
		class filter_view<R, Pred>::sentinel {
		private:
			sentinel_t<R> end_;
		public:
			sentinel() = default;
			explicit constexpr sentinel(filter_view& parent)
			: end_(__stl2::end(parent)) {}

			constexpr sentinel_t<R> base() const
			{ return end_; }

			friend constexpr bool operator==(const iterator& x, const sentinel& y)
			{ return x.current_ == y.end_; }
			friend constexpr bool operator==(const sentinel& x, const iterator& y)
			{ return y == x; }
			friend constexpr bool operator!=(const iterator& x, const sentinel& y)
			{ return !(x == y); }
			friend constexpr bool operator!=(const sentinel& x, const iterator& y)
			{ return !(y == x); }
		};

		template <class R, class Pred>
		filter_view(R&&, Pred) -> filter_view<all_view<R>, Pred>;
	} // namespace ext

	namespace view {
		namespace __filter {
			struct fn {
				template <InputRange R, IndirectUnaryPredicate<iterator_t<R>> Pred>
				requires ext::ViewableRange<R>
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
