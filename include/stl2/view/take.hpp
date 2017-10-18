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
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/optional.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/algorithm/min.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <InputRange R>
		requires View<R>
		struct take_view;

		template <InputRange R>
		requires View<R>
		struct __take_view : view_interface<take_view<R>> {
		private:
			R base_ {};
			using D = difference_type_t<iterator_t<R>>;
			D count_ {};
			template <bool Const>
			struct __sentinel;
		public:
			__take_view() = default;

			constexpr __take_view(R base, D count)
			: base_(std::move(base)), count_(count) {}

			template <InputRange O>
			requires (std::is_lvalue_reference_v<O> || View<__f<O>>) &&
				Constructible<R, all_view<O>>
			constexpr __take_view(O&& o, D count)
			: base_(view::all(std::forward<O>(o))), count_(count) {}

			constexpr R base() const
			{ return base_; }

			constexpr auto begin()
			{ return __stl2::make_counted_iterator(__stl2::begin(base_), count_); }

			constexpr auto begin() const requires Range<const R>
			{ return __stl2::make_counted_iterator(__stl2::begin(base_), count_); }

			constexpr auto begin() requires RandomAccessRange<R> && SizedRange<R>
			{ return __stl2::begin(base_); }

			constexpr auto begin() const
			requires RandomAccessRange<const R> && SizedRange<const R>
			{ return __stl2::begin(base_); }

			constexpr auto end()
			{ return __sentinel<models::SimpleView<R>>{ranges::end(base_)}; }

			constexpr auto end() const requires Range<const R>
			{ return __sentinel<true>{ranges::end(base_)}; }

			constexpr auto end()
			requires RandomAccessRange<R> && SizedRange<R>
			{ return __stl2::begin(base_) + this->size(); }

			constexpr auto end() const
			requires RandomAccessRange<const R> && SizedRange<const R>
			{ return __stl2::begin(base_) + this->size(); }

			constexpr auto size() requires SizedRange<R>
			{ return __stl2::min((D)__stl2::size(base_), count_); }

			constexpr auto size() const requires SizedRange<const R>
			{ return __stl2::min((D)__stl2::size(base_), count_); }
		};

		template <class R>
		template <bool Const>
		struct __take_view<R>::__sentinel {
		private:
			using Parent = __maybe_const<Const, __take_view>;
			using Base =  __maybe_const<Const, R>;
			sentinel_t<Base> end_ {};
			using CI = counted_iterator<iterator_t<Base>>;
		public:
			__sentinel() = default;

			constexpr explicit __sentinel(sentinel_t<Base> end)
			: end_(end) {}

			constexpr __sentinel(__sentinel<!Const> s)
			requires Const && ConvertibleTo<sentinel_t<R>, sentinel_t<Base>>
			: end_(s.base()) {}

			constexpr sentinel_t<Base> base() const
			{ return end_; }

			friend constexpr bool operator==(const __sentinel& x, const CI& y)
			requires EqualityComparable<iterator_t<Base>>
			{ return 0 == y.count() || x.end_ == y.base(); }

			friend constexpr bool operator==(const CI& x, const __sentinel& y)
			requires EqualityComparable<iterator_t<Base>>
			{ return y == x;}

			friend constexpr bool operator!=(const __sentinel& x, const CI& y)
			requires EqualityComparable<iterator_t<Base>>
			{ return !(x == y); }

			friend constexpr bool operator!=(const CI& x, const __sentinel& y)
			requires EqualityComparable<iterator_t<Base>>
			{ return !(y == x); }
		};

		template <InputRange R>
		requires View<R>
		struct take_view : __take_view<R> {
			take_view() = default;
			using __take_view<R>::__take_view;
			using iterator = decltype(std::declval<ext::__take_view<R>&>().begin());
			using sentinel = decltype(std::declval<ext::__take_view<R>&>().end());
		};

		template <InputRange R>
		requires View<R> && Range<const R>
		struct take_view<R> : __take_view<R> {
			take_view() = default;
			using __take_view<R>::__take_view;
			using iterator = decltype(std::declval<ext::__take_view<R>&>().begin());
			using sentinel = decltype(std::declval<ext::__take_view<R>&>().end());
			using const_iterator = decltype(std::declval<const ext::__take_view<R>&>().begin());
			using const_sentinel = decltype(std::declval<const ext::__take_view<R>&>().end());
		};

		template <InputRange R>
		take_view(R&& base, difference_type_t<iterator_t<R>> n)
		  -> take_view<all_view<R>>;
	}

	namespace view {
		namespace __take {
			struct fn {
				template <InputRange Rng>
				requires std::is_lvalue_reference_v<Rng> || View<__f<Rng>>
				constexpr auto operator()(Rng&& rng, difference_type_t<iterator_t<Rng>> count) const
				{ return ext::take_view{std::forward<Rng>(rng), count}; }

				template <Integral D>
				constexpr auto operator()(D count) const
				{ return detail::view_closure{*this, (D)count}; }
			};
		}

		inline constexpr __take::fn take {};
	}
} STL2_CLOSE_NAMESPACE

#endif
