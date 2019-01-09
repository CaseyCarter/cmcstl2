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
	struct common_type<T> : common_type<T, T> {};

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
	requires requires { typename common_type_t<T, U>; }
	struct common_type<T, U, V, W...>
	: common_type<common_type_t<T, U>, V, W...> {};

	////////////////////////////////////////////////////////////////////////////
	// basic_common_reference [meta.trans.other]
	//
	template<class, class, template<class> class, template<class> class>
	struct basic_common_reference {};

	////////////////////////////////////////////////////////////////////////////
	// common_reference [meta.trans.other]
	//
	template<class From>
	struct __copy_cv_ {
		static_assert(!std::is_reference_v<From>);
		template<class To> using invoke = To;
	};
	template<class From>
	struct __copy_cv_<const From> {
		template<class To> using invoke = const To;
	};
	template<class From>
	struct __copy_cv_<volatile From> {
		template<class To> using invoke = volatile To;
	};
	template<class From>
	struct __copy_cv_<const volatile From> {
		template<class To> using invoke = const volatile To;
	};
	template<class From, class To>
	using __copy_cv = meta::invoke<__copy_cv_<From>, To>;

	// CREF [meta.trans.other]/2.1
	template<class T>
	using __cref = std::add_lvalue_reference_t<const std::remove_reference_t<T>>;

	// COMMON_REF [meta.trans.other]/2
	template<class T, class U>
	struct __common_ref_ {
		static_assert(std::is_reference_v<T>);
		static_assert(std::is_reference_v<U>);
	};

	template<class T, class U>
	using __common_ref = meta::_t<__common_ref_<T, U>>;

	// [meta.trans.other]/2.5
	template<class T, class U>
	using __lref_res = __cond_res<__copy_cv<T, U> &, __copy_cv<U, T> &>;

	template<class T, class U>
	requires requires { typename __lref_res<T, U>; } &&
		std::is_reference_v<__lref_res<T, U>>
	struct __common_ref_<T&, U&> {
		using type = __lref_res<T, U>;
	};

	// [meta.trans.other]/2.6
	template<class T, class U, class R = __common_ref<T&, U&>>
	using __rref_res = std::remove_reference_t<R>&&;

	template<class T, class U>
	requires meta::Valid<__common_ref, T&, U&> &&
		ConvertibleTo<T&&, __rref_res<T, U>> &&
		ConvertibleTo<U&&, __rref_res<T, U>>
	struct __common_ref_<T&&, U&&> {
		using type = __rref_res<T, U>;
	};

	// [meta.trans.other]/2.7
	template<class T, class U>
	requires requires { typename __common_ref<const T&, U&>; } &&
		ConvertibleTo<T&&, __common_ref<const T&, U&>>
	struct __common_ref_<T&&, U&> {
		using type = __common_ref<const T&, U&>;
	};

	// [meta.trans.other]/2.8
	template<class T, class U>
	requires requires { typename __common_ref<T&, const U&>; } &&
		ConvertibleTo<U&&, __common_ref<T&, const U&>>
	struct __common_ref_<T&, U&&> {
		using type = __common_ref<T&, const U&>;
	};

	template<class>
	struct __xref {
		template<class U> using invoke = U;
	};
	template<class T>
	struct __xref<const T> {
		template<class U> using invoke = const U;
	};
	template<class T>
	struct __xref<volatile T> {
		template<class U> using invoke = volatile U;
	};
	template<class T>
	struct __xref<const volatile T> {
		template<class U> using invoke = const volatile U;
	};
	template<class T>
	struct __xref<T&> {
		template<class U> using invoke =
			std::add_lvalue_reference_t<meta::invoke<__xref<T>, U>>;
	};
	template<class T>
	struct __xref<T&&> {
		template<class U> using invoke =
			std::add_rvalue_reference_t<meta::invoke<__xref<T>, U>>;
	};

	template<class T, class U>
	using __basic_common_reference_t =
		meta::_t<basic_common_reference<__uncvref<T>, __uncvref<U>,
			__xref<T>::template invoke, __xref<U>::template invoke>>;

	template<class...>
	struct common_reference {};

	template<class... Ts>
	using common_reference_t = meta::_t<common_reference<Ts...>>;

	// [meta.trans.other]/5.2
	template<class T>
	struct common_reference<T> {
		using type = T;
	};

	// [meta.trans.other]/5.3.4
	template<class T, class U>
	struct __common_reference3 : common_type<T, U> {};

	// [meta.trans.other]/5.3.3
	template<class T, class U>
	requires requires { typename __cond_res<T, U>; }
	struct __common_reference3<T, U> {
		using type = __cond_res<T, U>;
	};

	template<class T, class U>
	struct __common_reference2 : __common_reference3<T, U> {};

	// [meta.trans.other]/5.3.2
	template<class T, class U>
	requires requires { typename __basic_common_reference_t<T, U>; }
	struct __common_reference2<T, U> {
		using type = __basic_common_reference_t<T, U>;
	};

	template<class T, class U>
	struct common_reference<T, U> : __common_reference2<T, U> {};

	// [meta.trans.other]/5.3.1
	template<class T, class U>
	requires std::is_reference_v<T> && std::is_reference_v<U> &&
		requires { typename __common_ref<T, U>; }
	struct common_reference<T, U> {
		using type = __common_ref<T, U>;
	};

	// [meta.trans.other]/5.4
	template<class T, class U, class V, class... W>
	requires requires { typename common_reference_t<T, U>; }
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
