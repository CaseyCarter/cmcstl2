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
	template <class T>
	struct __tag_properties {};
	template <class Spec, class Arg>
	struct __tag_properties<Spec(Arg)> {
		using specifier = Spec;
		using type = Arg;
	};
	template <class T>
	using __tag_spec = typename __tag_properties<T>::specifier;

	template <class T>
	using __tag_elem = meta::_t<__tag_properties<T>>;

	template <class Derived, std::size_t I>
	class tag_specifier_base;

	template <class T, class D, std::size_t I>
	requires
		DerivedFrom<
			typename T::template tagged_getter<tag_specifier_base<D, I>>,
				tag_specifier_base<D, I>>()
	using __tag_specifier_getter =
		typename T::template tagged_getter<tag_specifier_base<D, I>>;

	template <class T>
	concept bool TagSpecifier() {
		return requires {
			typename __tag_specifier_getter<T, std::tuple<int>, 0>;
		};
	}

	template <class T>
	concept bool TaggedType() {
		return requires {
			typename __tag_spec<T>;
			requires TagSpecifier<__tag_spec<T>>();
		};
	}

	template <class Base, TagSpecifier...Tags>
	requires sizeof...(Tags) <= tuple_size<Base>::value
	struct tagged;

	class __getters {
		template <class Base, TagSpecifier...Tags>
		requires sizeof...(Tags) <= tuple_size<Base>::value
		friend struct tagged;

		template <class Type, class Indices, TagSpecifier...Tags>
		class collect_;

		template <class Type, std::size_t...Is, TagSpecifier...Tags>
		requires sizeof...(Is) == sizeof...(Tags)
		class collect_<Type, std::index_sequence<Is...>, Tags...>
		: public __tag_specifier_getter<Tags, Type, Is>... {
		protected:
			~collect_() = default;
		};

		template <class Type, TagSpecifier...Tags>
		using collect = collect_<Type, std::index_sequence_for<Tags...>, Tags...>;
	};

	// tagged
	template <class Base, TagSpecifier...Tags>
	requires sizeof...(Tags) <= tuple_size<Base>::value
	struct tagged
	: Base, __getters::collect<tagged<Base, Tags...>, Tags...>
	{
		using Base::Base;
		tagged() = default;

		// Not to spec: constexpr per P0579
		template <class Other>
		requires Constructible<Base, Other>()
		constexpr tagged(tagged<Other, Tags...>&& that)
		noexcept(is_nothrow_constructible<Base, Other&&>::value)
		: Base(static_cast<Other&&>(that)) {}

		// Not to spec: constexpr per P0579
		template <class Other>
		requires Constructible<Base, const Other&>()
		constexpr tagged(tagged<Other, Tags...> const& that)
		noexcept(is_nothrow_constructible<Base, const Other&>::value)
		: Base(static_cast<const Other&>(that)) {}

		// Not to spec: constexpr per P0579
		template <class Other>
		requires Assignable<Base&, Other>()
		constexpr tagged& operator=(tagged<Other, Tags...>&& that)
		noexcept(is_nothrow_assignable<Base&, Other&&>::value)
		{
			static_cast<Base&>(*this) = static_cast<Other&&>(that);
			return *this;
		}

		// Not to spec: constexpr per P0579
		template <class Other>
		requires Assignable<Base&, const Other&>()
		constexpr tagged& operator=(const tagged<Other, Tags...>& that)
		noexcept(is_nothrow_assignable<Base&, const Other&>::value)
		{
			static_cast<Base&>(*this) = static_cast<const Other&>(that);
			return *this;
		}

		// Not to spec: constexpr per P0579
		template <class U>
		requires !Same<decay_t<U>, tagged>() && Assignable<Base&, U>()
		constexpr tagged& operator=(U&& u) &
		noexcept(is_nothrow_assignable<Base&, U&&>::value)
		{
			static_cast<Base&>(*this) = __stl2::forward<U>(u);
			return *this;
		}

		// Not to spec: constexpr per P0579
		constexpr void swap(tagged& that)
		noexcept(is_nothrow_swappable_v<Base&, Base&>)
		requires Swappable<Base&>()
		{
			__stl2::swap(static_cast<Base&>(*this), static_cast<Base&>(that));
		}

		// Not to spec: constexpr per P0579
		friend constexpr void swap(tagged& a, tagged& b)
		noexcept(noexcept(a.swap(b)))
		requires Swappable<Base&>()
		{
			a.swap(b);
		}

		// Not to spec: Extension
		constexpr Base& base() & { return *this; }
		constexpr const Base& base() const& { return *this; }
		constexpr Base&& base() && { return __stl2::move(*this); }
		constexpr const Base&& base() const&& { return __stl2::move(*this); }
	};

	template <class Derived, std::size_t I>
	class tag_specifier_base {
	protected:
		~tag_specifier_base() = default;

		constexpr auto&& get() & {
			using __stl2::get;
			return get<I>(derived().base());
		}
		constexpr auto&& get() const& {
			using __stl2::get;
			return get<I>(derived().base());
		}
		constexpr auto&& get() && {
			using __stl2::get;
			return get<I>(__stl2::move(derived()).base());
		}
		constexpr auto&& get() const&& {
			using __stl2::get;
			return get<I>(__stl2::move(derived()).base());
		}
	private:
		constexpr Derived& derived() noexcept {
			static_assert(models::DerivedFrom<Derived, tag_specifier_base>);
			return static_cast<Derived&>(*this);
		}
		constexpr const Derived& derived() const noexcept {
			static_assert(models::DerivedFrom<Derived, tag_specifier_base>);
			return static_cast<const Derived&>(*this);
		}
	};

	#define STL2_DEFINE_GETTER(name)                        \
		struct name {                                       \
			template <class Base>                           \
			struct tagged_getter : Base {                   \
				constexpr decltype(auto) name() & {         \
					return Base::get();                     \
				}                                           \
				constexpr decltype(auto) name() const& {    \
					return Base::get();                     \
				}                                           \
				constexpr decltype(auto) name() && {        \
					return __stl2::move(*this).Base::get(); \
				}                                           \
				/* Not to spec: Extension */                \
				constexpr decltype(auto) name() const&& {   \
					return __stl2::move(*this).Base::get(); \
				}                                           \
			protected:                                      \
				~tagged_getter() = default;                 \
			};                                              \
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

	#undef STL2_DEFINE_GETTER

	template <class T, class Base, class...Tags>
	constexpr std::size_t tuple_find<T, tagged<Base, Tags...>> =
		tuple_find<T, Base>;
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
