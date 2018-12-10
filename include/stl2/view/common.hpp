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
#ifndef STL2_VIEW_COMMON_HPP
#define STL2_VIEW_COMMON_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	// common_view [range.common.view]
	template<View V>
	requires (!CommonRange<V>)
	struct common_view : view_interface<common_view<V>> {
		common_view() = default;

		constexpr explicit common_view(V v) : base_(std::move(v)) {}

		template<ViewableRange R>
		requires (!CommonRange<R> && _ConstructibleFromRange<V, R>)
		constexpr explicit common_view(R&& r)
		: base_(view::all(std::forward<R>(r))) {}

		constexpr V base() const { return base_; }

		constexpr auto size() requires (!ext::SimpleView<V> && SizedRange<V>) {
			return __stl2::size(base_);
		}
		constexpr auto size() const requires SizedRange<const V> {
			return __stl2::size(base_);
		}

		constexpr auto begin() requires (!ext::SimpleView<V>) {
			if constexpr (RandomAccessRange<V> && SizedRange<V>)
				return __stl2::begin(base_);
			else
				return CI<false>{__stl2::begin(base_)};
		}
		constexpr auto begin() const requires Range<const V> {
			if constexpr (RandomAccessRange<const V> && SizedRange<const V>)
				return __stl2::begin(base_);
			else
				return CI<true>{__stl2::begin(base_)};
		}

		constexpr auto end() requires (!ext::SimpleView<V>) {
			if constexpr (RandomAccessRange<V> && SizedRange<V>)
				return __stl2::begin(base_) + __stl2::size(base_);
			else
				return CI<false>{__stl2::end(base_)};
		}
		constexpr auto end() const requires Range<const V> {
			if constexpr (RandomAccessRange<const V> && SizedRange<const V>)
				return __stl2::begin(base_) + __stl2::size(base_);
			else
				return CI<true>{__stl2::end(base_)};
		}

	private:
		template<bool IsConst> using CI = common_iterator<
			iterator_t<__maybe_const<IsConst, V>>,
			sentinel_t<__maybe_const<IsConst, V>>>;

		V base_ = V();
	};

	template<class R>
	common_view(R&&) -> common_view<all_view<R>>;

	namespace view {
		struct __common_fn : detail::__pipeable<__common_fn> {
			template<ViewableRange R>
			constexpr auto operator()(R&& r) const {
				if constexpr (CommonRange<R>)
					return view::all(std::forward<R>(r));
				else
					return common_view{std::forward<R>(r)};
			}
		};

		inline constexpr __common_fn common {};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif
