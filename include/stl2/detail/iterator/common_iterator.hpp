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
#ifndef STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/variant.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/memory/addressof.hpp>

///////////////////////////////////////////////////////////////////////////
// common_iterator [common.iterators]
//
STL2_OPEN_NAMESPACE {
	namespace __common_iterator {
		template <class I>
		requires
			models::Readable<I> &&
			!std::is_reference<reference_t<I>>::value &&
			models::Constructible<value_type_t<I>, reference_t<I>>
		class operator_arrow_proxy {
			value_type_t<I> value_;
		public:
			constexpr explicit operator_arrow_proxy(reference_t<I>&& x)
			noexcept(std::is_nothrow_constructible<value_type_t<I>, reference_t<I>>::value)
			: value_(std::forward<reference_t<I>>(x)) {}

			STL2_CONSTEXPR_EXT const value_type_t<I>* operator->() const noexcept {
				return __stl2::addressof(value_);
			}
		};

		Readable{I}
		constexpr I operator_arrow_(const I& i, ext::priority_tag<2>)
		noexcept(std::is_nothrow_copy_constructible<I>::value)
		requires
			_Is<I, std::is_pointer> || requires { i.operator->(); }
		{
			return i;
		}

		Readable{I}
		STL2_CONSTEXPR_EXT auto operator_arrow_(const I& i, ext::priority_tag<1>)
		noexcept(noexcept(*i))
		requires
			_Is<reference_t<const I>, std::is_reference>
		{
			return __stl2::addressof(*i);
		}

		Readable{I}
		operator_arrow_proxy<I> operator_arrow_(const I& i, ext::priority_tag<0>)
		noexcept(std::is_nothrow_move_constructible<operator_arrow_proxy<I>>::value &&
			noexcept(operator_arrow_proxy<I>{*i}))
		{
			return operator_arrow_proxy<I>{*i};
		}

		template <Readable I>
		requires
			requires(const I i) {
				__common_iterator::operator_arrow_(i, ext::max_priority_tag);
			}
		decltype(auto) operator_arrow(const I& i)
		STL2_NOEXCEPT_RETURN(
			__common_iterator::operator_arrow_(i, ext::max_priority_tag)
		)

		template <Iterator I, Sentinel<I> S>
		requires !Same<I, S>()
		class cursor;

		struct access {
			template <_SpecializationOf<cursor> C>
			static constexpr decltype(auto) v(C&& c) noexcept {
				return (__stl2::forward<C>(c).v_);
			}
		};

		template <Iterator I, Sentinel<I> S, ConvertibleTo<I> II, ConvertibleTo<S> SS>
		struct convert_visitor {
			constexpr auto operator()(const II& i) const
			STL2_NOEXCEPT_RETURN(
				variant<I, S>{in_place_index<0>, i}
			)
			constexpr auto operator()(const SS& s) const
			STL2_NOEXCEPT_RETURN(
				variant<I, S>{in_place_index<1>, s}
			)
		};

		template <Iterator I1, Iterator I2, Sentinel<I2> S1, Sentinel<I1> S2>
		struct equal_visitor {
			constexpr bool operator()(const I1&, const I2&) const noexcept {
				return true;
			}
			constexpr bool operator()(const I1& i1, const I2& i2) const
			noexcept(noexcept(bool(i1 == i2)))
			requires EqualityComparable<I1, I2>()
			{
				return i1 == i2;
			}
			constexpr bool operator()(const auto& lhs, const auto& rhs) const
			STL2_NOEXCEPT_RETURN(
				bool(lhs == rhs)
			)
			constexpr bool operator()(const S1&, const S2&) const noexcept {
				return true;
			}
		};

		template <class I2, SizedSentinel<I2> I1,
			SizedSentinel<I2> S1, SizedSentinel<I1> S2>
		struct difference_visitor {
			constexpr difference_type_t<I2> operator()(
				const auto& lhs, const auto& rhs) const
			STL2_NOEXCEPT_RETURN(
				static_cast<difference_type_t<I2>>(lhs - rhs)
			)
			constexpr difference_type_t<I2> operator()(
				const S1&, const S2&) const noexcept
			{
				return 0;
			}
		};

		Iterator{I}
		struct cursor_types_base {};
		InputIterator{I}
		struct cursor_types_base<I> {
			using value_type = value_type_t<I>;
			using single_pass = meta::bool_<!models::ForwardIterator<I>>;
		};

		template <Iterator I, Sentinel<I> S>
		requires !Same<I, S>()
		class cursor : public cursor_types_base<I> {
			friend access;
			variant<I, S> v_;
		public:
			using difference_type = difference_type_t<I>;

			class mixin : public detail::ebo_box<cursor> {
				using box_t = detail::ebo_box<cursor>;
			public:
				using difference_type = cursor::difference_type;
				using box_t::box_t;
			};

			cursor() = default;
			STL2_CONSTEXPR_EXT cursor(I i)
			noexcept(is_nothrow_constructible<variant<I, S>, I>::value)
			: v_{__stl2::move(i)} {}
			STL2_CONSTEXPR_EXT cursor(S s)
			noexcept(is_nothrow_constructible<variant<I, S>, S>::value)
			: v_{__stl2::move(s)} {}
			template <ConvertibleTo<I> II, ConvertibleTo<S> SS>
			STL2_CONSTEXPR_EXT cursor(const cursor<II, SS>& i)
			noexcept(noexcept(variant<I, S>{__stl2::visit(
				convert_visitor<I, S, II, SS>{}, access::v(i))}))
			: v_{__stl2::visit(convert_visitor<I, S, II, SS>{}, access::v(i))}
			{}

			STL2_CONSTEXPR_EXT decltype(auto) read() const
			noexcept(noexcept(*declval<const I&>()))
			requires InputIterator<I>()
			{
				STL2_EXPECT(__stl2::holds_alternative<I>(v_));
				return *__stl2::get_unchecked<I>(v_);
			}

			STL2_CONSTEXPR_EXT auto arrow() const
			noexcept(noexcept(__common_iterator::operator_arrow(std::declval<I const&>())))
			requires InputIterator<I>()
			{
				STL2_EXPECT(__stl2::holds_alternative<I>(v_));
				return __common_iterator::operator_arrow(__stl2::get_unchecked<I>(v_));
			}

			template <class T>
			requires
				!InputIterator<I>() && Writable<I, T>()
			STL2_CONSTEXPR_EXT void write(T&& t)
			noexcept(noexcept(*declval<I&>() = __stl2::forward<T>(t)))
			{
				STL2_EXPECT(__stl2::holds_alternative<I>(v_));
				*__stl2::get_unchecked<I>(v_) = __stl2::forward<T>(t);
			}

			STL2_CONSTEXPR_EXT void next()
			noexcept(noexcept(++declval<I&>()))
			{
				STL2_EXPECT(__stl2::holds_alternative<I>(v_));
				++__stl2::get_unchecked<I>(v_);
			}

			template <class OI, class OS>
			requires
				Sentinel<S, OI>() && Sentinel<OS, I>()
			STL2_CONSTEXPR_EXT bool equal(const cursor<OI, OS>& that) const
			STL2_NOEXCEPT_RETURN(
				__stl2::visit(equal_visitor<I, OI, S, OS>{}, access::v(that), v_)
			)

			template <class OI, class OS>
			requires
				SizedSentinel<OS, I>() && SizedSentinel<OI, I>() && SizedSentinel<S, OI>()
			STL2_CONSTEXPR_EXT difference_type_t<I> distance_to(
				const cursor<OI, OS>& that) const
			STL2_NOEXCEPT_RETURN(
				__stl2::visit(difference_visitor<I, OI, S, OS>{}, v_, access::v(that))
			)

			// Extension
			STL2_CONSTEXPR_EXT decltype(auto) indirect_move() const
			noexcept(noexcept(__stl2::iter_move(declval<const I&>())))
			requires Readable<I>()
			{
				STL2_EXPECT(__stl2::holds_alternative<I>(v_));
				return __stl2::iter_move(__stl2::get_unchecked<I>(v_));
			}

			// Extension
			template <IndirectlySwappable<I> OI, class OS>
			STL2_CONSTEXPR_EXT void indirect_swap(const cursor<OI, OS>& that)
			noexcept(noexcept(
				__stl2::iter_swap(declval<const I&>(), declval<const OI&>())))
			{
				auto&& tv = access::v(that);
				STL2_EXPECT(__stl2::holds_alternative<I>(v_));
				STL2_EXPECT(__stl2::holds_alternative<OI>(tv));
				__stl2::iter_swap(__stl2::get_unchecked<I>(v_),
					__stl2::get_unchecked<OI>(tv));
			}
		};
	}


	// Not to spec:
	// * hooks iter_move and iter_swap (See https://github.com/ericniebler/stl2/issues/245)
	template <Iterator I, Sentinel<I> S>
	requires
		!Same<I, S>()
	using common_iterator = basic_iterator<__common_iterator::cursor<I, S>>;
} STL2_CLOSE_NAMESPACE

#endif
