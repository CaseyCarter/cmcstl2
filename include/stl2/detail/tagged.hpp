// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015, 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_TAGGED_HPP
#define STL2_DETAIL_TAGGED_HPP

#include <utility>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/tuple_like.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

///////////////////////////////////////////////////////////////////////////
// Tagged tuple-like types [taggedtup]
//
STL2_OPEN_NAMESPACE {
	namespace __tagged {
		template <class T>
		struct properties {};
		template <class Spec, class Arg>
		struct properties<Spec(Arg)> {
			using specifier = Spec;
			using type = Arg;
		};
		template <class T>
		using specifier = typename properties<T>::specifier;

		template <class T>
		using element = meta::_t<properties<T>>;

		template <class Untagged>
		struct base : Untagged {
			using Untagged::Untagged;
			base() = default;
			base(const base&) = default;
			base(base&&) = default;
			constexpr base(Untagged&& that)
			noexcept(std::is_nothrow_move_constructible<Untagged>::value)
			requires MoveConstructible<Untagged>
			: Untagged(std::move(that))
			{}
			constexpr base(const Untagged& that)
			noexcept(std::is_nothrow_copy_constructible<Untagged>::value)
			requires CopyConstructible<Untagged>
			: Untagged(that)
			{}
			base& operator=(const base&) = default;
			base& operator=(base&&) = default;
		protected:
			~base() = default;
		};

		template <class Untagged, std::size_t, class...>
		struct chain {
			using type = base<Untagged>;
		};
		template <class Untagged, std::size_t I, class First, class... Rest>
		struct chain<Untagged, I, First, Rest...> {
			using type = typename First::template tagged_getter<Untagged, I,
				meta::_t<chain<Untagged, I + 1, Rest...>>>;
		};
	} // namespace __tagged

	template <class T>
	concept bool TagSpecifier =
		DerivedFrom<typename T::template tagged_getter<std::tuple<int>, 0, std::tuple<int>>,
			std::tuple<int>>;

	template <class T>
	concept bool TaggedType =
		requires {
			typename __tagged::specifier<T>;
			requires TagSpecifier<__tagged::specifier<T>>;
		};

	///////////////////////////////////////////////////////////////////////////
	// tagged [taggedtup.tagged]
	// Not to spec:
	// * constexpr per P0579
	// * Implements https://github.com/ericniebler/stl2/issues/172
	// * Implements https://github.com/ericniebler/stl2/issues/363
	// * Implements https://github.com/ericniebler/stl2/issues/364
	// * Implements https://github.com/ericniebler/stl2/issues/418
	//
	template <class Base, TagSpecifier...Tags>
	requires sizeof...(Tags) <= tuple_size<Base>::value
	struct tagged : meta::_t<__tagged::chain<Base, 0, Tags...>>	{
	private:
		using base_t = meta::_t<__tagged::chain<Base, 0, Tags...>>;
	public:
		tagged() = default;
		using base_t::base_t;

		// Note: const Base& and Base&& constructors are inherited from __tagged::base<Base>

		template <class Other>
		requires Constructible<Base, Other>
		constexpr tagged(tagged<Other, Tags...>&& that)
		noexcept(std::is_nothrow_constructible<Base, Other&&>::value)
		: base_t(static_cast<Other&&>(that)) {}

		template <class Other>
		requires Constructible<Base, const Other&>
		constexpr tagged(const tagged<Other, Tags...>& that)
		noexcept(std::is_nothrow_constructible<Base, const Other&>::value)
		: base_t(static_cast<const Other&>(that)) {}

		template <class Other>
		requires Assignable<Base&, Other>
		constexpr tagged& operator=(tagged<Other, Tags...>&& that)
		noexcept(std::is_nothrow_assignable<Base&, Other&&>::value)
		{
			static_cast<Base&>(*this) = static_cast<Other&&>(that);
			return *this;
		}

		template <class Other>
		requires Assignable<Base&, const Other&>
		constexpr tagged& operator=(const tagged<Other, Tags...>& that)
		noexcept(std::is_nothrow_assignable<Base&, const Other&>::value)
		{
			static_cast<Base&>(*this) = static_cast<const Other&>(that);
			return *this;
		}

		template <class U>
		requires !Same<decay_t<U>, tagged> && Assignable<Base&, U>
		constexpr tagged& operator=(U&& u) &
		noexcept(std::is_nothrow_assignable<Base&, U&&>::value)
		{
			static_cast<Base&>(*this) = std::forward<U>(u);
			return *this;
		}

		constexpr void swap(tagged& that)
		noexcept(is_nothrow_swappable_v<Base&, Base&>)
		requires Swappable<Base>
		{
			__stl2::swap(static_cast<Base&>(*this), static_cast<Base&>(that));
		}

		friend constexpr void swap(tagged& a, tagged& b)
		noexcept(noexcept(a.swap(b)))
		requires Swappable<Base>
		{
			a.swap(b);
		}
	};

	#define STL2_DEFINE_GETTER(name)                                                                \
		struct name {                                                                               \
			template <class Untagged, ::std::size_t I, class Next>                                  \
			struct tagged_getter : Next {                                                           \
				using Next::Next;                                                                   \
				tagged_getter() = default;                                                          \
				tagged_getter(const tagged_getter&) = default;                                      \
				tagged_getter(tagged_getter&&) = default;                                           \
				                                                                                    \
				tagged_getter& operator=(tagged_getter&&) = default;                                \
				tagged_getter& operator=(const tagged_getter&) = default;                           \
				                                                                                    \
				constexpr decltype(auto) name() &                                                   \
				noexcept(noexcept(::__stl2::detail::adl_get<I>(::std::declval<Untagged&>())))       \
				{                                                                                   \
					Untagged& self = *this;                                                         \
					return ::__stl2::detail::adl_get<I>(self);                                      \
				}                                                                                   \
				constexpr decltype(auto) name() const&                                              \
				noexcept(noexcept(::__stl2::detail::adl_get<I>(::std::declval<const Untagged&>()))) \
				{                                                                                   \
					const Untagged& self = *this;                                                   \
					return ::__stl2::detail::adl_get<I>(self);                                      \
				}                                                                                   \
				constexpr decltype(auto) name() &&                                                  \
				noexcept(noexcept(::__stl2::detail::adl_get<I>(::std::declval<Untagged>())))        \
				{                                                                                   \
					Untagged& self = *this;                                                         \
					return ::__stl2::detail::adl_get<I>(::std::move(self));                         \
				}                                                                                   \
				constexpr decltype(auto) name() const&&                                             \
				noexcept(noexcept(::__stl2::detail::adl_get<I>(::std::declval<const Untagged>())))  \
				{                                                                                   \
					const Untagged& self = *this;                                                   \
					return ::__stl2::detail::adl_get<I>(::std::move(self));                         \
				}                                                                                   \
			protected:                                                                              \
				~tagged_getter() = default;                                                         \
			};                                                                                      \
		};

	// tag specifiers [algorithm.general]
	namespace tag {
		STL2_DEFINE_GETTER(in)
		STL2_DEFINE_GETTER(in1)
		STL2_DEFINE_GETTER(in2)
		STL2_DEFINE_GETTER(out)
		STL2_DEFINE_GETTER(out1)
		STL2_DEFINE_GETTER(out2)
		STL2_DEFINE_GETTER(fun)
		STL2_DEFINE_GETTER(min)
		STL2_DEFINE_GETTER(max)
		STL2_DEFINE_GETTER(begin)
		STL2_DEFINE_GETTER(end)
		STL2_DEFINE_GETTER(count) // Extension
	}
} STL2_CLOSE_NAMESPACE

namespace std {
	template <class Base, ::__stl2::TagSpecifier...Tags>
	struct tuple_size<::__stl2::tagged<Base, Tags...>>
	: tuple_size<Base> { };

	template <size_t N, class Base, ::__stl2::TagSpecifier...Tags>
	struct tuple_element<N, ::__stl2::tagged<Base, Tags...>>
	: tuple_element<N, Base> { };
}

#endif
