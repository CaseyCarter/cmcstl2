// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Dvir Yitzchaki 2019
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_CONST_HPP
#define STL2_VIEW_CONST_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
	template <input_range R>
	requires view<R>
	struct const_view : view_interface<const_view<R>> {
		template <bool>
		struct __iterator;

	public:
		const_view() = default;
		constexpr explicit const_view(R base) : base_(std::move(base)) {}

		constexpr R base() const noexcept { return base_; }

		constexpr __iterator<false> begin() requires(!ext::simple_view<R>) {
			return __iterator<false>{*this, __stl2::begin(base_)};
		}

		constexpr __iterator<true> begin() const requires range<const R> {
			return __iterator<true>{*this, __stl2::begin(base_)};
		}

		constexpr auto end() requires(!ext::simple_view<R>) {
			return end_impl(*this);
		}

		constexpr auto end() const requires range<const R> {
			return end_impl(*this);
		}

		constexpr auto size() requires(!ext::simple_view<R> && sized_range<R>) {
			return __stl2::size(base_);
		}

		constexpr auto size() const requires sized_range<const R> {
			return __stl2::size(base_);
		}

	private:
		R base_ = R{};

		template <class Self>
		static constexpr auto end_impl(Self& self) {
			if constexpr (common_range<R>) {
				return __iterator<std::is_const_v<Self>>{self, __stl2::end(self.base_)};
			} else {
				return __stl2::end(self.base_);
			}
		}
	};

	template <input_range R>
	const_view(R&& r)->const_view<all_view<R>>;

	template <input_range R>
	template <bool is_const>
	class const_view<R>::__iterator {
		using parent_t = __maybe_const<is_const, const_view>;
		using base_t = __maybe_const<is_const, R>;

		friend __iterator<!is_const>;

		parent_t* parent_ = nullptr;
		iterator_t<base_t> current_{};

	public:
		using iterator_category = iterator_category_t<iterator_t<base_t>>;
		using value_type = std::add_const_t<range_value_t<base_t>>;
		using difference_type = range_difference_t<base_t>;

		__iterator() = default;

		constexpr __iterator(parent_t& parent, iterator_t<base_t> current)
			: parent_{std::addressof(parent)}, current_{current} {}

		constexpr __iterator(__iterator<!is_const> const& other) 
    requires is_const&& convertible_to<iterator_t<R>, iterator_t<base_t>>
			: parent_(other.parent_), current_(other.current_) {}

		constexpr iterator_t<base_t> base() const { return current_; }

		constexpr decltype(auto) operator*() const {
			return std::as_const(*current_);
		}

		constexpr __iterator& operator++() {
			++current_;
			return *this;
		}

		constexpr void operator++(int) { (void)++*this; }

		constexpr __iterator operator++(int) requires forward_range<base_t> {
			auto temp = *this;
			++*this;
			return temp;
		}

		constexpr __iterator& operator--() requires bidirectional_range<base_t> {
			--current_;
			return *this;
		}

		constexpr __iterator operator--(int) requires bidirectional_range<base_t> {
			auto temp = *this;
			--*this;
			return temp;
		}

		constexpr __iterator& operator+=(difference_type const n)
		requires random_access_range<base_t> {
			current_ += n;
			return *this;
		}

		constexpr __iterator& operator-=(difference_type const n)
		requires random_access_range<base_t> {
			current_ -= n;
			return *this;
		}

		constexpr decltype(auto) operator[](difference_type const n) const
			requires random_access_range<base_t> {
			return *(*this + n);
		}

		friend constexpr bool operator==(const __iterator& x, const __iterator& y)
		requires equality_comparable<iterator_t<base_t>> {
			return x.current_ == y.current_;
		}

		friend constexpr bool operator!=(const __iterator& x,
			const __iterator& y) requires equality_comparable<iterator_t<base_t>> {
			return !(x == y);
		}

		friend constexpr bool operator==(const __iterator& x,
										 const sentinel_t<base_t>& y) {
			return x.current_ == y;
		}

		friend constexpr bool operator==(const sentinel_t<base_t>& y,
										 const __iterator& x) {
			return x == y;
		}

		friend constexpr bool operator!=(const __iterator& x,
										 const sentinel_t<base_t>& y) {
			return !(x == y);
		}

		friend constexpr bool operator!=(const sentinel_t<base_t>& y,
										 const __iterator& x) {
			return !(x == y);
		}

		friend constexpr bool operator<(const __iterator& x, const __iterator& y)
		requires random_access_range<base_t> {
			return x.current_ < y.current_;
		}

		friend constexpr bool operator>(const __iterator& x, const __iterator& y)
		requires random_access_range<base_t> {
			return y < x;
		}

		friend constexpr bool operator<=(const __iterator& x, const __iterator& y) 
    requires random_access_range<base_t> {
			return !(y < x);
		}

		friend constexpr bool operator>=(const __iterator& x, const __iterator& y) 
    requires random_access_range<base_t> {
			return !(x < y);
		}

		friend constexpr __iterator operator+(__iterator i, difference_type n) 
    requires random_access_range<base_t> {
			return i += n;
		}

		friend constexpr __iterator operator+(difference_type n, __iterator i) 
    requires random_access_range<base_t> {
			return i += n;
		}

		friend constexpr __iterator operator-(__iterator i, difference_type n) 
    requires random_access_range<base_t> {
			return i -= n;
		}

		friend constexpr difference_type operator-(const __iterator& x, const __iterator& y)
		requires random_access_range<base_t> {
			return x.current_ - y.current_;
		}

		friend constexpr difference_type operator-(const __iterator& x, const sentinel_t<base_t>& y)
		requires sized_sentinel_for<iterator_t<base_t>, sentinel_t<base_t>> {
			return x.current_ - y;
		}

		friend constexpr difference_type operator-(const sentinel_t<base_t>& x, const __iterator& y)
		requires sized_sentinel_for<iterator_t<base_t>, sentinel_t<base_t>> {
			return x - y.current_;
		}
	};

	} // namespace ext

	namespace views::ext {
	struct __as_const_fn : detail::__pipeable<__as_const_fn> {
		template <viewable_range Rng>
		constexpr auto operator()(Rng&& rng) const {
			return __stl2::ext::const_view{std::forward<Rng>(rng)};
		}
	};

	inline constexpr __as_const_fn as_const{};
	} // namespace views::ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_CONST_HPP
