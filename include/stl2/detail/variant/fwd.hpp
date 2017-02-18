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
#ifndef STL2_DETAIL_VARIANT_FWD_HPP
#define STL2_DETAIL_VARIANT_FWD_HPP

#include <stl2/functional.hpp>
#include <stl2/detail/utility/in_place.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

#define __cpp_lib_variant 20150524

STL2_OPEN_NAMESPACE {
	struct monostate {};

	constexpr bool operator==(monostate, monostate)
	{ return true; }
	constexpr bool operator!=(monostate, monostate)
	{ return false; }
	constexpr bool operator<(monostate, monostate)
	{ return false; }
	constexpr bool operator>(monostate, monostate)
	{ return false; }
	constexpr bool operator<=(monostate, monostate)
	{ return true; }
	constexpr bool operator>=(monostate, monostate)
	{ return true; }

	namespace __variant {
		struct void_storage : monostate {
			void_storage() requires false;
		private:
			// void_storage must have at least one constexpr constructor
			// to be a literal type.
			struct hack_tag {};
			constexpr void_storage(hack_tag) {}
		};

		// Map from parameter type to actual type stored; wrap references,
		// replace voids.
		template <class T>
		struct element_storage_ {
			using type = T; // remove_cv_t<T>?
		};
		template <_Is<is_reference> T>
		struct element_storage_<T> {
			using type = reference_wrapper<remove_reference_t<T>>;
		};
		template <>
		struct element_storage_<void> {
			using type = void_storage;
		};
		template <class T>
		using element_t = meta::_t<element_storage_<T>>;

		// Convert a reference to a stored element to the
		// proper type, i.e., dereference reference_wrappers.
		template <_IsNot<is_const> T>
		constexpr remove_reference_t<T>&
		cook(element_t<meta::id_t<T>>& t) noexcept {
			return t;
		}

		template <class T>
		constexpr const remove_reference_t<T>&
		cook(const element_t<meta::id_t<T>>& t) noexcept {
			return t;
		}

		template <class T>
		constexpr T&& cook(element_t<meta::id_t<T>>&& t) noexcept {
			return __stl2::move(cook<T>(t));
		}

		// Obtain the index of T in Types; substitution failure if T
		// does not appear exactly once.
		template <class T, class Types,
			std::size_t I = meta::_v<meta::find_index<Types, T>>>
		requires
			I != meta::_v<meta::npos> &&
			Same<meta::find_index<meta::drop_c<Types, I + 1>, T>, meta::npos>()
		constexpr std::size_t index_of_type = I;

		template <class...Ts>
		requires (Destructible<element_t<Ts>>() && ...)
		class base;

		// VariantTypes<T> is a list of the alternative types of T if
		// T is derived from __variant::base.
		template <class...Types>
		meta::list<Types...> types_(base<Types...>& b); // not defined

		template <class T>
		using VariantTypes =
			decltype(types_(declval<__uncvref<T>&>()));

		// Satisfied iff __uncvref<T> is derived from a
		// specialization of __variant::base
		template <class T>
		concept bool Variant = requires { typename VariantTypes<T>; };

		struct v_access {
			template <std::size_t I, Variant V,
				_IsNot<is_void> T = meta::at_c<VariantTypes<V>, I>>
			static constexpr auto&& raw_get(in_place_index_t<I> i, V&& v) noexcept;

			template <std::size_t I, Variant V,
				_IsNot<is_void> T = meta::at_c<VariantTypes<V>, I>>
			static constexpr auto&& cooked_get(in_place_index_t<I> i, V&& v) noexcept;
		};
	} // namespace __variant

	template <class...Ts>
	requires (Destructible<__variant::element_t<Ts>>() && ...)
	class variant;

	class bad_variant_access : public std::logic_error {
	public:
		using std::logic_error::logic_error;
		bad_variant_access() :
			std::logic_error{"Attempt to access invalid variant alternative"} {}
	};

	template <class T, __variant::Variant V,
		std::size_t I = __variant::index_of_type<T, __variant::VariantTypes<V>>>
	constexpr bool holds_alternative(const V& v) noexcept {
		return v.index() == I;
	}
} STL2_CLOSE_NAMESPACE

namespace std {
	template <>
	struct hash<::__stl2::monostate> {
		using result_type = size_t;
		using argument_type = ::__stl2::monostate;

		constexpr size_t operator()(::__stl2::monostate) const {
			// https://xkcd.com/221/
			return 4;
		}
	};
} // namespace std

#endif
