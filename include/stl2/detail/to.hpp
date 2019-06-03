// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Eric Niebler
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_TO_HPP
#define STL2_DETAIL_TO_HPP

#include <stl2/detail/closure.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace __to {
		struct __adl_hook {
			explicit __adl_hook() = default;
		};

		template<class _Cont, class _IRange, class... _Args>
		META_CONCEPT __class_convertible = InputRange<_IRange> &&
			ForwardRange<std::remove_cv_t<_Cont>> && !View<std::remove_cv_t<_Cont>> &&
			MoveConstructible<std::remove_cv_t<_Cont>> &&
			// Proxy for Cpp17EmplaceConstructible:
			Constructible<ext::range_value_t<_Cont>, ext::range_reference_t<_IRange>> &&
			Constructible<_Cont, __range_common_iterator<_IRange>,
				__range_common_iterator<_IRange>, _Args...>;

		template<class _IRange, class _Cont, class... _Args>
		META_CONCEPT __reserve_assignable = SizedRange<_IRange> &&
			Constructible<_Cont, _Args...> && MoveConstructible<_Cont> &&
			requires(_Cont& __mc, const _Cont& __cc) {
#if STL2_BROKEN_COMPOUND_REQUIREMENT
				__cc.capacity(); requires Integral<decltype(__cc.capacity())>;
#else
				{ __cc.capacity() } -> Integral;
#endif // STL2_BROKEN_COMPOUND_REQUIREMENT
				__mc.reserve(__cc.capacity());
				requires Assignable<ext::range_reference_t<_Cont>, ext::range_reference_t<_IRange>>;
				__mc.assign(__range_common_iterator<_IRange>(), __range_common_iterator<_IRange>());
			};

		template<class _Cont>
		struct __class_to {
			template<class _IRange, class... _Args>
			requires __class_convertible<_Cont, _IRange, _Args...>
			constexpr auto operator()(_IRange&& __r, _Args&&... __args) const {
				using __ci = __range_common_iterator<_IRange>;
				if constexpr (__reserve_assignable<_IRange, remove_cv_t<_Cont>, _Args...>) {
					remove_cv_t<_Cont> __c(static_cast<_Args&&>(__args)...);
					auto const __size = size(__r);
					using __capacity = decltype(__c.capacity());
					// FIXME: What if __capacity can't represent __size?
					__c.reserve(static_cast<__capacity>(__size));
					__c.assign(__ci(begin(__r)), __ci(end(__r)));
					return __c;
				} else {
					return _Cont(__ci(begin(__r)), __ci(end(__r)), static_cast<_Args&&>(__args)...);
				}
			}
		};

		template<class _Cont, class... _Args>
		requires (CopyConstructible<_Args> && ...)
		struct STL2_EMPTY_BASES __class_closure : detail::closure<__class_to<_Cont>, _Args...> {
			constexpr __class_closure(_Args&&... __args)
			: detail::closure<__class_to<_Cont>, _Args...>{{}, static_cast<_Args&&>(__args)...} {}
		};

		template<class _Cont>
		constexpr __class_closure<_Cont> to(__adl_hook = __adl_hook{}) noexcept {
			return {};
		}

		template<class _IRange, class _Cont>
		requires __class_convertible<_Cont, _IRange>
		constexpr auto operator|(_IRange&& __r, __class_closure<_Cont>(&)(__adl_hook) noexcept) {
			return __class_to<_Cont>{}(static_cast<_IRange&&>(__r));
		}

		template<class _Cont, class _First, class... _Rest>
		requires __class_convertible<_Cont, _First, _Rest...> ||
			(CopyConstructible<_First> && (CopyConstructible<_Rest> && ...))
		constexpr auto to(_First&& __first, _Rest&&... __rest) {
			if constexpr (__class_convertible<_Cont, _First, _Rest...>) {
				return __class_to<_Cont>{}(
					static_cast<_First&&>(__first), static_cast<_Rest&&>(__rest)...);
			} else {
				return __class_closure<_Cont, _First, _Rest...>{
					static_cast<_First&&>(__first), static_cast<_Rest&&>(__rest)...};
			}
		}

		template<template<class...> class _ContT, InputRange _IRange, class... _Args>
		using __ctad = decltype(_ContT(__range_common_iterator<_IRange>(),
			__range_common_iterator<_IRange>(), std::declval<_Args>()...));

		template<template<class...> class _ContT, class _IRange, class... _Args>
		META_CONCEPT __CTADConvertible = InputRange<_IRange> && requires {
			typename __ctad<_ContT, _IRange, _Args...>;
			requires __class_convertible<__ctad<_ContT, _IRange, _Args...>, _IRange, _Args...>;
		};

		template<template<class...> class _ContT, class _IRange, class... _Args>
		META_CONCEPT __NoCTADConvertible = InputRange<_IRange> && requires {
			typename _ContT<ext::range_value_t<_IRange>>;
			requires __class_convertible<_ContT<ext::range_value_t<_IRange>>, _IRange, _Args...>;
		};

		template<template<class...> class _ContT, class _IRange, class... _Args>
		META_CONCEPT __template_convertible = __CTADConvertible<_ContT, _IRange, _Args...> ||
			__NoCTADConvertible<_ContT, _IRange, _Args...>;

		template<template<class...> class _ContT>
		struct __template_to {
			template<class _IRange, class... _Args>
			requires __template_convertible<_ContT, _IRange, _Args...>
			constexpr auto operator()(_IRange&& __r, _Args&&... __args) const {
				if constexpr (__CTADConvertible<_ContT, _IRange, _Args...>) {
					return __class_to<__ctad<_ContT, _IRange, _Args...>>{}(
						static_cast<_IRange&&>(__r), static_cast<_Args&&>(__args)...);
				} else {
					return __class_to<_ContT<ext::range_value_t<_IRange>>>{}(
						static_cast<_IRange&&>(__r), static_cast<_Args&&>(__args)...);
				}
			}
		};

		template<template<class...> class _ContT, class... _Args>
		requires (CopyConstructible<_Args> && ...)
		struct STL2_EMPTY_BASES __template_closure
		: detail::closure<__template_to<_ContT>, _Args...> {
			constexpr __template_closure(_Args&&... __args)
			: detail::closure<__template_to<_ContT>, _Args...>{
				{}, static_cast<_Args&&>(__args)...} {}
		};

		template<template<class...> class _ContT>
		constexpr __template_closure<_ContT> to(__adl_hook = __adl_hook{}) noexcept {
			return {};
		}

		template<template<class...> class _ContT, class _IRange>
		requires __template_convertible<_ContT, _IRange>
		constexpr auto operator|(
			_IRange&& __r, __template_closure<_ContT>(&)(__adl_hook) noexcept) {
			return __template_to<_ContT>{}(static_cast<_IRange&&>(__r));
		}

		template<template<class...> class _ContT, class _First, class... _Rest>
		requires __template_convertible<_ContT, _First, _Rest...> ||
			(CopyConstructible<_First> && (CopyConstructible<_Rest> && ...))
		constexpr auto to(_First&& __r, _Rest&&... __rest) {
			if constexpr (__template_convertible<_ContT, _First, _Rest...>) {
				return __template_to<_ContT>{}(
					static_cast<_First&&>(__r), static_cast<_Rest&&>(__rest)...);
			} else {
				return __template_closure<_ContT, _First, _Rest...>{
					static_cast<_First&&>(__r), static_cast<_Rest&&>(__rest)...};
			}
		}
	}

	using __to::to;
} STL2_CLOSE_NAMESPACE

#endif
