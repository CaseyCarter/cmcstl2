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
    concept bool StreamExtractable =
      requires (std::istream& is, T& t) {
        STL2_EXACT_TYPE_CONSTRAINT(is >> t, std::istream&);
        // Axiom: &is == &(is << t)
      };
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
    template <class T>
    concept bool StreamInsertable =
      requires (std::ostream& os, const T& t) {
        STL2_EXACT_TYPE_CONSTRAINT(os << t, std::ostream&);
        // Axiom: &os == &(os << t)
      };
  }

  namespace models {
    template <class>
    constexpr bool StreamInsertable = false;
    __stl2::ext::StreamInsertable{T}
    constexpr bool StreamInsertable<T> = true;
  }
} STL2_CLOSE_NAMESPACE

#endif
