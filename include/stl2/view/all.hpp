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
#ifndef STL2_VIEW_ALL_HPP
#define STL2_VIEW_ALL_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/iterator_range.hpp>

STL2_OPEN_NAMESPACE {
	namespace view {
        namespace __all {
            struct fn {
                template <Range Rng>
                requires View<__f<Rng>>
                constexpr __f<Rng> operator()(Rng&& rng) const {
                    return (Rng&&) rng;
                }
                template <SizedRange Rng>
                requires View<__f<Rng>>
                constexpr __f<Rng> operator()(Rng&& rng) const {
                    return (Rng&&) rng;
                }
                template <Range Rng>
                constexpr ext::iterator_range<iterator_t<Rng>, sentinel_t<Rng>>
                operator()(Rng& rng) const {
                    return {__stl2::begin(rng), __stl2::end(rng)};
                }
                template <SizedRange Rng>
                constexpr ext::sized_iterator_range<iterator_t<Rng>, sentinel_t<Rng>>
                operator()(Rng& rng) const {
                    using D = __stl2::difference_type_t<__stl2::iterator_t<Rng>>;
                    return {__stl2::begin(rng), __stl2::end(rng), (D)__stl2::size(rng)};
                }

                template <Range Rng>
                requires std::is_lvalue_reference_v<Rng> || View<decay_t<Rng>>
                friend constexpr decltype(auto) operator|(Rng&& rng, fn all) {
                    return all(std::forward<Rng>(rng));
                }
            };
        }

        inline constexpr __all::fn all {};
    }

    namespace ext {
        template <Range Rng>
        requires std::is_lvalue_reference_v<Rng> || View<decay_t<Rng>>
        using all_view = std::decay_t<decltype(view::all(std::declval<Rng>()))>;
    }
} STL2_CLOSE_NAMESPACE

#endif
