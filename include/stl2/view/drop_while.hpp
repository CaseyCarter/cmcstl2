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
#ifndef STL2_VIEW_DROP_WHILE_HPP
#define STL2_VIEW_DROP_WHILE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/algorithm/find_if_not.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/nth_iterator.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <utility>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <View R, IndirectPredicate<iterator_t<R>> Pred>
		class drop_while_view
		: public view_interface<drop_while_view<R, Pred>>
		, private detail::semiregular_box<Pred>
		, private detail::non_propagating_cache<iterator_t<R>, drop_while_view<R, Pred>> {
			using storage_t = detail::semiregular_box<Pred>;
			using storage_t::get;
		public:
			drop_while_view() = default;

			constexpr drop_while_view(R base, Pred pred)
			: storage_t{std::move(pred)}, base_(std::move(base)) {}

			template <ViewableRange O>
			requires _ConstructibleFromRange<R, O>
			constexpr drop_while_view(O&& o, Pred pred)
			: storage_t{std::move(pred)}, base_(view::all(static_cast<O&&>(o))) {}

			constexpr R base() const { return base_; }
			constexpr const Pred& pred() const noexcept { return get(); }

			constexpr auto begin() {
				using cache_t = typename drop_while_view::non_propagating_cache;
				auto& iterator_self = static_cast<cache_t&>(*this);
				if (!iterator_self) {
					iterator_self = __stl2::find_if_not(base_, pred());
				}

				return *iterator_self;
			}

			constexpr auto end() { return __stl2::end(base_); }
		private:
			R base_;
		};

		template <Range R, IndirectPredicate<iterator_t<R>> Pred>
		drop_while_view(R&&, Pred) -> drop_while_view<all_view<R>, Pred>;
	} // namespace ext

	namespace view::ext {
		struct __drop_while_fn : detail::__pipeable<__drop_while_fn> {
			template <class Rng, class Pred>
			constexpr auto operator()(Rng&& rng, Pred&& pred) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::drop_while_view{view::all(static_cast<Rng&&>(rng)), std::forward<Pred>(pred)}
			)

			template <CopyConstructible Pred>
			constexpr auto operator()(Pred pred) const
			{ return detail::view_closure{*this, std::move(pred)}; }
		};

		inline constexpr __drop_while_fn drop_while;
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_DROP_WHILE_HPP
