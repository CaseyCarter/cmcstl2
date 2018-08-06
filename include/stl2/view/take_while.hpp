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
#ifndef STL2_VIEW_TAKE_HPP
#define STL2_VIEW_TAKE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <View R, class Pred>
		requires InputRange<R> && std::is_object_v<Pred> &&
			IndirectUnaryPredicate<const Pred, iterator_t<R>>
		class take_while_view
		: public view_interface<take_while_view<R, Pred>>
		, private detail::semiregular_box<Pred> {
			template <bool> class __sentinel;
			using storage_t = detail::semiregular_box<Pred>;
			using storage_t::get;
		public:
			take_while_view() = default;

			constexpr take_while_view(R base, Pred pred)
			: storage_t{std::move(pred)}, base_(static_cast<R&&>(base)) {}

			template <ViewableRange O>
			requires _ConstructibleFromRange<R, O>
			constexpr take_while_view(O&& o, Pred pred)
			: storage_t{std::move(pred)}, base_(view::all(static_cast<O&&>(o))) {}

			constexpr R base() const { return base_; }
			constexpr const Pred& pred() const noexcept { return get(); }

			constexpr auto begin() requires !SimpleView<R> { return begin_impl(*this); }
			constexpr auto begin() const requires Range<const R> { return begin_impl(*this); }

			constexpr auto end() requires !SimpleView<R> { return end_impl(*this); }
			constexpr auto end() const requires Range<const R>
			{ return end_impl(*this); }
		private:
			R base_;

			template <class Self>
			static constexpr auto begin_impl(Self& self) { return __stl2::begin(self.base_); }

			template <class Self>
			static constexpr auto end_impl(Self& self) {
				constexpr bool is_const = std::is_const_v<Self>;
				return __sentinel<is_const>{__stl2::end(self.base_), &self.pred()};
			}
		};

		template <class R, class Pred>
		take_while_view(R&&, Pred) -> take_while_view<all_view<R>, Pred>;

		template <class R, class Pred>
		template <bool Const>
		class take_while_view<R, Pred>::__sentinel {
			friend __sentinel<false>;
			using Base = __maybe_const<Const, R>;
			sentinel_t<Base> end_ {};
			const Pred* pred_;
		public:
			__sentinel() = default;

			constexpr explicit __sentinel(sentinel_t<Base> end, const Pred* pred)
			: end_(end), pred_(pred) {}

			constexpr __sentinel(__sentinel<!Const> s)
			requires Const && ConvertibleTo<sentinel_t<R>, sentinel_t<Base>>
			: end_(s.base()), pred_(s.pred_) {}

			constexpr sentinel_t<Base> base() const { return end_; }

			friend constexpr bool operator==(const __sentinel& x, const iterator_t<Base> y)
			{ return x.end_ == y || !(*x.pred_)(*y); }
			friend constexpr bool operator==(const iterator_t<Base>& x, const __sentinel& y) { return y == x; }
			friend constexpr bool operator!=(const __sentinel& x, const iterator_t<Base>& y) { return !(x == y); }
			friend constexpr bool operator!=(const iterator_t<Base>& x, const __sentinel& y) { return !(y == x); }
		};
	} // namespace ext

	namespace view::ext {
		struct __take_while_fn : detail::__pipeable<__take_while_fn> {
			template <class Rng, class Pred>
			constexpr auto operator()(Rng&& rng, Pred&& pred) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::take_while_view{view::all(static_cast<Rng&&>(rng)), std::forward<Pred>(pred)}
			)

			template <__stl2::ext::CopyConstructibleObject Pred>
			constexpr auto operator()(Pred pred) const
			{ return detail::view_closure{*this, std::move(pred)}; }
		};

		inline constexpr __take_while_fn take_while {};
	} // namespace view::ext
} STL2_CLOSE_NAMESPACE

#endif
