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
	template <class T, class charT = char, class traits = std::char_traits<charT>>
	concept bool StreamExtractable =
		requires(std::basic_istream<charT, traits>& is, T& t) {
			{ is >> t } -> Same<std::basic_istream<charT, traits>>&;
			// Axiom: &is == &(is << t)
		};

	namespace models {
		template <class, class charT = char, class traits = std::char_traits<charT>>
		constexpr bool StreamExtractable = false;
		__stl2::StreamExtractable{T, charT, traits}
		constexpr bool StreamExtractable<T, charT, traits> = true;
	}

	///////////////////////////////////////////////////////////////////////////
	// StreamInsertable [Extension]
	//
	template <class T, class charT = char, class traits = std::char_traits<charT>>
	concept bool StreamInsertable =
		requires(std::basic_ostream<charT, traits>& os, const T& t) {
			{ os << t } -> Same<std::basic_ostream<charT, traits>>&;
			// Axiom: &os == &(os << t)
		};

	namespace models {
		template <class, class charT = char, class traits = std::char_traits<charT>>
		constexpr bool StreamInsertable = false;
		__stl2::StreamInsertable{T, charT, traits}
		constexpr bool StreamInsertable<T, charT, traits> = true;
	}
} STL2_CLOSE_NAMESPACE

#endif
