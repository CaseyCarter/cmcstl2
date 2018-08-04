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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <View R>
		class drop_view
		: public view_interface<drop_view<R>>
		, private detail::non_propagating_cache<iterator_t<R>, drop_view<R>, !RandomAccessRange<const R>> {
			using D = iter_difference_t<iterator_t<R>>;
		public:
			drop_view() = default;

			constexpr drop_view(R base, D count)
				: base_(std::move(base)),
				  count_(count)
			{}

			template <ViewableRange O>
			requires
				_ConstructibleFromRange<R, O>
			constexpr drop_view(O&& o, D count)
				: base_(view::all(std::forward<O>(o))),
				  count_(count)
			{}

			constexpr R base() const { return base_; }

			constexpr auto begin()
			requires !(SimpleView<R> && RandomAccessRange<R>)
			{ return begin_impl(*this); }
			constexpr auto begin() const requires Range<const R> && RandomAccessRange<const R>
			{ return begin_impl(*this); }

			constexpr auto end()
			requires !(SimpleView<R>)
			{ return end_impl(*this); }
			constexpr auto end() const requires Range<const R>
			{ return end_impl(*this); }

			constexpr auto size() requires !SimpleView<R> && SizedRange<R> { return size_impl(*this); }
			constexpr auto size() const requires SizedRange<const R> { return size_impl(*this); }
		private:
			R base_;
			D count_;

			constexpr auto& cached_begin() noexcept
			{ return static_cast<typename drop_view::non_propagating_cache&>(*this); }

			template <class X>
			static constexpr auto begin_impl(X& x)
			{
				auto compute_begin = [&x]{
					STL2_EXPECT(x.count_ >= 0);
					if constexpr (SizedRange<__maybe_const<is_const_v<X>, R>>) {
						D const dist = __stl2::distance(x.base_);
						return __stl2::next(__stl2::begin(x.base_), dist < x.count_ ? dist : x.count_);
					}
					else {
						return __stl2::next(__stl2::begin(x.base_), x.count_, __stl2::end(x.base_));
					}
				};

				if constexpr (RandomAccessRange<__maybe_const<is_const_v<X>, R>>) {
					return compute_begin();
				}
				else {
					auto& iterator_self = x.cached_begin();
					if (!iterator_self) {
						iterator_self = compute_begin();
					}

					return *iterator_self;
				}
			}

			template <class X>
			static constexpr auto end_impl(X& x) { return __stl2::end(x.base_); }

			template <class X>
			static constexpr auto size_impl(X& x)
			{
				auto const size = __stl2::size(x.base_);
				auto const count = static_cast<decltype(size)>(x.count_);
				return size < count ? 0 : size - count;
			}
		};

		template <Range R>
		drop_view(R&&, iter_difference_t<iterator_t<R>>) -> drop_view<all_view<R>>;
	} // namespace ext

	namespace view::ext {
		struct __drop_fn : detail::__pipeable<__drop_fn> {
			template <Range Rng>
			constexpr auto operator()(Rng&& rng, iter_difference_t<iterator_t<Rng>> count) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::drop_view(all(std::forward<Rng>(rng)), count)
			)

			template <Integral D>
			constexpr auto operator()(D count) const
			{
				return detail::view_closure(*this, static_cast<D>(count));
			}
		};

		inline constexpr __drop_fn drop {};
	} // mamespace view::ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_DROP_HPP
