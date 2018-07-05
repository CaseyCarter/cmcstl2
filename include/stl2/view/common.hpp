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
#ifndef STL2_VIEW_BOUNDED_VIEW_HPP
#define STL2_VIEW_BOUNDED_VIEW_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <View Rng>
		requires !CommonRange<Rng>
		struct common_view : view_interface<common_view<Rng>> {
		private:
			Rng rng_;
		public:
			common_view() = default;

			constexpr common_view(Rng rng)
			: rng_(std::move(rng)) {}

			template <ViewableRange O>
			requires !CommonRange<O> && _ConstructibleFromRange<Rng, O>
			constexpr common_view(O&& o)
			: rng_(view::all(std::forward<O>(o))) {}

			constexpr Rng base() const { return rng_; }

			constexpr auto begin()
			{ return common_iterator<iterator_t<Rng>, sentinel_t<Rng>>(__stl2::begin(rng_)); }
			constexpr auto end()
			{ return common_iterator<iterator_t<Rng>, sentinel_t<Rng>>(__stl2::end(rng_)); }

			constexpr auto begin() requires RandomAccessRange<Rng> && SizedRange<Rng>
			{ return __stl2::begin(rng_); }
			constexpr auto end() requires RandomAccessRange<Rng> && SizedRange<Rng>
			{ return __stl2::begin(rng_) + __stl2::size(rng_); }

			constexpr auto begin() const requires Range<const Rng>
			{
				return common_iterator<iterator_t<const Rng>, sentinel_t<const Rng>>(
					__stl2::begin(rng_));
			}
			constexpr auto end() const requires Range<const Rng>
			{
				return common_iterator<iterator_t<const Rng>, sentinel_t<const Rng>>(
					__stl2::end(rng_));
			}

			constexpr auto begin() const
			requires RandomAccessRange<const Rng> && SizedRange<const Rng>
			{ return __stl2::begin(rng_); }
			constexpr auto end() const
			requires RandomAccessRange<const Rng> && SizedRange<Rng>
			{ return __stl2::begin(rng_) + __stl2::size(rng_); }

			constexpr auto size() const requires SizedRange<const Rng>
			{ return __stl2::size(rng_); }
		};

		template <class O>
		common_view(O&&) -> common_view<all_view<O>>;
	} // namespace ext

	namespace view {
		namespace __stl2_common {
			struct fn : detail::__pipeable<fn> {
				template <ext::ViewableRange R>
				requires CommonRange<R>
				constexpr auto operator()(R&& r) const
				{ return view::all(std::forward<R>(r)); }

				template <ext::ViewableRange R>
				constexpr auto operator()(R&& r) const
				{ return ext::common_view{std::forward<R>(r)}; }
			};
		} // namespace __stl2_common

		inline constexpr __stl2_common::fn common {};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif
