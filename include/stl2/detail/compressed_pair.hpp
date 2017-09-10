// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_COMPRESSED_PAIR_HPP
#define STL2_DETAIL_COMPRESSED_PAIR_HPP

#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/object.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <Destructible T, Destructible U>
		class compressed_pair :
			detail::ebo_box<T, meta::size_t<0>>,
			detail::ebo_box<U, meta::size_t<1>> {
			using first_t = detail::ebo_box<T, meta::size_t<0>>;
			using second_t = detail::ebo_box<U, meta::size_t<1>>;
		public:
			compressed_pair()
			requires DefaultConstructible<T> &&
				DefaultConstructible<U> = default;

			template <typename TT = T, typename UU = U>
			requires
				Constructible<T, TT> && Constructible<U, UU>
			constexpr compressed_pair(TT&& t, UU&& u)
			noexcept(is_nothrow_constructible<first_t, TT&&>::value &&
				is_nothrow_constructible<second_t, UU&&>::value)
			: first_t(std::forward<TT>(t))
			, second_t(std::forward<UU>(u)) {}

			constexpr T& first() & noexcept {
				return first_t::get();
			}
			constexpr const T& first() const& noexcept {
				return first_t::get();
			}
			constexpr T&& first() && noexcept {
				return std::move(*this).first_t::get();
			}
			constexpr const T&& first() const&& noexcept {
				return std::move(*this).first_t::get();
			}

			constexpr U& second() & noexcept {
				return second_t::get();
			}
			constexpr const U& second() const& noexcept {
				return second_t::get();
			}
			constexpr U&& second() && noexcept {
				return std::move(*this).second_t::get();
			}
			constexpr const U&& second() const&& noexcept {
				return std::move(*this).second_t::get();
			}
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
