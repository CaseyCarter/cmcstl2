// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include "Book.hpp"
#include <cassert>
#include "raii.hpp"
#include <stl2/algorithm.hpp>
#include <stl2/detail/memory/uninitialized_default_construct.hpp>
#include <stl2/detail/memory/destroy.hpp>

namespace ranges = __stl2;

class Construct {
public:
   Construct()
   {
      ++instantiated;
   }

   Construct(const Construct&)
   {
      ++instantiated;
   }

   Construct(Construct&&) noexcept
   {
      ++instantiated;
   }

   ~Construct()
   {
      --instantiated;
   }

   static int get_instantiated()
   {
      return instantiated;
   }

   Construct& operator=(Construct&&) noexcept = default;
private:
   static int instantiated;
};

int Construct::instantiated{0};

int main()
{
   auto independent = raii<Construct>{1 << 20};
   auto test = [&independent](const auto& p) {
      assert(Construct::get_instantiated() == 0);
      assert(p == independent.end());
      assert(Construct::get_instantiated() == 0);
   };

   ranges::uninitialized_default_construct(independent);
   test(ranges::destroy(independent.begin(), independent.end()));

   ranges::uninitialized_default_construct(independent);
   test(ranges::destroy(independent.cbegin(), independent.cend()));

   ranges::uninitialized_default_construct(independent);
   test(ranges::destroy(independent));

   ranges::uninitialized_default_construct(independent);
   test(ranges::destroy_n(independent.begin(), independent.size()));

   ranges::uninitialized_default_construct(independent);
   test(ranges::destroy_n(independent.cbegin(), independent.size()));
}