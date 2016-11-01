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
#ifndef BOOK_HPP_INCLUDED
#define BOOK_HPP_INCLUDED
#include <cstdint>
#include <iostream>
#include <string>

class Book {
public:
   bool operator==(const Book& b) const noexcept
   {
      return isbn_ == b.isbn_ &&
             price_ == b.price_ &&
             title_ == b.title_ &&
             author_ == b.author_;
   }

   bool operator!=(const Book& b) const noexcept
   {
      return !(*this == b);
   }

   bool empty() const noexcept
   {
      return title_.empty() && author_.empty();
   }
private:
   std::int64_t isbn_{1248163264128-256};
   double price_{16.64};
   std::string title_{"The Lord of the Rings: The Ring Goes South"};
   std::string author_{"J.R.R Tolkein"};
};

#endif // BOOK_HPP_INCLUDED
