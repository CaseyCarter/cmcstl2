// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_CONCEPTS_HPP
#define STL2_DETAIL_ITERATOR_CONCEPTS_HPP

#include <functional>
#include <iterator>

#include <stl2/type_traits.hpp>
#include <stl2/utility.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/increment.hpp>

///////////////////////////////////////////////////////////////////////////
// Iterator concepts [iterator.requirements]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		template <class T>
		concept bool Dereferenceable =
			requires(T& t) {
				{ *t } -> auto&&;
			};
	}

	namespace models {
		template <class>
		constexpr bool Dereferenceable = false;
		detail::Dereferenceable{R}
		constexpr bool Dereferenceable<R> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// reference_t [iterator.assoc]
	//
	detail::Dereferenceable{R}
	using reference_t = decltype(*declval<R&>());

	///////////////////////////////////////////////////////////////////////////
	// iter_move
	// Not to spec: Uses a two-step dispatch instead of requiring user overloads
	// to compete with the default overloads in overload resolution.
	// See https://github.com/ericniebler/stl2/issues/242
	//
	namespace __iter_move {
		// Not a poison pill, simply a non-ADL block.
		void iter_move(); // undefined

		template <class>
		constexpr bool has_customization = false;
		template <class R>
		requires
			detail::Dereferenceable<R> &&
			requires(R&& r) {
				{ iter_move((R&&)r) } -> auto&&;
			}
		constexpr bool has_customization<R> = true;

		template <class R>
		using rvalue =
			meta::if_<is_reference<R>, remove_reference_t<R>&&, decay_t<R>>;

		struct fn {
			template <class R>
			requires
				detail::Dereferenceable<R> && has_customization<R>
			constexpr decltype(auto) operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				iter_move((R&&)r)
			)

			template <class R>
			requires
				detail::Dereferenceable<R>
			constexpr rvalue<reference_t<R>> operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				static_cast<rvalue<reference_t<R>>>(*r)
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& iter_move = detail::static_const<__iter_move::fn>::value;
	}

	///////////////////////////////////////////////////////////////////////////
	// rvalue_reference_t [Extension]
	// From the proxy iterator work (P0022).
	//
	detail::Dereferenceable{R}
	using rvalue_reference_t = decltype(__stl2::iter_move(declval<R&>()));

	///////////////////////////////////////////////////////////////////////////
	// value_type [readable.iterators]
	//
	namespace detail {
		template <class T>
		constexpr bool IsValueType = !is_void<T>::value;

		template <class T>
		concept bool MemberValueType =
			requires { typename T::value_type; };

		template <class T>
		concept bool MemberElementType =
			requires { typename T::element_type; };
	}

	template <class>
	struct value_type {};

	template <class T>
	struct value_type<T*> :
		meta::lazy::if_<is_object<T>, remove_cv_t<T>> {};

	template <_Is<is_array> T>
	struct value_type<T> : value_type<decay_t<T>> {};

	template <class I>
	struct value_type<I const> : value_type<decay_t<I>> {};

	template <detail::MemberValueType T>
	struct value_type<T> {
		using type = typename T::value_type;
	};

	template <detail::MemberElementType T>
	struct value_type<T> {
		using type = typename T::element_type;
	};

	///////////////////////////////////////////////////////////////////////////
	// value_type_t [readable.iterators]
	//
	template <class T>
	using value_type_t = meta::_t<value_type<T>>;

	///////////////////////////////////////////////////////////////////////////
	// Readable [readable.iterators]
	// Not to spec: Disables the "Experimental additional tests"
	// See https://github.com/ericniebler/stl2/issues/239
	// Additional changes from P0547
	//
	template <class I>
	concept bool Readable() {
		return
			requires {
				// Associated types
				typename value_type_t<I>;
				typename reference_t<I>;
				typename rvalue_reference_t<I>;
			} &&
			// Relationships between associated types
			CommonReference<reference_t<I>, value_type_t<I>&>() &&
			CommonReference<reference_t<I>, rvalue_reference_t<I>>() &&
			CommonReference<rvalue_reference_t<I>, const value_type_t<I>&>();
	}

	namespace models {
		template <class>
		constexpr bool Readable = false;
		__stl2::Readable{I}
		constexpr bool Readable<I> = true;
	}

	// A generally useful dependent type
	Readable{I}
	using iter_common_reference_t =
		common_reference_t<reference_t<I>, value_type_t<I>&>;

	///////////////////////////////////////////////////////////////////////////
	// Writable [iterators.writable]
	// Not to spec, but getting closer.
	// See https://github.com/ericniebler/stl2/issues/240
	// Additional changes from P0547
	//
	template <class, class>
	constexpr bool __writable = false;
	template <class Out, class R>
	requires
		requires(Out& o, R&& r) {
			(void)(*o = (R&&)r);
			// Handwaving: After the assignment, the value of the element denoted
			//             by o "corresponds" to the value originally denoted by r.
			// Axiom: If r equals foo && Readable<Out>() &&
			//        Same<value_type_t<Out>, ????>() then
			//        (*o = (R&&)r, *o equals foo)
		}
	constexpr bool __writable<Out, R> = true;

	template <class Out, class R>
	concept bool Writable() {
		return detail::Dereferenceable<Out> && __writable<Out, R>;
	}

	namespace models {
		template <class, class>
		constexpr bool Writable = false;
		__stl2::Writable{Out, R}
		constexpr bool Writable<Out, R> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// IndirectlyMovable [commonalgoreq.indirectlymovable]
	//
	template <class In, class Out>
	concept bool IndirectlyMovable() {
		return Readable<In>() && Writable<Out, rvalue_reference_t<In>>();
	}

	namespace models {
		template <class, class>
		constexpr bool IndirectlyMovable = false;
		__stl2::IndirectlyMovable{In, Out}
		constexpr bool IndirectlyMovable<In, Out> = true;
	}

	template <class In, class Out>
	constexpr bool is_nothrow_indirectly_movable_v = false;

	IndirectlyMovable{In, Out}
	constexpr bool is_nothrow_indirectly_movable_v<In, Out> =
		noexcept(noexcept(declval<reference_t<Out>>() = __stl2::iter_move(declval<In>())));

	///////////////////////////////////////////////////////////////////////////
	// IndirectlyMovableStorable [commonalgoreq.indirectlymovable]
	//
	template <class In, class Out>
	concept bool IndirectlyMovableStorable() {
		return IndirectlyMovable<In, Out>() &&
			Writable<Out, value_type_t<In>&&>() &&
			Movable<value_type_t<In>>() &&
			Constructible<value_type_t<In>, rvalue_reference_t<In>>() &&
			Assignable<value_type_t<In>&, rvalue_reference_t<In>>();
	}

	namespace models {
		template <class, class>
		constexpr bool IndirectlyMovableStorable = false;
		__stl2::IndirectlyMovableStorable{In, Out}
		constexpr bool IndirectlyMovableStorable<In, Out> = true;
	}

	template <class In, class Out>
	constexpr bool is_nothrow_indirectly_movable_storable_v = false;

	IndirectlyMovableStorable{In, Out}
	constexpr bool is_nothrow_indirectly_movable_storable_v<In, Out> =
		is_nothrow_indirectly_movable_v<In, Out> &&
		is_nothrow_assignable<reference_t<Out>, value_type_t<In>>::value &&
		is_nothrow_constructible<value_type_t<In>, rvalue_reference_t<In>>::value &&
		is_nothrow_assignable<value_type_t<In>&, rvalue_reference_t<In>>::value;

	///////////////////////////////////////////////////////////////////////////
	// IndirectlyCopyable [commonalgoreq.indirectlycopyable]
	//
	template <class In, class Out>
	concept bool IndirectlyCopyable() {
		return Readable<In>() && Writable<Out, reference_t<In>>();
	}

	namespace models {
		template <class, class>
		constexpr bool IndirectlyCopyable = false;
		__stl2::IndirectlyCopyable{In, Out}
		constexpr bool IndirectlyCopyable<In, Out> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// IndirectlyCopyableStorable [commonalgoreq.indirectlycopyable]
	//
	template <class In, class Out>
	concept bool IndirectlyCopyableStorable() {
		return IndirectlyCopyable<In, Out>() &&
			Writable<Out, const value_type_t<In>&>() &&
			Copyable<value_type_t<In>>() &&
			Constructible<value_type_t<In>, reference_t<In>>() &&
			Assignable<value_type_t<In>&, reference_t<In>>();
	}

	namespace models {
		template <class, class>
		constexpr bool IndirectlyCopyableStorable = false;
		__stl2::IndirectlyCopyableStorable{In, Out}
		constexpr bool IndirectlyCopyableStorable<In, Out> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// iter_swap
	// Not to spec
	// See https://github.com/ericniebler/stl2/issues/242
	//
	namespace __iter_swap {
		// Poison pill for iter_swap. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		void iter_swap(auto, auto) = delete;

		template <class, class>
		constexpr bool has_customization = false;
		template <class R1, class R2>
		requires
			requires(R1&& r1, R2&& r2) {
				iter_swap((R1&&)r1, (R2&&)r2);
			}
		constexpr bool has_customization<R1, R2> = true;

		template <class R1, class R2>
		requires
			Swappable<reference_t<R1>, reference_t<R2>>()
		constexpr void impl(R1&& r1, R2&& r2)
		STL2_NOEXCEPT_RETURN(
			__stl2::swap(*r1, *r2)
		)

		template <class R1, class R2>
		requires
			!Swappable<reference_t<R1>, reference_t<R2>>() &&
			IndirectlyMovableStorable<R1, R2>() &&
			IndirectlyMovableStorable<R2, R1>()
		constexpr void impl(R1& r1, R2& r2)
			noexcept(
				is_nothrow_indirectly_movable_storable_v<R1, R2> &&
				is_nothrow_indirectly_movable_storable_v<R2, R1>)
		{
			value_type_t<R1> tmp = __stl2::iter_move(r1);
			*r1 = __stl2::iter_move(r2);
			*r2 = __stl2::move(tmp);
		}

		struct fn {
			template <class R1, class R2>
			requires
				Readable<remove_reference_t<R1>>() &&
				Readable<remove_reference_t<R2>>() &&
				has_customization<R1, R2>
			constexpr void operator()(R1&& r1, R2&& r2) const
			STL2_NOEXCEPT_RETURN(
				static_cast<void>(iter_swap((R1&&)r1, (R2&&)r2))
			)

			template <class R1, class R2>
			requires
				Readable<remove_reference_t<R1>>() &&
				Readable<remove_reference_t<R2>>() &&
				!has_customization<R1&, R2&> &&
				requires(R1& r1, R2& r2) {
					__iter_swap::impl(r1, r2);
				}
			constexpr void operator()(R1&& r1, R2&& r2) const
			STL2_NOEXCEPT_RETURN(
				__iter_swap::impl(r1, r2)
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& iter_swap = detail::static_const<__iter_swap::fn>::value;
	}

	///////////////////////////////////////////////////////////////////////////
	// IndirectlySwappable [commonalgoreq.indirectlyswappable]
	// Not to spec
	// See https://github.com/ericniebler/stl2/issues/241
	//

	template <class I1, class I2 = I1>
	concept bool IndirectlySwappable() {
		return requires(I1&& i1, I2&& i2) {
			__stl2::iter_swap((I1&&)i1, (I2&&)i2);
			__stl2::iter_swap((I2&&)i2, (I1&&)i1);
			__stl2::iter_swap((I1&&)i1, (I1&&)i1);
			__stl2::iter_swap((I2&&)i2, (I2&&)i2);
		};
	}

	namespace models {
		template <class I1, class I2 = I1>
		constexpr bool IndirectlySwappable = false;
		__stl2::IndirectlySwappable{I1, I2}
		constexpr bool IndirectlySwappable<I1, I2> = true;
	}

	template <class R1, class R2>
	constexpr bool is_nothrow_indirectly_swappable_v = false;

	IndirectlySwappable{R1, R2}
	constexpr bool is_nothrow_indirectly_swappable_v<R1, R2> =
		noexcept(__stl2::iter_swap(declval<R1>(), declval<R2>())) &&
		noexcept(__stl2::iter_swap(declval<R2>(), declval<R1>())) &&
		noexcept(__stl2::iter_swap(declval<R1>(), declval<R1>())) &&
		noexcept(__stl2::iter_swap(declval<R2>(), declval<R2>()));

	template <class R1, class R2>
	using is_nothrow_indirectly_swappable_t =
		meta::bool_<is_nothrow_indirectly_swappable_v<R1, R2>>;

	template <class R1, class R2>
	struct is_nothrow_indirectly_swappable :
		is_nothrow_indirectly_swappable_t<R1, R2> {};

	///////////////////////////////////////////////////////////////////////////
	// Iterator tags [std.iterator.tags]
	// Extension: contiguous_iterator_tag for denoting contiguous iterators.
	//
	struct output_iterator_tag {};
	struct input_iterator_tag {};
	struct forward_iterator_tag : input_iterator_tag {};
	struct bidirectional_iterator_tag : forward_iterator_tag {};
	struct random_access_iterator_tag : bidirectional_iterator_tag {};

	namespace ext {
		struct contiguous_iterator_tag : random_access_iterator_tag {};
	}

	///////////////////////////////////////////////////////////////////////////
	// iterator_category and iterator_category_t [iterator.assoc]
	// Extension: Category for pointers is ext::contiguous_iterator_tag,
	//     which derives from random_access_iterator_tag.
	//
	namespace detail {
		template <class T>
		concept bool MemberIteratorCategory =
			requires { typename T::iterator_category; };

		namespace std_to_stl2_iterator_category_ {
			input_iterator_tag f(std::input_iterator_tag*);
			forward_iterator_tag f(std::forward_iterator_tag*);
			bidirectional_iterator_tag f(std::bidirectional_iterator_tag*);
			random_access_iterator_tag f(std::random_access_iterator_tag*);
		}

		template <class T>
		using std_to_stl2_iterator_category =
			decltype(std_to_stl2_iterator_category_::f((T*)nullptr));
	}

	template <class> struct iterator_category {};

	template <_IsNot<is_void> T>
	struct iterator_category<T*> {
		using type = ext::contiguous_iterator_tag;
	};

	template <detail::MemberIteratorCategory T>
	struct iterator_category<T> {
		using type = typename T::iterator_category;
	};

	template <detail::MemberIteratorCategory T>
	requires DerivedFrom<typename T::iterator_category, std::input_iterator_tag>()
	struct iterator_category<T> {
		using type = detail::std_to_stl2_iterator_category<typename T::iterator_category>;
	};

	template <detail::MemberIteratorCategory T>
	requires
		DerivedFrom<typename T::iterator_category, std::output_iterator_tag>() &&
		!DerivedFrom<typename T::iterator_category, std::input_iterator_tag>()
	struct iterator_category<T> {};

	template <class T>
	using iterator_category_t =
		meta::_t<iterator_category<remove_cv_t<T>>>;

	///////////////////////////////////////////////////////////////////////////
	// Iterator [iterators.iterator]
	//
	// Denotes an element of a range, i.e., is a position.
	//
	template <class I>
	concept bool Iterator() {
		return detail::Dereferenceable<I&> && WeaklyIncrementable<I>();
		// Axiom?: i is non-singular iff it denotes an element
		// Axiom?: if i equals j then i and j denote equal elements
		// Axiom?: I{} is in the domain of copy/move construction/assignment
		//        (This should probably be a requirement of the object concepts,
		//         or at least Semiregular.)
	}

	namespace models {
		template <class>
		constexpr bool Iterator = false;
		__stl2::Iterator{I}
		constexpr bool Iterator<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// Sentinel [sentinel.iterators]
	//
	// A relationship between an Iterator and a Semiregular ("sentinel")
	// that denote a range.
	//
	template <class S, class I>
	concept bool Sentinel() {
		return Iterator<I>() &&
			Semiregular<S>() &&
			WeaklyEqualityComparable<S, I>();
			// Axiom: if [i,s) denotes a range then:
			//        * i == s is well-defined
			//        * if bool(i == s) then [i,s) is empty
			//        * if bool(i != s) then:
			//          * i is dereferenceable
			//          * the element denoted by i is the first element of [i,s)
			//          * [++i,s) denotes a range
	}

	namespace models {
		template <class, class>
		constexpr bool Sentinel = false;
		__stl2::Sentinel{S, I}
		constexpr bool Sentinel<S, I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// SizedSentinel [iterators.sizedsentinel]
	//
	// Refinement of Sentinel that provides the capability to compute the
	// distance between a Iterator and Sentinel that denote a range in
	// constant time.
	//
	template <class S, class I>
	constexpr bool disable_sized_sentinel = false;

	template <class S, class I>
	concept bool SizedSentinel() {
		return Sentinel<S, I>() &&
			!disable_sized_sentinel<remove_cv_t<S>, remove_cv_t<I>> &&
			requires(const I i, const S s) {
				{ s - i } -> Same<difference_type_t<I>>&&;
				{ i - s } -> Same<difference_type_t<I>>&&;
				// Axiom: If [i,s) denotes a range and N is the smallest
				//        non-negative integer such that N applications of
				//        ++i make bool(i == s) == true
				//        * if N is representable by difference_type_t<I> then
				//          s - i is well-defined and equal to N
				//        * if -N is representable by difference_type_t<I> then
				//          i - s is well-defined and equal to -N
			};
	}

	namespace models {
		template <class S, class I>
		constexpr bool SizedSentinel = false;
		__stl2::SizedSentinel{S, I}
		constexpr bool SizedSentinel<S, I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// OutputIterator [iterators.output]
	// Not to spec:
	// adds requirement on *i++ from P0541.
	template <class I, class T>
	concept bool OutputIterator() {
		return Iterator<I>() && Writable<I, T>() &&
			requires(I& i, T&& t) {
				*i++ = std::forward<T>(t);
			};
	}

	namespace models {
		template <class, class>
		constexpr bool OutputIterator = false;
		__stl2::OutputIterator{I, T}
		constexpr bool OutputIterator<I, T> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// InputIterator [iterators.input]
	// Not to spec: No requirements on i++
	// See http://github.com/ericniebler/stl2/issues/232
	//
	template <class I>
	concept bool InputIterator() {
		return Iterator<I>() &&
			Readable<I>() &&
			requires(I& i, const I& ci) {
				typename iterator_category_t<I>;
				DerivedFrom<iterator_category_t<I>, input_iterator_tag>();
				i++;
			};
	}

	namespace models {
		template <class>
		constexpr bool InputIterator = false;
		__stl2::InputIterator{I}
		constexpr bool InputIterator<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// ForwardIterator [iterators.forward]
	//
	template <class I>
	concept bool ForwardIterator() {
		return InputIterator<I>() &&
			DerivedFrom<iterator_category_t<I>, forward_iterator_tag>() &&
			Incrementable<I>() &&
			Sentinel<I, I>();
			// Axiom: I{} is equality-preserving and non-singular
			// Axiom: if i equals j then i and j denote the same element.
			// Axiom: if [i,s) denotes a range && bool(i != s) then [i,i+1) denotes
			//        a range and i == j has the same definition space as
			//        i+1 == j
			//        Note: intent is to require == et al to be well-defined over
			//        all iterator values that participate in a range.
	}

	namespace models {
		template <class>
		constexpr bool ForwardIterator = false;
		__stl2::ForwardIterator{I}
		constexpr bool ForwardIterator<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// BidirectionalIterator [iterators.bidirectional]
	//
	template <class I>
	concept bool BidirectionalIterator() {
		return ForwardIterator<I>() &&
			DerivedFrom<iterator_category_t<I>, bidirectional_iterator_tag>() &&
			ext::Decrementable<I>();
	}

	namespace models {
		template <class>
		constexpr bool BidirectionalIterator = false;
		__stl2::BidirectionalIterator{I}
		constexpr bool BidirectionalIterator<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// RandomAccessIterator [iterators.random.access]
	//
	template <class I>
	concept bool RandomAccessIterator() {
		return BidirectionalIterator<I>() &&
			DerivedFrom<iterator_category_t<I>, random_access_iterator_tag>() &&
			SizedSentinel<I, I>() &&
			// Should ordering be in SizedSentinel and/or RandomAccessIncrementable?
			StrictTotallyOrdered<I>() &&
			ext::RandomAccessIncrementable<I>() &&
			requires(const I& ci, const difference_type_t<I> n) {
				ci[n];
				requires Same<decltype(ci[n]), reference_t<I>>();
			};
		// FIXME: Axioms for definition space of ordering operations. Don't
		// require them to be the same space as ==, since pointers can't meet
		// that requirement. Formulation should be similar to that for == in
		// ForwardIterator, e.g., "if [i,j) denotes a range, i < j et al are
		// well-defined."
	}

	namespace models {
		template <class>
		constexpr bool RandomAccessIterator = false;
		__stl2::RandomAccessIterator{I}
		constexpr bool RandomAccessIterator<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// ContiguousIterator [Extension]
	//
	namespace ext {
		template <class I>
		concept bool ContiguousIterator() {
			return RandomAccessIterator<I>() &&
				DerivedFrom<iterator_category_t<I>, contiguous_iterator_tag>() &&
				_Is<reference_t<I>, is_reference> &&
				Same<value_type_t<I>, decay_t<reference_t<I>>>();
		}
	}

	namespace models {
		template <class>
		constexpr bool ContiguousIterator = false;
		__stl2::ext::ContiguousIterator{I}
		constexpr bool ContiguousIterator<I> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// iterator_traits [iterator.assoc]
	//
	template <InputIterator I>
	struct __pointer_type {
		using type = add_pointer_t<reference_t<I>>;
	};

	template <InputIterator I>
	requires
		requires(I i) {
			{ i.operator->() } -> auto&&;
		}
	struct __pointer_type<I> {
		using type = decltype(declval<I&>().operator->());
	};

	template <class>
	struct __iterator_traits {};

	Iterator{I}
	struct __iterator_traits<I> {
		using difference_type = difference_type_t<I>;
		using value_type = void;
		using reference = void;
		using pointer = void;
		using iterator_category = output_iterator_tag;
	};

	InputIterator{I}
	struct __iterator_traits<I> {
		using difference_type = difference_type_t<I>;
		using value_type = value_type_t<I>;
		using reference = reference_t<I>;
		using pointer = meta::_t<__pointer_type<I>>;
		using iterator_category = iterator_category_t<I>;
	};

	template <class I>
	using iterator_traits = __iterator_traits<I>;

	///////////////////////////////////////////////////////////////////////////
	// Standard iterator traits [iterator.stdtraits]
	namespace detail {
		namespace stl2_to_std_iterator_category_ {
			template <class Ref, class Cat> Cat g(std::input_iterator_tag*, Cat*);
			template <class Ref, class Cat> Cat g(std::output_iterator_tag*, Cat*);
			template <class Ref, class Cat> std::input_iterator_tag g(void*, Cat*, Ref* = 0);
			template <class Ref> std::input_iterator_tag g(const void*, input_iterator_tag*);
			template <class Ref> std::forward_iterator_tag g(const void*, forward_iterator_tag*);
			template <class Ref> std::bidirectional_iterator_tag g(const void*, bidirectional_iterator_tag*);
			template <class Ref> std::random_access_iterator_tag g(const void*, random_access_iterator_tag*);
		}

		template <class Cat, class Ref>
		using stl2_to_std_iterator_category =
			decltype(stl2_to_std_iterator_category_::g<Ref>((Cat*)0, (Cat*)0));

		template <class T, class U = void>
		struct value_type_with_a_default : meta::id<U> {};
		template <class T, class U>
			requires requires { typename T::value_type; }
		struct value_type_with_a_default<T, U> : meta::id<typename T::value_type> {};

		template <class T, class U = void>
		struct reference_with_a_default : meta::id<U> {};
		template <class T, class U>
			requires requires { typename T::reference; }
		struct reference_with_a_default<T, U> : meta::id<typename T::reference> {};

		template <class T, class U = void>
		struct pointer_with_a_default : meta::id<U> {};
		template <class T, class U>
			requires requires { typename T::pointer; }
		struct pointer_with_a_default<T, U> : meta::id<typename T::pointer> {};

		template <class I>
		concept bool LooksLikeSTL1Iterator() {
			return requires {
				typename I::iterator_category;
				requires DerivedFrom<typename I::iterator_category, std::input_iterator_tag>() ||
								 DerivedFrom<typename I::iterator_category, std::output_iterator_tag>();
			};
		}
	}
} STL2_CLOSE_NAMESPACE

namespace std {
	template <::__stl2::Iterator Out>
		// HACKHACK to avoid partial specialization after instantiation errors. Platform
		// vendors can avoid this hack by fixing up stdlib headers to fwd declare these
		// partial specializations in the same place that std::iterator_traits is first
		// defined.
		requires !::__stl2::detail::LooksLikeSTL1Iterator<Out>()
	struct iterator_traits<Out> {
		using difference_type   = ::__stl2::difference_type_t<Out>;
		using value_type        = meta::_t<::__stl2::detail::value_type_with_a_default<Out>>;
		using reference         = meta::_t<::__stl2::detail::reference_with_a_default<Out>>;
		using pointer           = meta::_t<::__stl2::detail::pointer_with_a_default<Out>>;
		using iterator_category = std::output_iterator_tag;
	};

	template <::__stl2::InputIterator In>
	requires
		!::__stl2::detail::LooksLikeSTL1Iterator<In>()
	struct iterator_traits<In> { };

	template <::__stl2::InputIterator In>
	requires
		!::__stl2::detail::LooksLikeSTL1Iterator<In>() &&
		::__stl2::Sentinel<In, In>()
	struct iterator_traits<In> {
		using difference_type   = ::__stl2::difference_type_t<In>;
		using value_type        = ::__stl2::value_type_t<In>;
		using reference         =
				meta::_t<::__stl2::detail::reference_with_a_default<
						In, ::__stl2::reference_t<In>>>;
		using pointer           =
				meta::_t<::__stl2::detail::pointer_with_a_default<In,
						typename ::__stl2::iterator_traits<In>::pointer>>;
		using iterator_category =
			::__stl2::detail::stl2_to_std_iterator_category<
					::__stl2::iterator_category_t<In>,
					::__stl2::reference_t<In>>;
	};
} // namespace std

#endif
