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
#ifndef STL2_DETAIL_META_HPP
#define STL2_DETAIL_META_HPP

#include <type_traits>
#include <meta/meta.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// <type_traits> imports
	//
	using std::integral_constant;
	using std::bool_constant;
	using std::true_type;
	using std::false_type;

	using std::is_void;
	using std::is_null_pointer;
	using std::is_integral;
	using std::is_floating_point;
	using std::is_array;
	using std::is_pointer;
	using std::is_lvalue_reference;
	using std::is_rvalue_reference;
	using std::is_member_object_pointer;
	using std::is_member_function_pointer;
	using std::is_enum;
	using std::is_union;
	using std::is_class;
	using std::is_function;

	using std::is_reference;
	using std::is_arithmetic;
	using std::is_fundamental;
	using std::is_object;
	using std::is_scalar;
	using std::is_compound;
	using std::is_member_pointer;

	using std::is_const;
	using std::is_volatile;
	using std::is_trivial;
	using std::is_trivially_copyable;
	using std::is_standard_layout;
	using std::is_pod;
	using std::is_literal_type;
	using std::is_empty;
	using std::is_polymorphic;
	using std::is_abstract;
	using std::is_final;

	using std::is_signed;
	using std::is_unsigned;

	using std::is_destructible;
	using std::is_trivially_destructible;
	using std::is_nothrow_destructible;

	using std::is_constructible;
	using std::is_trivially_constructible;
	using std::is_nothrow_constructible;

	using std::is_default_constructible;
	using std::is_trivially_default_constructible;
	using std::is_nothrow_default_constructible;

	using std::is_copy_constructible;
	using std::is_trivially_copy_constructible;
	using std::is_nothrow_copy_constructible;

	using std::is_move_constructible;
	using std::is_trivially_move_constructible;
	using std::is_nothrow_move_constructible;

	using std::is_assignable;
	using std::is_trivially_assignable;
	using std::is_nothrow_assignable;

	using std::is_copy_assignable;
	using std::is_trivially_copy_assignable;
	using std::is_nothrow_copy_assignable;

	using std::is_move_assignable;
	using std::is_trivially_move_assignable;
	using std::is_nothrow_move_assignable;

	using std::has_virtual_destructor;

	using std::alignment_of;
	using std::rank;
	using std::extent;

	using std::is_same;
	using std::is_base_of;
	using std::is_convertible;

	using std::remove_const;
	using std::remove_const_t;
	using std::remove_volatile;
	using std::remove_volatile_t;
	using std::remove_cv;
	using std::remove_cv_t;
	using std::add_const;
	using std::add_const_t;
	using std::add_volatile;
	using std::add_volatile_t;
	using std::add_cv;
	using std::add_cv_t;

	using std::remove_reference;
	using std::remove_reference_t;
	using std::add_lvalue_reference;
	using std::add_lvalue_reference_t;
	using std::add_rvalue_reference;
	using std::add_rvalue_reference_t;

	using std::make_signed;
	using std::make_signed_t;
	using std::make_unsigned;
	using std::make_unsigned_t;

	using std::remove_extent;
	using std::remove_extent_t;
	using std::remove_all_extents;
	using std::remove_all_extents_t;

	using std::remove_pointer;
	using std::remove_pointer_t;
	using std::add_pointer;
	using std::add_pointer_t;

	using std::aligned_storage;
	using std::aligned_storage_t;
	using std::aligned_union;
	using std::aligned_union_t;
	using std::decay;
	using std::decay_t;
	using std::enable_if;
	using std::enable_if_t;
	using std::conditional;
	using std::conditional_t;
	using std::underlying_type;
	using std::underlying_type_t;
	using std::result_of;
	using std::result_of_t;
	using std::void_t;

	template <class T>
	using __uncvref = remove_cv_t<remove_reference_t<T>>;

	namespace detail {
		template <class>
		struct __as_integer_sequence {};
		template <class T, T...Is>
		struct __as_integer_sequence<meta::list<std::integral_constant<T, Is>...>> {
			using type = std::integer_sequence<T, Is...>;
		};
	}
	template <class T>
	using __as_integer_sequence = meta::_t<detail::__as_integer_sequence<T>>;

	template <bool IsConst, class T>
	using __maybe_const = meta::if_c<IsConst, T const, T>;
} STL2_CLOSE_NAMESPACE

#endif
