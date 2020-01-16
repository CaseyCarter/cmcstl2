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
#include <stl2/detail/algorithm/find_if_not.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <utility>

STL2_OPEN_NAMESPACE {
		template<view R, indirect_unary_predicate<iterator_t<R>> Pred>
		requires input_range<R> && std::is_object_v<Pred>
		class STL2_EMPTY_BASES drop_while_view
		: public view_interface<drop_while_view<R, Pred>>
		, private detail::semiregular_box<Pred>
		, private detail::non_propagating_cache<iterator_t<R>, drop_while_view<R, Pred>> {
			using storage_t = detail::semiregular_box<Pred>;
			using storage_t::get;
		public:
			drop_while_view() = default;

			constexpr drop_while_view(R base, Pred pred)
			: storage_t{std::move(pred)}, base_(std::move(base)) {}

			constexpr R base() const { return base_; }
			constexpr const Pred& pred() const noexcept { return get(); }

			constexpr auto begin() {
				using cache_t = typename drop_while_view::non_propagating_cache;
				auto& iterator_self = static_cast<cache_t&>(*this);
				if (!iterator_self) {
					iterator_self = find_if_not(base_, [this](auto&& i) mutable {
						// A predicate must be equality-preserving. While it's not possible to generally
						// check that te predicate isn't equality-preserving, we can trap
						// non-equality-preserving invocables on-the-spot by calling them multiple times
						// and compare the results.
						// This is only enabled in debug-mode, since it's potentially an expensive check.
						auto result = get()(i);
						STL2_ASSERT(result == get()(i));
						return result;
					});
				}

				return *iterator_self;
			}

			constexpr auto end() { return __stl2::end(base_); }
		private:
			R base_{};
		};

		template<class R, class Pred>
		drop_while_view(R&&, Pred) -> drop_while_view<all_view<R>, Pred>;

	namespace views {
		struct __drop_while_fn : detail::__pipeable<__drop_while_fn> {
			template<class Rng, class Pred>
			constexpr auto operator()(Rng&& rng, Pred&& pred) const
			STL2_REQUIRES_RETURN(
				drop_while_view{
					views::all(static_cast<Rng&&>(rng)), std::forward<Pred>(pred)}
			)

			template<__stl2::ext::copy_constructible_object Pred>
			constexpr auto operator()(Pred pred) const
			{ return detail::view_closure{*this, std::move(pred)}; }
		};

		inline constexpr __drop_while_fn drop_while{};
	} // namespace views
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_DROP_WHILE_HPP
