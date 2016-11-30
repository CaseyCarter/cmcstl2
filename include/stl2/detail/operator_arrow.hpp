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
#ifndef STL2_DETAIL_OPERATOR_ARROW_HPP
#define STL2_DETAIL_OPERATOR_ARROW_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
	// FIXME: Remove after merge and #include <stl2/detail/memory/addressof.hpp>
	template <class T>
	constexpr T* addressof(T& t) noexcept;

		namespace detail {
		template <Readable I>
		requires
			_IsNot<reference_t<I>, is_reference> &&
			models::Constructible<value_type_t<I>, reference_t<I>>
		class operator_arrow_proxy {
			value_type_t<I> value_;
		public:
			constexpr explicit operator_arrow_proxy(reference_t<I>&& x)
			noexcept(is_nothrow_constructible<value_type_t<I>, reference_t<I>>::value)
			: value_(__stl2::move(x)) {}

			STL2_CONSTEXPR_EXT const value_type_t<I>* operator->() const noexcept {
				return __stl2::addressof(value_);
			}
		};

		Readable{I}
		constexpr I operator_arrow(const I& i, ext::priority_tag<2>)
		noexcept(is_nothrow_copy_constructible<I>::value)
		requires
			_Is<I, is_pointer> || requires { i.operator->(); }
		{
			return i;
		}

		Readable{I}
		STL2_CONSTEXPR_EXT auto operator_arrow(const I& i, ext::priority_tag<1>)
		noexcept(noexcept(*i))
		requires
			_Is<reference_t<const I>, is_reference>
		{
			return __stl2::addressof(*i);
		}

		Readable{I}
		operator_arrow_proxy<I> operator_arrow(const I& i, ext::priority_tag<0>)
		noexcept(is_nothrow_move_constructible<operator_arrow_proxy<I>>::value &&
						 noexcept(operator_arrow_proxy<I>{*i}))
		{
			return operator_arrow_proxy<I>{*i};
		}
	}

	template <Readable I>
	requires
		requires (const I i) {
			detail::operator_arrow(i, ext::max_priority_tag);
		}
	decltype(auto) __operator_arrow(const I& i)
	STL2_NOEXCEPT_RETURN(
		detail::operator_arrow(i, ext::max_priority_tag)
	)
} STL2_CLOSE_NAMESPACE

#endif
