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
    template <class T>
    constexpr bool __stream_extractable = false;
    template <class T>
      requires requires (std::istream& is, T& t) {
        STL2_EXACT_TYPE_CONSTRAINT(is >> t, std::istream&);
        // Axiom: &is == &(is << t)
      }
    constexpr bool __stream_extractable<T> = true;

    template <class T>
    concept bool StreamExtractable = __stream_extractable<T>;
  }

  namespace models {
    template <class>
    constexpr bool StreamExtractable = false;
    __stl2::ext::StreamExtractable{T}
    constexpr bool StreamExtractable<T> = true;
  }

  ///////////////////////////////////////////////////////////////////////////
  // StreamInsertable [Extension]
  //
  namespace ext {
    template <class T, class Stream = std::basic_ostream<char, std::char_traits<char>>>
    constexpr bool __stream_insertable = false;
    template <class T, class Stream>
      requires requires (Stream& os, const T& t) {
        STL2_EXACT_TYPE_CONSTRAINT(os << t, Stream&);
        // Axiom: &os == &(os << t)
      }
    constexpr bool __stream_insertable<T, Stream> = true;

    template <class T, class Stream = std::basic_ostream<char, std::char_traits<char>>>
    concept bool StreamInsertable = __stream_insertable<T, Stream>;
  }

  namespace models {
    template <class, class Stream = std::basic_ostream<char, std::char_traits<char>>>
    constexpr bool StreamInsertable = false;
    __stl2::ext::StreamInsertable{T, Stream}
    constexpr bool StreamInsertable<T, Stream> = true;
  }
} STL2_CLOSE_NAMESPACE

#endif
