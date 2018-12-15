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
	//     requires IndirectRelation<equal_to,
	//                projected<iterator_t<Rng>, Proj>, const T*>()
	//   safe_iterator_t<Rng>
	//   find(Rng&& rng, const T& value, Proj proj = {});
	//
	// can be implemented to perfect-forward to the iterator overload as:
	//
	//   template<InputRange Rng, class T, class Proj = identity>
	//     requires IndirectRelation<equal_to,
	//                projected<iterator_t<Rng>, __f<Proj>>, // NW: __f<Proj>
	//                const T*>()
	//   safe_iterator_t<Rng>
	//   find(Rng&& rng, const T& value, Proj&& proj = {}) {
	//     return find(begin(rng), end(rng), value, forward<Proj>(proj));
	//   }
	//
	// __f<Proj> is an alias for the decayed type that will eventually
	// be used in the target function, and its constraints ensure that
	// the decayed type can in fact be constructed from the actual type.
	//
	template<class T>
		requires Constructible<std::decay_t<T>, T>
	using __f = std::decay_t<T>;

	namespace ext {
		///////////////////////////////////////////////////////////////////////////
		// 'structible object concepts
		//
		template<class T>
		META_CONCEPT DestructibleObject = Object<T> && Destructible<T>;

		template<class T, class... Args>
		META_CONCEPT ConstructibleObject = Object<T> && Constructible<T, Args...>;

		template<class T>
		META_CONCEPT DefaultConstructibleObject = Object<T> && DefaultConstructible<T>;

		template<class T>
		META_CONCEPT MoveConstructibleObject = Object<T> && MoveConstructible<T>;

		template<class T>
		META_CONCEPT CopyConstructibleObject = Object<T> && CopyConstructible<T>;

		///////////////////////////////////////////////////////////////////////////
		// TriviallyFoo concepts
		//
		template<class T>
		META_CONCEPT TriviallyDestructible =
			Destructible<T> && std::is_trivially_destructible_v<T>;

		template<class T, class... Args>
		META_CONCEPT TriviallyConstructible =
			Constructible<T, Args...> &&
			std::is_trivially_constructible_v<T, Args...>;

		template<class T>
		META_CONCEPT TriviallyDefaultConstructible =
			DefaultConstructible<T> &&
			std::is_trivially_default_constructible_v<T>;

		template<class T>
		META_CONCEPT TriviallyMoveConstructible =
			MoveConstructible<T> && std::is_trivially_move_constructible_v<T>;

		template<class T>
		META_CONCEPT TriviallyCopyConstructible =
			CopyConstructible<T> &&
			TriviallyMoveConstructible<T> &&
			std::is_trivially_copy_constructible_v<T>;

		template<class T>
		META_CONCEPT TriviallyMovable =
			Movable<T> &&
			TriviallyMoveConstructible<T> &&
			std::is_trivially_move_assignable_v<T>;

		template<class T>
		META_CONCEPT TriviallyCopyable =
			Copyable<T> &&
			TriviallyMovable<T> &&
			TriviallyCopyConstructible<T> &&
			std::is_trivially_copy_assignable_v<T>;
	}
} STL2_CLOSE_NAMESPACE

#endif
