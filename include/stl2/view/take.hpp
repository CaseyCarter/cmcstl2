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
#ifndef STL2_VIEW_TAKE_HPP
#define STL2_VIEW_TAKE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template<view R>
	struct take_view : view_interface<take_view<R>> {
	private:
		template<bool> struct __sentinel;
		using D = iter_difference_t<iterator_t<R>>;

		R base_ {};
		D count_ {};

		template<class Self>
		static constexpr auto begin_(Self& self) {
			using RR = __maybe_const<std::is_const_v<Self>, R>;
			if constexpr (sized_range<RR>) {
				if constexpr (random_access_range<RR>) {
					return __stl2::begin(self.base_);
				} else {
					return counted_iterator{__stl2::begin(self.base_), static_cast<D>(self.size())};
				}
			} else {
				return counted_iterator{__stl2::begin(self.base_), self.count_};
			}
		}
		template<class Self>
		static constexpr auto end_(Self& self) {
			constexpr bool is_const = std::is_const_v<Self>;
			using RR = __maybe_const<is_const, R>;
			if constexpr (random_access_range<RR> && sized_range<RR>) {
				return __stl2::begin(self.base_) + self.size();
			} else if constexpr (sized_range<RR>) {
				return default_sentinel{};
			} else {
				return __sentinel<is_const>{__stl2::end(self.base_)};
			}
		}
		template<class Self>
		static constexpr auto size_(Self& self) {
			const auto n = __stl2::size(self.base_);
			const auto c = static_cast<decltype(n)>(self.count_);
			return n < c ? n : c;
		}
	public:
		take_view() = default;

		constexpr take_view(R base, D count)
		: base_(static_cast<R&&>(base)), count_(count) {}

		constexpr R base() const { return base_; }

		constexpr auto begin() requires (!ext::simple_view<R>) { return begin_(*this); }
		constexpr auto begin() const requires range<const R> { return begin_(*this); }

		constexpr auto end() requires (!ext::simple_view<R>) { return end_(*this); }
		constexpr auto end() const requires range<const R> || sized_range<R>
		{ return end_(*this); }

		constexpr auto size() requires (!ext::simple_view<R> && sized_range<R>) { return size_(*this); }
		constexpr auto size() const requires sized_range<const R> { return size_(*this); }
	};

	template<range R>
	take_view(R&&, iter_difference_t<iterator_t<R>>) -> take_view<all_view<R>>;

	template<view R>
	template<bool Const>
	struct take_view<R>::__sentinel {
	private:
		using Base = __maybe_const<Const, R>;
		using CI = counted_iterator<iterator_t<Base>>;

		sentinel_t<Base> end_ {};
	public:
		__sentinel() = default;

		constexpr explicit __sentinel(sentinel_t<Base> end)
		: end_(end) {}

		constexpr __sentinel(__sentinel<!Const> s)
		requires Const && convertible_to<sentinel_t<R>, sentinel_t<Base>>
		: end_(s.base()) {}

		constexpr sentinel_t<Base> base() const { return end_; }

		friend constexpr bool operator==(const __sentinel& x, const CI& y)
		{ return 0 == y.count() || x.end_ == y.base(); }
		friend constexpr bool operator==(const CI& x, const __sentinel& y) { return y == x;}
		friend constexpr bool operator!=(const __sentinel& x, const CI& y) { return !(x == y); }
		friend constexpr bool operator!=(const CI& x, const __sentinel& y) { return !(y == x); }
	};

	namespace views {
		struct __take_fn {
			template<range Rng>
			constexpr auto operator()(Rng&& rng, iter_difference_t<iterator_t<Rng>> count) const
			STL2_REQUIRES_RETURN(
				take_view{views::all(static_cast<Rng&&>(rng)), count}
			)

			template<integral D>
			constexpr auto operator()(D count) const
			{ return detail::view_closure{*this, static_cast<D>(count)}; }
		};

		inline constexpr __take_fn take {};
	}
} STL2_CLOSE_NAMESPACE

#endif
