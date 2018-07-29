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
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <Range R>
		requires
			View<R>
		struct drop_view;

		template <Range R>
		requires
			View<R>
		class __drop_view : public view_interface<drop_view<R>> {
			using D = iter_difference_t<iterator_t<R>>;

			template <bool Const>
			struct __sentinel;
		public:
			__drop_view() = default;

			constexpr __drop_view(R base, D count)
				: base_(std::move(base)),
				  count_(count)
			{}

			template <Range O>
			requires
				ext::ViewableRange<O> &&
				_ConstructibleFromRange<R, O>
			constexpr __drop_view(O&& o, D count)
				: base_(view::all(std::forward<O>(o))),
				  count_(count)
			{}

			constexpr R base() const
			{
				return base_;
			}

			constexpr auto begin()
			{
				return begin_impl(*this);
			}

			constexpr auto begin() const
			requires
				Range<R const>
			{
				return begin_impl(*this);
			}

			constexpr auto begin()
			requires
				SizedRange<R>
			{
				return begin_impl(*this);
			}

			constexpr auto begin() const
			requires
				SizedRange<R const>
			{
				return begin_impl(*this);
			}

			constexpr auto end()
			{
				return __sentinel<SimpleView<R>>(__stl2::end(base_));
			}

			constexpr auto end() const
			requires
				Range<R const>
			{
				return __sentinel<true>(__stl2::end(base_));
			}

			constexpr auto size() const
			requires
				SizedRange<R>
			{
				return __stl2::distance(base_) < count_ ? 0 : __stl2::distance(base_) - count_;
			}
		private:
			R base_;
			D count_;

			template <class X>
			static constexpr decltype(auto) begin_impl(X&& x)
			{
				return __stl2::next(__stl2::begin(x.base_), x.count_);
			}

			template <class X>
			static constexpr decltype(auto) begin_impl(X&& x)
			requires
				SizedRange<R>
			{
				D const dist = __stl2::distance(x.base_);
				return (__stl2::next(__stl2::begin(x.base_), dist < x.count_ ? dist : x.count_));
			}
		};

		template <class R>
		template <bool Const>
		class __drop_view<R>::__sentinel {
			using Base = __maybe_const<Const, R>;
			sentinel_t<Base> end_ {};
		public:
			__sentinel() = default;

			constexpr explicit __sentinel(sentinel_t<Base> end)
				: end_(end)
			{}

			constexpr __sentinel(__sentinel<!Const> s)
			requires
				Const &&
				ConvertibleTo<sentinel_t<R>, sentinel_t<Base>>
				: end_(s.base())
			{}

			constexpr sentinel_t<Base> base() const
			{
				return end_;
			}

			friend constexpr bool operator==(__sentinel const& x, iterator_t<Base> const& y)
			requires
				EqualityComparable<iterator_t<Base>>
			{
				return x.end_ == y;
			}

			friend constexpr bool operator==(iterator_t<Base> const& x, __sentinel const& y)
			requires
				EqualityComparable<iterator_t<Base>>
			{
				return y == x;
			}

			friend constexpr bool operator!=(__sentinel const& x, iterator_t<Base> const& y)
			requires
				EqualityComparable<iterator_t<Base>>
			{
				return !(x == y);
			}

			friend constexpr bool operator!=(iterator_t<Base> const& x, __sentinel const& y)
			requires
				EqualityComparable<iterator_t<Base>>
			{
				return !(y == x);
			}
		};

		template <Range R>
		requires
			View<R>
		struct drop_view : __drop_view<R> {
			drop_view() = default;
			using __drop_view<R>::__drop_view;
			using iterator = decltype(std::declval<ext::__drop_view<R>&>().base());
			using sentinel = decltype(std::declval<ext::__drop_view<R>&>().end());
		};

		template <Range R>
		requires
			View<R> &&
			Range<R const>
		struct drop_view<R> : __drop_view<R> {
			drop_view() = default;
			using __drop_view<R>::__drop_view;
			using iterator = decltype(std::declval<ext::__drop_view<R>&>().base());
			using sentinel = decltype(std::declval<ext::__drop_view<R>&>().end());
			using const_iterator = decltype(std::declval<ext::__drop_view<R> const&>().base());
			using const_sentinel = decltype(std::declval<ext::__drop_view<R> const&>().end());
		};

		template <Range R>
		drop_view(R&&, iter_difference_t<iterator_t<R>>) -> drop_view<all_view<R>>;
	} // namespace ext

	namespace view::ext {
		struct __drop_fn {
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
