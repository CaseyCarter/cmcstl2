// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-2016
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
#include <utility>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/hash.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/smf_control.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/memory/addressof.hpp>

///////////////////////////////////////////////////////////////////////////
// Implementation of C++17 optional
//
STL2_OPEN_NAMESPACE {
	struct nullopt_t {
		struct __secret_tag {};
		explicit constexpr nullopt_t(__secret_tag) {}
	};

#if defined(__cpp_inline_variables)
	inline constexpr nullopt_t nullopt{nullopt_t::__secret_tag{}};
#else
	template <class>
	extern constexpr nullopt_t __nullopt{nullopt_t::__secret_tag{}};

	namespace {
		constexpr const nullopt_t& nullopt = __nullopt<void>;
	}
#endif

	class bad_optional_access : public std::logic_error {
	public:
		bad_optional_access()
		: logic_error{"Attempt to access disengaged optional"} {}
	};

	ext::DestructibleObject{T} class optional;

	namespace __optional {
		template <class = void>
		[[noreturn]] void bad_access() {
			throw bad_optional_access{};
		}

		template <class T, class U>
		requires
			SwappableWith<T&, U&> &&
			Constructible<T, U> &&
			Constructible<U, T>
		void swap(optional<T>& lhs, optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			lhs.swap(rhs)
		)
		template <class T>
		requires
			Swappable<T> &&
			MoveConstructible<T>
		void swap(optional<T>& lhs, optional<T>& rhs)
		STL2_NOEXCEPT_RETURN(
			lhs.swap(rhs)
		)

		template <class T>
		requires Destructible<T>
		class storage_destruct_layer {
		public:
			~storage_destruct_layer() { if (engaged_) clear(); }

			constexpr storage_destruct_layer() noexcept {}
			template <class... Args>
			requires Constructible<T, Args...>
			constexpr explicit storage_destruct_layer(std::in_place_t, Args&&... args)
			noexcept(std::is_nothrow_constructible<T, Args...>::value)
			: item_(std::forward<Args>(args)...), engaged_{true} {}

			void clear() noexcept {
				STL2_EXPECT(engaged_);
				item_.~T();
			}
		protected:
			union {
				char dummy_ = {};
				T item_;
			};
			bool engaged_ = false;
		};

		template <class T>
		requires
			Destructible<T> &&
			std::is_trivially_destructible<T>::value
		class storage_destruct_layer<T> {
		public:
			constexpr storage_destruct_layer() noexcept {}
			template <class... Args>
			requires Constructible<T, Args...>
			constexpr explicit storage_destruct_layer(std::in_place_t, Args&&... args)
			noexcept(std::is_nothrow_constructible<T, Args...>::value)
			: item_(std::forward<Args>(args)...), engaged_{true} {}

			void clear() noexcept {
				STL2_EXPECT(engaged_);
			}
		protected:
			union {
				char dummy_ = {};
				T item_;
			};
			bool engaged_ = false;
		};

		template <class T>
		class storage_construct_layer : public storage_destruct_layer<T> {
		public:
			using storage_destruct_layer<T>::storage_destruct_layer;

			template <class... Args>
			requires Constructible<T, Args...>
			void construct(Args&&... args)
			noexcept(std::is_nothrow_constructible<T, Args...>::value)
			{
				const volatile void* as_void = detail::addressof(this->item_);
				::new(const_cast<void*>(as_void)) T{std::forward<Args>(args)...};
				this->engaged_ = true;
			}

			void reset() noexcept {
				if (this->engaged_) {
					this->clear();
				}
				this->engaged_ = false;
			}

			constexpr bool has_value() const noexcept { return this->engaged_; }
			constexpr explicit operator bool() const noexcept { return this->engaged_; }

			constexpr T& operator*() & noexcept {
				STL2_EXPECT(has_value());
				return this->item_;
			}
			constexpr const T& operator*() const& noexcept {
				STL2_EXPECT(has_value());
				return this->item_;
			}
			constexpr T&& operator*() && noexcept {
				STL2_EXPECT(has_value());
				return std::move(this->item_);
			}
			constexpr const T&& operator*() const&& noexcept {
				STL2_EXPECT(has_value());
				return std::move(this->item_);
			}
		};

		template <class T>
		class smf_layer : public storage_construct_layer<T> {
		public:
			using storage_construct_layer<T>::storage_construct_layer;

			smf_layer() = default;

			smf_layer(const smf_layer& that)
			noexcept(std::is_nothrow_copy_constructible<T>::value)
			{ construct_from(that); }
			smf_layer(smf_layer&& that)
			noexcept(std::is_nothrow_copy_constructible<T>::value)
			{ construct_from(std::move(that)); }

			smf_layer& operator=(const smf_layer& that) &
			noexcept(std::is_nothrow_copy_constructible<T>::value &&
				std::is_nothrow_copy_assignable<T>::value)
			{ return assign(that); }
			smf_layer& operator=(smf_layer&& that) &
			noexcept(std::is_nothrow_move_constructible<T>::value &&
				std::is_nothrow_move_assignable<T>::value)
			{ return assign(std::move(that)); }
		private:
			template <class That>
			requires Same<smf_layer, __uncvref<That>>
			void construct_from(That&& that) &
			noexcept(std::is_nothrow_constructible<T, That>::value)
			{
				if (that.has_value()) {
					this->construct(*std::forward<That>(that));
				}
			}
			template <class That>
			requires Same<smf_layer, __uncvref<That>>
			smf_layer& assign(That&& that) &
			noexcept(std::is_nothrow_constructible<T, That>::value &&
				std::is_nothrow_assignable<T&, That>::value)
			{
				if (this->has_value()) {
					if (that.has_value()) {
						**this = *std::forward<That>(that);
					} else {
						this->reset();
					}
				} else {
					construct_from(std::forward<That>(that));
				}
				return *this;
			}
		};

		template <class T>
		requires std::is_trivially_copyable<T>::value
		struct smf_layer<T> : storage_construct_layer<T> {
			using storage_construct_layer<T>::storage_construct_layer;
		};
	} // namespace __optional

	namespace ext {
		template <class> struct optional_storage {};

		template <class T>
		requires Destructible<T>
		struct optional_storage<T> {
			using type = __optional::smf_layer<T>;
		};
	} // namespace ext

	ext::DestructibleObject{T}
	class optional
	: public meta::_t<ext::optional_storage<T>>
	, detail::smf_control::copy<CopyConstructible<T>>
	, detail::smf_control::move<MoveConstructible<T>>
	, detail::smf_control::copy_assign<Copyable<T>>
	, detail::smf_control::move_assign<Movable<T>>
	{
		using base_t = meta::_t<ext::optional_storage<T>>;

		template <class U>
		static constexpr bool should_unwrap_construct =
			!(Constructible<T, optional<U>&> ||
			  Constructible<T, optional<U>&&> ||
			  Constructible<T, const optional<U>&> ||
			  Constructible<T, const optional<U>&&> ||
			  ConvertibleTo<optional<U>&, T> ||
			  ConvertibleTo<optional<U>&&, T> ||
			  ConvertibleTo<const optional<U>&, T> ||
			  ConvertibleTo<const optional<U>&&, T>);

		template <class U>
		static constexpr bool should_unwrap_assign =
			should_unwrap_construct<U> &&
			!(Assignable<T&, optional<U>&> ||
			  Assignable<T&, optional<U>&&> ||
			  Assignable<T&, const optional<U>&> ||
			  Assignable<T&, const optional<U>&&>);

	public:
		static_assert(!Same<__uncvref<T>, nullopt_t>,
			"optional cannot hold nullopt_t");
		static_assert(!Same<__uncvref<T>, std::in_place_t>,
			"optional cannot hold std::in_place_t");

		using value_type = T;

		constexpr optional() noexcept {}
		constexpr optional(nullopt_t) noexcept
		: optional{} {}

		optional(const optional&) = default;
		optional(optional&&) = default;

		template <class... Args>
		requires Constructible<T, Args...>
		constexpr explicit optional(std::in_place_t, Args&&... args)
		: base_t{std::in_place, std::forward<Args>(args)...} {}
		template <class E, class... Args>
		requires Constructible<T, std::initializer_list<E>&, Args...>
		constexpr explicit optional(std::in_place_t, std::initializer_list<E> il, Args&&... args)
		: base_t{std::in_place, il, std::forward<Args>(args)...} {}
		template <class U = T>
		requires
			!Same<U, std::in_place_t> &&
			!Same<optional, std::decay_t<U>> &&
			Constructible<T, U>
		constexpr explicit optional(U&& u)
		noexcept(std::is_nothrow_constructible<T, U>::value)
		: base_t{std::in_place, std::forward<U>(u)} {}
		template <class U = T>
		requires
			!Same<U, std::in_place_t> &&
			!Same<optional, std::decay_t<U>> &&
			Constructible<T, U> &&
			ConvertibleTo<U, T>
		constexpr optional(U&& u)
		noexcept(std::is_nothrow_constructible<T, U>::value)
		: base_t{std::in_place, std::forward<U>(u)} {}
		template <class U>
		requires
			Constructible<T, const U&> &&
			should_unwrap_construct<U>
		explicit optional(const optional<U>& that)
		noexcept(std::is_nothrow_constructible<T, const U&>::value)
		{ if (that) this->construct(*that); }
		template <class U>
		requires
			Constructible<T, const U&> &&
			should_unwrap_construct<U> &&
			ConvertibleTo<U, T>
		optional(const optional<U>& that)
		noexcept(std::is_nothrow_constructible<T, const U&>::value)
		{ if (that) this->construct(*that); }

		template <class U>
		requires
			Constructible<T, U>
		explicit optional(optional<U>&& that)
		noexcept(std::is_nothrow_constructible<T, U>::value)
		{ if (that) this->construct(std::move(*that)); }
		template <class U>
		requires
			Constructible<T, U> &&
			ConvertibleTo<U, T>
		optional(optional<U>&& that)
		noexcept(std::is_nothrow_constructible<T, U>::value)
		{ if (that) this->construct(std::move(*that)); }

		~optional() = default;

		optional& operator=(nullopt_t) & noexcept {
			this->reset();
			return *this;
		}
		optional& operator=(const optional&) & = default;
		optional& operator=(optional&&) & = default;

		template <class U = T>
		requires
			!Same<optional, std::decay_t<U>> &&
			!(std::is_scalar<T>::value && Same<T, std::decay_t<U>>) &&
			Constructible<T, U> &&
			Assignable<T&, U>
		optional& operator=(U&& u) &
		noexcept(std::is_nothrow_assignable<T&, U>::value &&
			std::is_nothrow_constructible<T, U>::value)
		{
			if (*this) {
				**this = std::forward<U>(u);
			} else {
				this->construct(std::forward<U>(u));
			}
			return *this;
		}

		template <class U>
		requires
			Constructible<T, const U&> &&
			Assignable<T, const U&> &&
			should_unwrap_assign<U>
		optional& operator=(const optional<U>& that) &
		noexcept(std::is_nothrow_constructible<T, const U&>::value &&
			std::is_nothrow_assignable<T&, const U&>::value)
		{
			if (that) {
				if (*this) {
					**this = *that;
				} else {
					this->construct(*that);
				}
			} else {
				this->reset();
			}
			return *this;
		}

		template <class U>
		requires
			Constructible<T, U> &&
			Assignable<T, U> &&
			should_unwrap_assign<U>
		optional& operator=(optional<U>&& that) &
		noexcept(std::is_nothrow_constructible<T, U>::value &&
			std::is_nothrow_assignable<T&, U>::value)
		{
			if (that) {
				if (*this) {
					**this = std::move(*that);
				} else {
					this->construct(std::move(*that));
				}
			} else {
				this->reset();
			}
			return *this;
		}

		template <class... Args>
		requires Constructible<T, Args...>
		void emplace(Args&&... args)
		noexcept(std::is_nothrow_constructible<T, Args...>::value)
		{
			this->reset();
			this->construct(std::forward<Args>(args)...);
		}
		template <class U, class... Args>
		requires Constructible<T, std::initializer_list<U>&, Args...>
		void emplace(std::initializer_list<U> il, Args&&... args)
		noexcept(std::is_nothrow_constructible<T,
			std::initializer_list<U>&, Args...>::value)
		{
			this->reset();
			this->construct(il, std::forward<Args>(args)...);
		}

		template <class U>
		requires
			SwappableWith<T&, U&> &&
			Constructible<T, U> &&
			Constructible<U, T>
		void swap(optional<U>& that)
		noexcept(is_nothrow_swappable_v<T&, U&> &&
			std::is_nothrow_constructible<T, U>::value &&
			std::is_nothrow_constructible<U, T>::value)
		{
			if (*this) {
				if (that) {
					__stl2::swap(**this, *that);
				} else {
					that.construct(std::move(**this));
					this->reset();
				}
			} else {
				if (that) {
					this->construct(std::move(*that));
					that.reset();
				} else {
					// Nothing to do.
				}
			}
		}

		constexpr const T* operator->() const {
			return detail::addressof(**this);
		}
		constexpr T* operator->() {
			return detail::addressof(**this);
		}

		using base_t::operator*;

		using base_t::operator bool;
		using base_t::has_value;

		constexpr T const& value() const & {
			if (!*this) __optional::bad_access();
			return **this;
		}
		constexpr T& value() & {
			if (!*this) __optional::bad_access();
			return **this;
		}
		constexpr T&& value() && {
			if (!*this) __optional::bad_access();
			return std::move(**this);
		}
		constexpr const T&& value() const && {
			if (!*this) __optional::bad_access();
			return std::move(**this);
		}

		template <ConvertibleTo<T> U>
		requires
			ConvertibleTo<U, T> &&
			CopyConstructible<T>
		constexpr T value_or(U&& u) const & {
			return *this
				? **this
				: static_cast<T>(std::forward<U>(u));
		}
		template <class U>
		requires
			ConvertibleTo<U, T> &&
			MoveConstructible<T>
		constexpr T value_or(U&& u) && {
			return *this
				? std::move(**this)
				: static_cast<T>(std::forward<U>(u));
		}

		using base_t::reset;
	};

	namespace __optional {
		template <class T, class U>
		concept bool can_eq =
			requires(const T& t, const U& u) {
				{ t == u } -> Boolean;
			};

		template <class T, class U>
		concept bool can_neq =
			requires(const T& t, const U& u) {
				{ t != u } -> Boolean;
			};

		template <class T, class U>
		concept bool can_lt =
			requires(const T& t, const U& u) {
				{ t < u } -> Boolean;
			};

		template <class T, class U>
		concept bool can_gt =
			requires(const T& t, const U& u) {
				{ t > u } -> Boolean;
			};

		template <class T, class U>
		concept bool can_lte =
			requires(const T& t, const U& u) {
				{ t <= u } -> Boolean;
			};

		template <class T, class U>
		concept bool can_gte =
			requires(const T& t, const U& u) {
				{ t >= u } -> Boolean;
			};

		template <class T, class U>
		requires can_eq<T, U>
		constexpr bool operator==(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			bool(lhs) == bool(rhs) && (!lhs || *lhs == *rhs)
		)
		template <class T, class U>
		requires can_neq<T, U>
		constexpr bool operator!=(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			bool(lhs) != bool(rhs) || (lhs && *lhs != *rhs)
		)

		template <class T, class U>
		requires can_lt<T, U>
		constexpr bool operator<(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			rhs && (!lhs || *lhs < *rhs)
		)

		template <class T, class U>
		requires can_gt<T, U>
		constexpr bool operator>(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			lhs && (!rhs || *lhs > *rhs)
		)

		template <class T, class U>
		requires can_lte<T, U>
		constexpr bool operator<=(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			!lhs || (rhs && *lhs <= *rhs)
		)

		template <class T, class U>
		requires can_gte<T, U>
		constexpr bool operator>=(const optional<T>& lhs, const optional<U>& rhs)
		STL2_NOEXCEPT_RETURN(
			!rhs || (lhs && *lhs >= *rhs)
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
		requires
			!_SpecializationOf<U, optional> &&
			can_eq<T, U>
		constexpr bool operator==(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			o ? bool(*o == u) : false
		)
		template <class T, class U>
		requires
			!_SpecializationOf<T, optional> &&
			can_eq<T, U>
		constexpr bool operator==(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o ? bool(t == *o) : false
		)

		template <class T, class U>
		requires
			!_SpecializationOf<U, optional> &&
			can_neq<T, U>
		constexpr bool operator!=(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			o ? bool(*o != u) : true
		)
		template <class T, class U>
		requires
			!_SpecializationOf<T, optional> &&
			can_neq<T, U>
		constexpr bool operator!=(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o ? bool(t != *o) : true
		)

		template <class T, class U>
		requires
			!_SpecializationOf<U, optional> &&
			can_lt<T, U>
		constexpr bool operator<(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			o ? bool(*o < u) : true
		)
		template <class T, class U>
		requires
			!_SpecializationOf<T, optional> &&
			can_lt<T, U>
		constexpr bool operator<(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o ? bool(t < *o) : false
		)

		template <class T, class U>
		requires
			!_SpecializationOf<U, optional> &&
			can_gt<T, U>
		constexpr bool operator>(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			o ? bool(*o > u) : false
		)
		template <class T, class U>
		requires
			!_SpecializationOf<T, optional> &&
			can_gt<T, U>
		constexpr bool operator>(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o ? bool(t > *o) : true
		)

		template <class T, class U>
		requires
			!_SpecializationOf<U, optional> &&
			can_lte<T, U>
		constexpr bool operator<=(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			o ? bool(*o <= u) : true
		)
		template <class T, class U>
		requires
			!_SpecializationOf<T, optional> &&
			can_lte<T, U>
		constexpr bool operator<=(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o ? bool(t <= *o) : false
		)

		template <class T, class U>
		requires
			!_SpecializationOf<U, optional> &&
			can_gte<T, U>
		constexpr bool operator>=(const optional<T>& o, const U& u)
		STL2_NOEXCEPT_RETURN(
			o ? bool(*o >= u) : false
		)
		template <class T, class U>
		requires
			!_SpecializationOf<T, optional> &&
			can_gte<T, U>
		constexpr bool operator>=(const T& t, const optional<U>& o)
		STL2_NOEXCEPT_RETURN(
			o ? bool(t >= *o) : true
		)
	} // namespace __optional

	template <class T, class... Args>
	constexpr optional<T> make_optional(Args&&... args)
	noexcept(std::is_nothrow_constructible<optional<T>, Args...>::value)
	{ return optional<T>{std::in_place, std::forward<Args>(args)...}; }
	template <class T, class E, class... Args>
	constexpr optional<T> make_optional(std::initializer_list<E> il, Args&&... args)
	noexcept(std::is_nothrow_constructible<optional<T>, std::initializer_list<E>&, Args...>::value)
	{ return optional<T>{std::in_place, il, std::forward<Args>(args)...}; }

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

		constexpr size_t operator()(const ::__stl2::optional<T>& o) const {
			if (o) {
				return hash<T>{}(*o);
			} else {
				return 42;
			}
		}
	};
}

#endif
