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
#ifndef STL2_DETAIL_HASH_HPP
#define STL2_DETAIL_HASH_HPP

#include <cstddef>
#include <functional>
#include <stl2/detail/fwd.hpp>

///////////////////////////////////////////////////////////////////////////
// Hash machinery.
//
STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// Hashable [Extension]
	// Extremely limited since std::hash is not SFINAE-friendly.
	//
	namespace ext {
		template<class T>
		STL2_CONCEPT Hashable = requires(const T& e) {
			typename std::hash<T>;
			{ std::hash<T>{}(e) } -> std::size_t;
		};
	}

	///////////////////////////////////////////////////////////////////////////
	// hash_combine [Extension]
	// Stolen from Boost.
	//
	namespace ext {
		Hashable{T}
		inline void hash_combine(std::size_t& seed, const T& v) {
			std::hash<T> hasher;
			seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
