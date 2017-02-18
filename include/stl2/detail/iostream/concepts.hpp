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
#ifndef STL2_DETAIL_IOSTREAM_CONCEPTS_HPP
#define STL2_DETAIL_IOSTREAM_CONCEPTS_HPP

#include <iosfwd>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// StreamExtractable [Extension]
	//
	namespace ext {
		template <class T, class Stream = std::istream>
		concept bool StreamExtractable =
			requires(Stream& is, T& t) {
				{ is >> t } -> Same<Stream&>&&;
				// Axiom: &is == &(is << t)
			};
	}

	namespace models {
		template <class, class Stream = std::istream>
		constexpr bool StreamExtractable = false;
		__stl2::ext::StreamExtractable{T, Stream}
		constexpr bool StreamExtractable<T, Stream> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// StreamInsertable [Extension]
	//
	namespace ext {
		template <class T, class Stream = std::ostream>
		concept bool StreamInsertable =
			requires(Stream& os, const T& t) {
				{ os << t } -> Same<Stream&>&&;
				// Axiom: &os == &(os << t)
			};
	}

	namespace models {
		template <class, class Stream = std::ostream>
		constexpr bool StreamInsertable = false;
		__stl2::ext::StreamInsertable{T, Stream}
		constexpr bool StreamInsertable<T, Stream> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
