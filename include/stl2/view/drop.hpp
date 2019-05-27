// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Eric Niebler
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_DROP_HPP
#define STL2_VIEW_DROP_HPP

#include <stl2/detail/cached_position.hpp>
#include <stl2/detail/closure.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/nth_iterator.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<View R>
		class STL2_EMPTY_BASES drop_view
		: public view_interface<drop_view<R>>
		, private detail::cached_position<R, drop_view<R>, !RandomAccessRange<const R>> {
			using D = iter_difference_t<iterator_t<R>>;
		public:
			drop_view() = default;

			constexpr drop_view(R base, D count)
			: base_(std::move(base)), count_(count)
			{}

			constexpr R base() const { return base_; }

			constexpr auto begin() requires (!SimpleView<R> || !RandomAccessRange<R>)
			{ return begin_impl(*this); }
			constexpr auto begin() const requires Range<const R> && RandomAccessRange<const R>
			{ return begin_impl(*this); }

			constexpr auto end() requires (!SimpleView<R>)
			{ return end_impl(*this); }
			constexpr auto end() const requires Range<const R>
			{ return end_impl(*this); }

			constexpr auto size() requires (!SimpleView<R> && SizedRange<R>) { return size_impl(*this); }
			constexpr auto size() const requires SizedRange<const R> { return size_impl(*this); }
		private:
			R base_;
			D count_;

			template<class X>
			static constexpr auto begin_impl(X& x) {
				if constexpr (RandomAccessRange<__maybe_const<std::is_const_v<X>, R>>) {
					return __stl2::ext::nth_iterator(x.base_, x.count_);
				} else {
					using cache_t = typename drop_view::cached_position;
					auto& cache = static_cast<cache_t&>(x);
					if (cache) {
						return cache.get(x.base_);
					}

					auto iter = __stl2::ext::nth_iterator(x.base_, x.count_);
					cache.set(x.base_, iter);
					return iter;
				}
			}

			template<class X>
			static constexpr auto end_impl(X& x) { return __stl2::end(x.base_); }

			template<class X>
			static constexpr auto size_impl(X& x) {
				auto const size = __stl2::size(x.base_);
				auto const count = static_cast<decltype(size)>(x.count_);
				return size < count ? 0 : size - count;
			}
		};

		template<Range R>
		drop_view(R&&, iter_difference_t<iterator_t<R>>) -> drop_view<all_view<R>>;
	} // namespace ext

	namespace view::ext {
		struct __drop_fn : detail::__pipeable<__drop_fn> {
			template<Range Rng>
			constexpr auto operator()(Rng&& rng, iter_difference_t<iterator_t<Rng>> count) const
#if STL2_WORKAROUND_CLANGC_50
			requires requires(Rng&& rng, iter_difference_t<iterator_t<Rng>> count) {
				__stl2::ext::drop_view(all(std::forward<Rng>(rng)), count);
			} {
				return __stl2::ext::drop_view(all(std::forward<Rng>(rng)), count);
			}
#else // ^^^ workaround / no workaround vvv
			STL2_REQUIRES_RETURN(
				__stl2::ext::drop_view(all(std::forward<Rng>(rng)), count)
			)
#endif // STL2_WORKAROUND_CLANGC_50

			template<Integral D>
			constexpr auto operator()(D count) const
			{
				return detail::closure(*this, static_cast<D>(count));
			}
		};

		inline constexpr __drop_fn drop {};
	} // mamespace view::ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_DROP_HPP
