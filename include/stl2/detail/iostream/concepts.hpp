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
	template<class T, class charT = char, class traits = std::char_traits<charT>>
	META_CONCEPT StreamExtractable =
		requires(std::basic_istream<charT, traits>& is, T& t) {
#if STL2_BROKEN_COMPOUND_REQUIREMENT
			is >> t; requires Same<decltype(is >> t), std::basic_istream<charT, traits>&>;
#else
			{ is >> t } -> Same<std::basic_istream<charT, traits>&>;
#endif // STL2_BROKEN_COMPOUND_REQUIREMENT
			// Axiom: &is == &(is << t)
		};

	///////////////////////////////////////////////////////////////////////////
	// StreamInsertable [Extension]
	//
	template<class T, class charT = char, class traits = std::char_traits<charT>>
	META_CONCEPT StreamInsertable =
		requires(std::basic_ostream<charT, traits>& os, const T& t) {
#if STL2_BROKEN_COMPOUND_REQUIREMENT
			os << t; requires Same<decltype(os << t), std::basic_ostream<charT, traits>&>;
#else
			{ os << t } -> Same<std::basic_ostream<charT, traits>&>;
#endif // STL2_BROKEN_COMPOUND_REQUIREMENT
			// Axiom: &os == &(os << t)
		};
} STL2_CLOSE_NAMESPACE

#endif
