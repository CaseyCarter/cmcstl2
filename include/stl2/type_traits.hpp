// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_TYPE_TRAITS
#define STL2_TYPE_TRAITS

#include <type_traits>

#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	////////////////////////////////////////////////////////////////////////////
	// common_type [meta.trans.other]
	//

	// Modified CREF from [meta.trans.other]/2.1 that doesn't work with void.
	template<class T>
	using __cref_nonvoid = const std::remove_reference_t<T>&;

	// COND_RES [meta.trans.other]/2.4
	template<class T, class U>
	using __cond_res =
		decltype(false ? std::declval<T(&)()>()() : std::declval<U(&)()>()());

	template<class...>
	struct common_type {};

	template<class... Ts>
	using common_type_t = meta::_t<common_type<Ts...>>;

	// [meta.trans.other]/3.2
	template<class T>
	struct common_type<T> : common_type<std::decay_t<T>, std::decay_t<T>> {};

	// [meta.trans.other]/3.3: "Otherwise, there shall be no member type."
	template<class, class> // NB: The arguments have been decayed by the time they get here.
	struct __common_type3 {};

	// [meta.trans.other]/3.3.4
	// I'm 99% certain that [meta.trans.other]/3.3.4 never produces a useful
	// common type when T or U is void - proper cases for void would be handled
	// earlier in 3.3.3 - so __cref_nonvoid is fine here.
	template<class T, class U> // NB: Ditto decayed.
	requires requires { typename __cond_res<__cref_nonvoid<T>, __cref_nonvoid<U>>; }
	struct __common_type3<T, U> {
		using type = std::decay_t<__cond_res<__cref_nonvoid<T>, __cref_nonvoid<U>>>;
	};

	template<class T, class U> // NB: Ditto decayed.
	struct __common_type2 : __common_type3<T, U> {};

	// Note: std::common_type handles [meta.trans.other]/3.3.3
	template<class T, class U> // NB: Ditto decayed.
	requires requires { typename std::common_type_t<T, U>; }
	struct __common_type2<T, U> {
		using type = std::common_type_t<T, U>;
	};

	template<class T, class U>
	struct common_type<T, U> : __common_type2<T, U> {};

	template<class T, class U>
	requires (!_Decayed<T> || !_Decayed<U>)
	struct common_type<T, U> : common_type<std::decay_t<T>, std::decay_t<U>> {};

	template<class T, class U, class V, class...  W>
	requires requires { typename common_type_t<std::decay_t<T>, std::decay_t<U>>; }
	struct common_type<T, U, V, W...>
	: common_type<common_type_t<std::decay_t<T>, std::decay_t<U>>, V, W...> {};

	////////////////////////////////////////////////////////////////////////////
	// basic_common_reference [meta.trans.other]
	//
	template<class, class, template<class> class, template<class> class>
	struct basic_common_reference {};

	////////////////////////////////////////////////////////////////////////////
	// common_reference [meta.trans.other]
	//
	template<class T>
	struct __unary {
		template<class U>
		using apply = meta::invoke<T, U>;
	};

	// CREF [meta.trans.other]/2.1
	template<class T>
	using __cref = std::add_lvalue_reference_t<const std::remove_reference_t<T>>;

	template<class From, class To>
	struct __copy_cv_ : meta::id<To> {};
	template<class From, class To>
	struct __copy_cv_<From const, To> : std::add_const<To> {};
	template<class From, class To>
	struct __copy_cv_<From volatile, To> : std::add_volatile<To> {};
	template<class From, class To>
	struct __copy_cv_<From const volatile, To> : std::add_cv<To> {};
	template<class From, class To>
	using __copy_cv = meta::_t<__copy_cv_<From, To>>;

	template<class T, class U>
	struct __builtin_common {};
	template<class T, class U>
	using __builtin_common_t = meta::_t<__builtin_common<T, U>>;
	template<class T, class U>
	requires meta::Valid<__cond_res, __cref<T>, __cref<U>>
	struct __builtin_common<T, U> : std::decay<__cond_res<__cref<T>, __cref<U>>> {};

	template<class T, class U, class R = __builtin_common_t<T &, U &>>
	using __rref_res = std::conditional_t<std::is_reference_v<R>,
		std::remove_reference_t<R> &&, R>;

	template<class T, class U>
	requires
		meta::Valid<__builtin_common_t, T &, U &> &&
		ConvertibleTo<T &&, __rref_res<T, U>> &&
		ConvertibleTo<U &&, __rref_res<T, U>>
	struct __builtin_common<T &&, U &&> : meta::id<__rref_res<T, U>> {};

	template<class T, class U>
	using __lref_res = __cond_res<__copy_cv<T, U> &, __copy_cv<U, T> &>;

	template<class T, class U>
	struct __builtin_common<T &, U &> : meta::defer<__lref_res, T, U> {};

	template<class T, class U>
	requires
		meta::Valid<__builtin_common_t, T &, U const &> &&
		ConvertibleTo<U &&, __builtin_common_t<T &, U const &>>
	struct __builtin_common<T &, U &&> : __builtin_common<T &, U const &> {};
	template<class T, class U>
	struct __builtin_common<T &&, U &> : __builtin_common<U &, T &&> {};

	namespace __qual {
		using __rref = meta::quote<std::add_rvalue_reference_t>;
		using __lref = meta::quote<std::add_lvalue_reference_t>;
		template<class>
		struct __xref : meta::id<meta::compose<meta::quote<meta::_t>, meta::quote<meta::id>>> {};
		template<class T>
		struct __xref<T&> : meta::id<meta::compose<__lref, meta::_t<__xref<T>>>> {};
		template<class T>
		struct __xref<T&&> : meta::id<meta::compose<__rref, meta::_t<__xref<T>>>> {};
		template<class T>
		struct __xref<const T> : meta::id<meta::quote<std::add_const_t>> {};
		template<class T>
		struct __xref<volatile T> : meta::id<meta::quote<std::add_volatile_t>> {};
		template<class T>
		struct __xref<const volatile T> : meta::id<meta::quote<std::add_cv_t>> {};
	}

	template<class T, class U>
	using __basic_common_reference =
		basic_common_reference<__uncvref<T>, __uncvref<U>,
			__unary<meta::_t<__qual::__xref<T>>>::template apply,
			__unary<meta::_t<__qual::__xref<U>>>::template apply>;

	template<class...>
	struct common_reference {};

	template<class... Ts>
	using common_reference_t = meta::_t<common_reference<Ts...>>;

	template<class T>
	struct common_reference<T> : meta::id<T> {};

	template<class T, class U>
	struct __common_reference2_2_ : common_type<T, U> {};

	template<class T, class U>
	requires requires { typename __cond_res<T, U>; }
	struct __common_reference2_2_<T, U> : meta::id<__cond_res<T, U>> {};

	template<class T, class U>
	struct __common_reference2_1_ : __common_reference2_2_<T, U> {};

	template<class T, class U>
	requires meta::Valid<meta::_t, __basic_common_reference<T, U>>
	struct __common_reference2_1_<T, U> : __basic_common_reference<T, U> {};

	template<class T, class U>
	struct __common_reference2 : __common_reference2_1_<T, U> {};

	template<class T, class U>
	requires std::is_reference_v<T> && std::is_reference_v<U> &&
		meta::Valid<__builtin_common_t, T, U> &&
		std::is_reference_v<__builtin_common_t<T, U>>
	struct __common_reference2<T, U> : __builtin_common<T, U> {};

	template<class T, class U>
	struct common_reference<T, U> : __common_reference2<T, U> {};

	template<class T, class U, class V, class... W>
	requires meta::Valid<common_reference_t, T, U>
	struct common_reference<T, U, V, W...>
	: common_reference<common_reference_t<T, U>, V, W...> {};

	///////////////////////////////////////////////////////////////////////////
	// CommonReference [concept.commonref]
	//
	template<class T, class U>
	META_CONCEPT CommonReference =
		requires {
			typename common_reference_t<T, U>;
			typename common_reference_t<U, T>;
		} &&
		Same<common_reference_t<T, U>, common_reference_t<U, T>> &&
		ConvertibleTo<T, common_reference_t<T, U>> &&
		ConvertibleTo<U, common_reference_t<T, U>>;

	///////////////////////////////////////////////////////////////////////////
	// Common [concept.common]
	//
	template<class T, class U>
	META_CONCEPT Common =
		requires {
			typename common_type_t<T, U>;
			typename common_type_t<U, T>;
			requires Same<common_type_t<T, U>, common_type_t<U, T>>;
			static_cast<common_type_t<T, U>>(std::declval<T>());
			static_cast<common_type_t<T, U>>(std::declval<U>());
		} &&
		CommonReference<
			std::add_lvalue_reference_t<const T>,
			std::add_lvalue_reference_t<const U>> &&
		CommonReference<
			std::add_lvalue_reference_t<common_type_t<T, U>>,
			common_reference_t<
				std::add_lvalue_reference_t<const T>,
				std::add_lvalue_reference_t<const U>>>;
} STL2_CLOSE_NAMESPACE

#endif
