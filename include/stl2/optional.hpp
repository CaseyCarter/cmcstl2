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
#ifndef STL2_OPTIONAL_HPP
#define STL2_OPTIONAL_HPP

#include <ostream>
#include <stdexcept>
#include <stl2/type_traits.hpp>
#include <stl2/variant.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/in_place.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// Implementation of N4529 optional
//
// TODO:
// * Update to N4606 + LWG2451
// * Exception safety audit
// * optional<T&>: Not forbidden since variant supports references, but I
//   need to ensure the semantics are reasonable.
//
STL2_OPEN_NAMESPACE {
	struct nullopt_t {
		explicit constexpr nullopt_t(int) {}
	};
	template <class = void>
	constexpr nullopt_t __nullopt{42};

	namespace {
		constexpr const nullopt_t& nullopt = __nullopt<>;
	}

	class bad_optional_access : public std::logic_error {
	public:
		bad_optional_access()
		: logic_error{"Attempt to access disengaged optional"} {}
	};

	namespace __optional {
		template <class = void>
		[[noreturn]] bool bad_access() {
			throw bad_optional_access{};
		}

		Destructible{T}
		class optional;

		struct access {
			template <_SpecializationOf<optional> O>
			static constexpr auto&& v(O&& o) noexcept {
				return __stl2::forward<O>(o).v_;
			}
			static constexpr decltype(auto) cv(const optional<auto>& o) noexcept {
				return o.v_;
			}
		};

		ext::ExplicitlyConvertibleTo{From, To}
		struct narrowing_converter {
			static_assert(is_reference<From>());
			From from_;

			constexpr operator To() &&
			STL2_NOEXCEPT_RETURN(
				static_cast<To>(std::forward<From>(from_))
			)
		};

		template <class To, class From>
		requires ext::ExplicitlyConvertibleTo<From, To>()
		constexpr auto allow_narrowing_conversion(From&& f) noexcept {
			return narrowing_converter<From&&, To>{__stl2::forward<From>(f)};
		}

		template <class T>
		struct convert_visitor {
			constexpr auto operator()(nullopt_t) const noexcept {
				return variant<nullopt_t, T>{nullopt};
			}
			template <ConvertibleTo<T> U>
			constexpr auto operator()(U&& u) const
			STL2_NOEXCEPT_RETURN(
				variant<nullopt_t, T>{emplaced_type<T>,
					allow_narrowing_conversion<T>(__stl2::forward<U>(u))}
			)
		};

		Destructible{T}
		class optional {
			friend struct access;
			variant<nullopt_t, T> v_ = nullopt;

			template <class U>
			requires Same<variant<nullopt_t, T>, decay_t<U>>()
			static constexpr decltype(auto) get_unchecked(U&& v) noexcept {
				STL2_EXPECT(__stl2::holds_alternative<T>(v));
				return __stl2::get_unchecked<T>(__stl2::forward<U>(v));
			}

			template <class U>
			requires Same<variant<nullopt_t, T>, decay_t<U>>()
			static constexpr decltype(auto) get(U&& v) {
				__stl2::holds_alternative<T>(v) || bad_access();
				return __stl2::get_unchecked<T>(__stl2::forward<U>(v));
			}

		public:
			using value_type = T;

			static_assert(!is_same<__uncvref<T>, nullopt_t>(),
				"optional cannot hold nullopt_t");
			static_assert(!is_same<__uncvref<T>, in_place_t>(),
				"optional cannot hold in_place_t");

			constexpr optional() noexcept = default;
			constexpr optional(nullopt_t) noexcept
			: optional() {}

			constexpr optional(const T& t)
			noexcept(is_nothrow_copy_constructible<T>::value)
			requires CopyConstructible<T>()
			: v_{t} {}
			constexpr optional(T&& t)
			noexcept(is_nothrow_move_constructible<T>::value)
			requires MoveConstructible<T>()
			: v_{__stl2::move(t)} {}

			template <class...Args>
			requires Constructible<T, Args...>()
			constexpr explicit optional(in_place_t, Args&&...args)
			noexcept(is_nothrow_constructible<T, Args...>::value)
			: v_{emplaced_type<T>, __stl2::forward<Args>(args)...} {}

			template <class U, class...Args>
			requires Constructible<T, std::initializer_list<U>&, Args...>()
			constexpr explicit optional(
				in_place_t, std::initializer_list<U> il, Args&&...args)
			noexcept(is_nothrow_constructible<T,
				std::initializer_list<U>&, Args...>::value)
			: v_{emplaced_type<T>, il, __stl2::forward<Args>(args)...} {}

			// Extensions: optional<U> converts to optional<T> if U converts to T.
			template <ConvertibleTo<T> U>
			requires Constructible<T, U>()
			constexpr optional(optional<U>&& that)
			noexcept(is_nothrow_constructible<T, U>::value)
			: v_{__stl2::visit(convert_visitor<T>{},
				access::v(__stl2::move(that)))}
			{}

			template <ConvertibleTo<T> U>
			explicit constexpr optional(optional<U>&& that)
			noexcept(is_nothrow_constructible<T, U>::value)
			: v_{__stl2::visit(convert_visitor<T>{},
				access::v(__stl2::move(that)))}
			{}

			template <ConvertibleTo<T> U>
			requires Constructible<T, const U&>()
			constexpr optional(const optional<U>& that)
			noexcept(is_nothrow_constructible<T, const U&>::value)
			: v_{__stl2::visit(convert_visitor<T>{}, access::cv(that))} {}

			template <ConvertibleTo<T> U>
			explicit constexpr optional(const optional<U>& that)
			noexcept(is_nothrow_constructible<T, const U&>::value)
			: v_{__stl2::visit(convert_visitor<T>{}, access::cv(that))} {}

			STL2_CONSTEXPR_EXT optional&
			operator=(nullopt_t) & noexcept {
				v_ = nullopt;
				return *this;
			}

			template <class U>
			requires Same<decay_t<U>, T>() && Constructible<T, U>() &&
				Assignable<T&, U>()
			optional& operator=(U&& u) & {
				try {
					v_ = __stl2::forward<U>(u);
				} catch(...) {
					v_ = nullopt;
					throw;
				}
				return *this;
			}

			template <class U>
			requires Same<decay_t<U>, T>() && Constructible<T, U>() &&
				Assignable<T&, U>() && is_nothrow_move_constructible<T>::value
			STL2_CONSTEXPR_EXT optional&
			operator=(U&& u) &
			noexcept(is_nothrow_assignable<T&, U>::value)
			{
				v_ = __stl2::forward<U>(u);
				return *this;
			}

			template <ConvertibleTo<T> U>
			STL2_CONSTEXPR_EXT optional&
			operator=(const optional<U>& that) &
			noexcept(is_nothrow_constructible<T, const U&>::value &&
				is_nothrow_assignable<T&, const U&>::value)
			{
				if (that) {
					if (*this) {
						**this = *that;
					} else {
						v_.template emplace<T>(*that);
					}
				} else {
					*this = {};
				}
				return *this;
			}

			template <ConvertibleTo<T> U>
			STL2_CONSTEXPR_EXT optional&
			operator=(optional<U>&& that) &
			noexcept(is_nothrow_constructible<T, U>::value &&
				is_nothrow_assignable<T&, U>::value)
			{
				if (that) {
					if (*this) {
						**this = __stl2::move(*that);
					} else {
						v_.template emplace<T>(__stl2::move(*that));
					}
				} else {
					*this = {};
				}
				return *this;
			}

			template <class...Args>
			requires Constructible<T, Args...>()
			STL2_CONSTEXPR_EXT void
			emplace(Args&&...args)
			noexcept(is_nothrow_constructible<T, Args...>::value)
			{ v_.template emplace<T>(__stl2::forward<Args>(args)...); }

			template <class U, class...Args>
			requires Constructible<T, std::initializer_list<U>&, Args...>()
			STL2_CONSTEXPR_EXT void
			emplace(std::initializer_list<U> il, Args&&...args)
			noexcept(is_nothrow_constructible<T,
				std::initializer_list<U>&, Args...>::value)
			{ v_.template emplace<T>(il, __stl2::forward<Args>(args)...); }

			template <class U>
			requires
				Swappable<T&, U&>() && Constructible<T, U&&>() &&
				Constructible<U, T&&>()
			STL2_CONSTEXPR_EXT void swap(optional<U>& that)
			noexcept(is_nothrow_move_constructible<T>::value &&
				is_nothrow_move_constructible<U>::value &&
				is_nothrow_swappable_v<T&, U&>)
			{
				if (*this) {
					if (that) {
						__stl2::swap(**this, *that);
					} else {
						access::v(that).template emplace<U>(__stl2::move(**this));
						*this = {};
					}
				} else {
					if (that) {
						v_.template emplace<T>(__stl2::move(*that));
						that = {};
					} else {
						// Nothing to do.
					}
				}
			}

			constexpr const T* operator->() const {
				return &optional::get_unchecked(v_);
			}
			constexpr T* operator->() {
				return &optional::get_unchecked(v_);
			}

			constexpr T const& operator*() const & noexcept {
				return optional::get_unchecked(v_);
			}
			constexpr T& operator*() & noexcept {
				return optional::get_unchecked(v_);
			}
			constexpr T&& operator*() && noexcept {
				return optional::get_unchecked(__stl2::move(v_));
			}
			constexpr const T&& operator*() const && {
				return optional::get_unchecked(__stl2::move(v_));
			}

			constexpr explicit operator bool() const noexcept {
				return !__stl2::holds_alternative<nullopt_t>(v_);
			}

			constexpr T const& value() const & {
				return optional::get(v_);
			}
			constexpr T& value() & {
				return optional::get(v_);
			}
			constexpr T&& value() && {
				return optional::get(__stl2::move(v_));
			}
			constexpr const T&& value() const && {
				return optional::get(__stl2::move(v_));
			}

			template <ext::ExplicitlyConvertibleTo<T> U>
			requires CopyConstructible<T>()
			constexpr T value_or(U&& u) const & {
				return *this
					? **this
					: static_cast<T>(__stl2::forward<U>(u));
			}
			template <ext::ExplicitlyConvertibleTo<T> U>
			requires CopyConstructible<T>()
			constexpr T value_or(U&& u) && {
				return *this
					? __stl2::move(**this)
					: static_cast<T>(__stl2::forward<U>(u));
			}
		};

		template <class T, class U>
		requires Swappable<T&, U&>()
		STL2_CONSTEXPR_EXT void swap(optional<T>& lhs, optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			lhs.swap(rhs)
		)
		template <class T>
		requires Swappable<T&>()
		STL2_CONSTEXPR_EXT void swap(optional<T>& lhs, optional<T>& rhs)
		STL2_NOEXCEPT_RETURN(
			lhs.swap(rhs)
		)

		struct eq_visitor {
			EqualityComparable{T, U}
			constexpr bool operator()(const T& l, const U& r) const
			STL2_NOEXCEPT_RETURN(
				l == r
			)
			constexpr bool operator()(const auto&, const auto&) const noexcept {
				return false;
			}
			constexpr bool operator()(nullopt_t, nullopt_t) const noexcept {
				return true;
			}
		};

		EqualityComparable{T, U}
		constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			__stl2::visit(eq_visitor{}, access::cv(lhs), access::cv(rhs))
		)
		EqualityComparable{T, U}
		constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			!(lhs == rhs)
		)

		struct lt_visitor {
			StrictTotallyOrdered{T, U}
			constexpr bool operator()(const T& l, const U& r) const
			STL2_NOEXCEPT_RETURN(
				l < r
			)
			constexpr bool operator()(const auto&, nullopt_t) const noexcept {
				return false;
			}
			constexpr bool operator()(nullopt_t, const auto&) const noexcept {
				return true;
			}
			constexpr bool operator()(nullopt_t, nullopt_t) const noexcept {
				return false;
			}
		};

		StrictTotallyOrdered{T, U}
		constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			__stl2::visit(lt_visitor{}, access::cv(lhs), access::cv(rhs))
		)

		StrictTotallyOrdered{T, U}
		constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			rhs < lhs
		)

		StrictTotallyOrdered{T, U}
		constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			!(rhs < lhs)
		)

		StrictTotallyOrdered{T, U}
		constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			!(lhs < rhs)
		)

		constexpr bool operator==(const optional<auto>& o, nullopt_t) noexcept {
			return !o;
		}
		constexpr bool operator==(nullopt_t, const optional<auto>& o) noexcept {
			return o == nullopt;
		}

		constexpr bool operator!=(const optional<auto>& o, nullopt_t) noexcept {
			return !(o == nullopt);
		}
		constexpr bool operator!=(nullopt_t, const optional<auto>& o) noexcept {
			return o != nullopt;
		}

		constexpr bool operator<(const optional<auto>&, nullopt_t) noexcept {
			return false;
		}
		constexpr bool operator<(nullopt_t, const optional<auto>& o) noexcept {
			return !(o == nullopt);
		}

		constexpr bool operator<=(const optional<auto>& o, nullopt_t) noexcept {
			return o == nullopt;
		}
		constexpr bool operator<=(nullopt_t, const optional<auto>&) noexcept {
			return true;
		}

		constexpr bool operator>(const optional<auto>& o, nullopt_t) noexcept {
			return o != nullopt;
		}
		constexpr bool operator>(nullopt_t, const optional<auto>&) noexcept {
			return false;
		}

		constexpr bool operator>=(const optional<auto>&, nullopt_t) noexcept {
			return true;
		}
		constexpr bool operator>=(nullopt_t, const optional<auto>& o) noexcept {
			return o == nullopt;
		}

		template <class T, class U>
		requires !_SpecializationOf<U, optional> && EqualityComparable<T, U>()
		constexpr bool operator==(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			o ? *o == u : false
		)
		template <class T, class U>
		requires !_SpecializationOf<T, optional> && EqualityComparable<T, U>()
		constexpr bool operator==(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o == t
		)

		template <class T, class U>
		requires !_SpecializationOf<U, optional> && EqualityComparable<T, U>()
		constexpr bool operator!=(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			!(o == u)
		)
		template <class T, class U>
		requires !_SpecializationOf<T, optional> && EqualityComparable<T, U>()
		constexpr bool operator!=(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			!(o == t)
		)

		template <class T, class U>
		requires !_SpecializationOf<U, optional> && StrictTotallyOrdered<T, U>()
		constexpr bool operator<(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			o ? *o < u : true
		)
		template <class T, class U>
		requires !_SpecializationOf<T, optional> && StrictTotallyOrdered<T, U>()
		constexpr bool operator<(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o ? t < *o : false
		)

		template <class T, class U>
		requires !_SpecializationOf<U, optional> && StrictTotallyOrdered<T, U>()
		constexpr bool operator>(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			u < o
		)
		template <class T, class U>
		requires !_SpecializationOf<T, optional> && StrictTotallyOrdered<T, U>()
		constexpr bool operator>(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o < t
		)

		template <class T, class U>
		requires !_SpecializationOf<U, optional> && StrictTotallyOrdered<T, U>()
		constexpr bool operator<=(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			!(u < o)
		)
		template <class T, class U>
		requires !_SpecializationOf<T, optional> && StrictTotallyOrdered<T, U>()
		constexpr bool operator<=(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			!(o < t)
		)

		template <class T, class U>
		requires !_SpecializationOf<U, optional> && StrictTotallyOrdered<T, U>()
		constexpr bool operator>=(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			!(o < u)
		)
		template <class T, class U>
		requires !_SpecializationOf<T, optional> && StrictTotallyOrdered<T, U>()
		constexpr bool operator>=(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			!(t < o)
		)
	} // namespace __optional

	using __optional::optional;

	template <class T>
	constexpr optional<decay_t<T>> make_optional(T&& t)
	noexcept(is_nothrow_constructible<optional<decay_t<T>>, T>::value)
	{ return optional<decay_t<T>>(__stl2::forward<T>(t)); }

	Common{T, U}
	struct common_type<optional<T>, optional<U>> {
		using type = optional<common_type_t<T, U>>;
	};
} STL2_CLOSE_NAMESPACE

namespace std {
	::__stl2::ext::Hashable{T}
	struct hash<::__stl2::optional<T>> {
		using result_type = size_t;
		using argument_type = ::__stl2::optional<T>;

		constexpr size_t operator()(const argument_type& o) const {
			if (o) {
				size_t seed = 0;
				::__stl2::ext::hash_combine(seed, *o);
				return seed;
			} else {
				return 42;
			}
		}
	};
}

#endif
