// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Christopher Di Bella
//  Copyright Casey Carter
//  Copyright Eric Niebler
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_STRIDE_HPP
#define STL2_VIEW_STRIDE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE{
	namespace ext {
		template<input_range R>
		requires view<R>
		struct STL2_EMPTY_BASES stride_view
		: view_interface<stride_view<R>> {
		private:
			template<bool>
			struct __iterator;

			using D = range_difference_t<R>;
		public:
			stride_view() = default;

			constexpr explicit stride_view(R base, D stride)
			: base_(static_cast<R&&>(base))
			, stride_{stride}
			{
				STL2_EXPECT(stride_ > 0);
			}

			constexpr __iterator<false> begin() requires (!ext::simple_view<R>)
			{ return __iterator<false>{*this}; }

			constexpr __iterator<true> begin() const requires range<const R>
			{ return __iterator<true>{*this}; }

			constexpr auto end() requires (!ext::simple_view<R>)
			{ return end_impl(*this); }

			constexpr auto end() const requires range<const R>
			{ return end_impl(*this); }

			constexpr range_difference_t<R> size() requires (!ext::simple_view<R> && sized_range<R>)
			{ return compute_distance(ranges::size(base_)); }

			constexpr range_difference_t<R> size() const requires sized_range<const R>
			{ return compute_distance(ranges::size(base_)); }

			constexpr range_difference_t<R> stride() const
			{ return stride_; }

			constexpr R base() const { return base_; }
		private:
			R base_{};
			D stride_ = D{};

			constexpr range_difference_t<R>
			compute_distance(range_difference_t<R> const distance) const noexcept
			{
				auto const quotient = distance / stride_;
				auto const remainder = distance % stride_;
				return quotient + static_cast<range_difference_t<R>>(remainder > 0);
			}

			template<class Self>
			static constexpr auto end_impl(Self& self)
			{
				if constexpr (sized_range<R> && common_range<R>) {
					return __iterator<std::is_const_v<Self>>{
						self,
						__stl2::end(self.base_),
						static_cast<range_difference_t<R>>(__stl2::size(self.base_)) % self.stride_
					};
				}
				else {
					return __stl2::end(self.base_);
				}
			}
		};

		template<range R>
		stride_view(R&&, range_difference_t<R>) -> stride_view<all_view<R>>;

		template<class R>
		template<bool is_const>
		struct stride_view<R>::__iterator {
		private:
			using parent_t = __maybe_const<is_const, stride_view>;
			using base_t = __maybe_const<is_const, R>;


			friend __iterator<!is_const>;

			parent_t* parent_ = nullptr;
			iterator_t<base_t> current_{};
			range_difference_t<base_t> step_{};
		public:
			using iterator_category = std::conditional_t<contiguous_range<R>,
			                                             std::random_access_iterator_tag,
			                                             iterator_category_t<iterator_t<base_t>>>;
			using value_type = range_value_t<base_t>;
			using difference_type = range_difference_t<base_t>;

			__iterator() = default;

			constexpr __iterator(parent_t& parent)
			: parent_(std::addressof(parent))
			, current_(ranges::begin(parent.base_))
			{}

			constexpr __iterator(parent_t& parent, iterator_t<base_t> end, range_difference_t<base_t> step)
			: parent_(std::addressof(parent))
			, current_(std::move(end))
			, step_{step}
			{}

			constexpr __iterator(__iterator<!is_const> const& other)
			requires is_const && convertible_to<iterator_t<R>, iterator_t<base_t>>
			: parent_(other.parent_)
			, current_(other.current_)
			{}

			constexpr iterator_t<base_t> base() const
			{ return current_; }

			constexpr decltype(auto) operator*() const
			{ return *current_; }

			constexpr __iterator& operator++()
			{ return advance(1); }

			constexpr void operator++(int)
			{ ++current_; }

			constexpr __iterator operator++(int) requires forward_range<base_t>
			{
				auto temp = *this;
				++*this;
				return temp;
			}

			constexpr __iterator& operator--() requires bidirectional_range<base_t>
			{ return advance(-1); }

			constexpr __iterator operator--(int) requires bidirectional_range<base_t>
			{
				auto temp = *this;
				--*this;
				return temp;
			}

			constexpr __iterator& operator+=(difference_type const n)
			requires random_access_range<base_t>
			{ return advance(n); }

			constexpr __iterator& operator-=(difference_type const n)
			requires random_access_range<base_t>
			{ return advance(-n); }

			constexpr decltype(auto) operator[](difference_type const n) const
			requires random_access_range<base_t>
			{ return *(*this + n); }

			friend constexpr bool operator==(const __iterator& x, const __iterator& y)
			requires equality_comparable<iterator_t<base_t>>
			{ return x.current_ == y.current_; }

			friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
			requires equality_comparable<iterator_t<base_t>>
			{ return !(x == y); }

			friend constexpr bool operator==(const __iterator& x, const sentinel_t<base_t>& y)
			{ return x.current_ == y; }

			friend constexpr bool operator==(const sentinel_t<base_t>& y, const __iterator& x)
			{ return x == y; }

			friend constexpr bool operator!=(const __iterator& x, const sentinel_t<base_t>& y)
			{ return !(x == y); }

			friend constexpr bool operator!=(const sentinel_t<base_t>& y, const __iterator& x)
			{ return !(x == y); }

			friend constexpr bool operator<(const __iterator& x, const __iterator& y)
			requires random_access_range<base_t>
			{ return x.current_ < y.current_; }

			friend constexpr bool operator>(const __iterator& x, const __iterator& y)
			requires random_access_range<base_t>
			{ return y < x; }

			friend constexpr bool operator<=(const __iterator& x, const __iterator& y)
			requires random_access_range<base_t>
			{ return !(y < x); }

			friend constexpr bool operator>=(const __iterator& x, const __iterator& y)
			requires random_access_range<base_t>
			{ return !(x < y); }

			friend constexpr __iterator operator+(__iterator i, difference_type n)
			requires random_access_range<base_t>
			{ return i += n; }

			friend constexpr __iterator operator+(difference_type n, __iterator i)
			requires random_access_range<base_t>
			{ return i += n; }

			friend constexpr __iterator operator-(__iterator i, difference_type n)
			requires random_access_range<base_t>
			{ return i -= n; }

			friend constexpr difference_type operator-(const __iterator& x, const __iterator& y)
			requires random_access_range<base_t>
			{ return x.parent_->compute_distance(x.current_ - y.current_); }

			friend constexpr difference_type operator-(const __iterator& x, const sentinel_t<base_t>& y)
			requires sized_sentinel_for<iterator_t<base_t>, sentinel_t<base_t>>
			{ return x.parent_->compute_distance(x.current_ - y); }

			friend constexpr difference_type operator-(const sentinel_t<base_t>& x, const __iterator& y)
			requires sized_sentinel_for<iterator_t<base_t>, sentinel_t<base_t>>
			{ return y.parent_->compute_distance(x - y.current_); }
		private:
			__iterator& advance(difference_type const n)
			{
				if constexpr (bidirectional_range<parent_t>) {
					if (n > 0) {
						step_ = parent_->stride_ - ranges::advance(current_, n * parent_->stride_, ranges::end(parent_->base_));
					}
					else if (n < 0) {
						auto const stride = step_ == 0 ? n * parent_->stride_
						                               : (n + 1) * parent_->stride_ - step_;
						step_ = ranges::advance(current_, stride, ranges::begin(parent_->base_));
					}
					return *this;
				}

				ranges::advance(current_, n * parent_->stride_, ranges::end(parent_->base_));
				return *this;
			}
		};
	} // namespace ext

	namespace views::ext {
		struct __stride_fn {
			template<input_range R>
			constexpr auto operator()(R&& r, __stl2::ext::range_difference_t<R> const n) const
			STL2_REQUIRES_RETURN(
				__stl2::ext::stride_view{
					views::all(static_cast<R&&>(r)),
					n
				}
			)

			template<integral I>
			constexpr auto operator()(I const n) const noexcept
			{ return detail::view_closure{*this, static_cast<I>(n)}; }
		};

		inline constexpr __stride_fn stride {};
	} // namespace views::ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_STRIDE_HPP
