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
#ifndef STL2_DETAIL_CONCEPTS_OBJECT_HPP
#define STL2_DETAIL_CONCEPTS_OBJECT_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object/assignable.hpp>
#include <stl2/detail/concepts/object/regular.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// __f [Implementation detail, at least for now]
	//
	// Utility alias that simplifies the specification of forwarding functions
	// whose target will eventually accept a parameter by value. E.g., the
	// range overload of find:
	//
	//   template<InputRange Rng, class T, class Proj = identity>
	//     requires IndirectRelation<equal_to<>,
	//                projected<iterator_t<Rng>, Proj>, const T*>()
	//   safe_iterator_t<Rng>
	//   find(Rng&& rng, const T& value, Proj proj = Proj{});
	//
	// can be implemented to perfect-forward to the iterator overload as:
	//
	//   template<InputRange Rng, class T, class Proj = identity>
	//     requires IndirectRelation<equal_to<>,
	//                projected<iterator_t<Rng>, __f<Proj>>, // NW: __f<Proj>
	//                const T*>()
	//   safe_iterator_t<Rng>
	//   find(Rng&& rng, const T& value, Proj&& proj = Proj{}) {
	//     return find(begin(rng), end(rng), value, forward<Proj>(proj));
	//   }
	//
	// __f<Proj> is an alias for the decayed type that will eventually
	// be used in the target function, and its constraints ensure that
	// the decayed type can in fact be constructed from the actual type.
	//
	template<class T>
		requires Constructible<decay_t<T>, T>
	using __f = decay_t<T>;

	namespace ext {
		///////////////////////////////////////////////////////////////////////////
		// 'structible object concepts
		//
		template<class T>
		STL2_CONCEPT DestructibleObject = Object<T> && Destructible<T>;

		template<class T, class... Args>
		STL2_CONCEPT ConstructibleObject = Object<T> && Constructible<T, Args...>;

		template<class T>
		STL2_CONCEPT DefaultConstructibleObject = Object<T> && DefaultConstructible<T>;

		template<class T>
		STL2_CONCEPT MoveConstructibleObject = Object<T> && MoveConstructible<T>;

		template<class T>
		STL2_CONCEPT CopyConstructibleObject = Object<T> && CopyConstructible<T>;

		///////////////////////////////////////////////////////////////////////////
		// TriviallyFoo concepts
		//
		template<class T>
		STL2_CONCEPT TriviallyDestructible =
			Destructible<T> && _Is<T, is_trivially_destructible>;

		template<class T, class... Args>
		STL2_CONCEPT TriviallyConstructible =
			Constructible<T, Args...> &&
			_Is<T, is_trivially_constructible, Args...>;

		template<class T>
		STL2_CONCEPT TriviallyDefaultConstructible =
			DefaultConstructible<T> &&
			_Is<T, is_trivially_default_constructible>;

		template<class T>
		STL2_CONCEPT TriviallyMoveConstructible =
			MoveConstructible<T> && _Is<T, is_trivially_move_constructible>;

		template<class T>
		STL2_CONCEPT TriviallyCopyConstructible =
			CopyConstructible<T> &&
			TriviallyMoveConstructible<T> &&
			_Is<T, is_trivially_copy_constructible>;

		template<class T>
		STL2_CONCEPT TriviallyMovable =
			Movable<T> &&
			TriviallyMoveConstructible<T> &&
			_Is<T, is_trivially_move_assignable>;

		template<class T>
		STL2_CONCEPT TriviallyCopyable =
			Copyable<T> &&
			TriviallyMovable<T> &&
			TriviallyCopyConstructible<T> &&
			_Is<T, is_trivially_copy_assignable>;
	}
} STL2_CLOSE_NAMESPACE

#endif
