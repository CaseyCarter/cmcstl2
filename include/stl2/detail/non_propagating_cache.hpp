// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//

#ifndef STL2_DETAIL_NON_PROPAGATING_CACHE_HPP
#define STL2_DETAIL_NON_PROPAGATING_CACHE_HPP

#include <optional>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template<ext::DestructibleObject T, class Tag = void, bool Enable = true>
		struct non_propagating_cache : std::optional<T> {
			non_propagating_cache() = default;
			constexpr non_propagating_cache(nullopt_t) noexcept
			{}
			constexpr non_propagating_cache(non_propagating_cache const &) noexcept
			: std::optional<T>{}
			{}
			constexpr non_propagating_cache(non_propagating_cache &&that) noexcept
			: std::optional<T>{}
			{ that.std::optional<T>::reset(); }
			constexpr non_propagating_cache &operator=(non_propagating_cache const &) noexcept {
				std::optional<T>::reset();
				return *this;
			}
			constexpr non_propagating_cache &operator=(non_propagating_cache &&that) noexcept {
				that.std::optional<T>::reset();
				std::optional<T>::reset();
				return *this;
			}
			using std::optional<T>::operator=;
		};

		template<ext::DestructibleObject T, class Tag>
		struct non_propagating_cache<T, Tag, false> {};
	}
} STL2_CLOSE_NAMESPACE

#endif
