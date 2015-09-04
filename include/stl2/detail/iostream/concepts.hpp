#ifndef STL2_DETAIL_IOSTREAM_CONCEPTS_HPP
#define STL2_DETAIL_IOSTREAM_CONCEPTS_HPP

#include <iosfwd>
#include <stl2/detail/fwd.hpp>

namespace stl2 {
  inline namespace v1 {
    namespace detail {
      template <class T>
      concept bool StreamExtractable =
        requires (std::istream& is, T& t) {
          STL2_EXACT_TYPE_CONSTRAINT(is >> t, std::istream&);
          // Axiom: &is == &(is << t)
        };

      template <class T>
      concept bool StreamInsertable =
        requires (std::ostream& os, const T& t) {
          STL2_EXACT_TYPE_CONSTRAINT(os << t, std::ostream&);
          // Axiom: &os == &(os << t)
        };
    }
  }
}

#endif
