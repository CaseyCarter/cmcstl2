#ifndef STL2_DETAIL_HASH_HPP
#define STL2_DETAIL_HASH_HPP

#include <cstddef>
#include <functional>
#include <stl2/detail/fwd.hpp>

///////////////////////////////////////////////////////////////////////////
// Hash machinery.
//
namespace stl2 {
  inline namespace v1 {
    namespace __hash {
      template <class T>
      concept bool Hashable =
        requires (const T& e) {
          typename std::hash<T>;
          { std::hash<T>{}(e) } -> std::size_t;
        };

      Hashable{T}
      inline void combine(std::size_t& seed, const T& v) {
        // Lifted from Boost.
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
      }
    }
  }
}

#endif
