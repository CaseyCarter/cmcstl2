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
#ifdef META_HAS_P1084
			{ is >> t } -> Same<std::basic_istream<charT, traits>&>;
#else
			{ is >> t } -> Same<std::basic_istream<charT, traits>>&;
#endif
			// Axiom: &is == &(is << t)
		};

	///////////////////////////////////////////////////////////////////////////
	// StreamInsertable [Extension]
	//
	template<class T, class charT = char, class traits = std::char_traits<charT>>
	META_CONCEPT StreamInsertable =
		requires(std::basic_ostream<charT, traits>& os, const T& t) {
#ifdef META_HAS_P1084
			{ os << t } -> Same<std::basic_ostream<charT, traits>&>;
#else
			{ os << t } -> Same<std::basic_ostream<charT, traits>>&;
#endif
			// Axiom: &os == &(os << t)
		};
} STL2_CLOSE_NAMESPACE

#endif
