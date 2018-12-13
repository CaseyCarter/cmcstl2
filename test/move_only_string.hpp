// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Eric Niebler
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_MOVE_ONLY_STRING_HPP
#define STL2_MOVE_ONLY_STRING_HPP

#include <cstring>
#include <stl2/detail/swap.hpp>

namespace cmcstl2_test {
	namespace ranges = __stl2;
	// a simple type to test move semantics
	struct move_only_string {
		move_only_string(char const* sz = "") noexcept
		: sz_(sz)
		, length_(std::strlen(sz))
		{}
		move_only_string(move_only_string&& that) noexcept
		: sz_(ranges::exchange(that.sz_, ""))
		, length_(ranges::exchange(that.length_, {}))
		{}
		move_only_string(move_only_string const&) = delete;
		move_only_string& operator=(move_only_string&& that) noexcept
		{
			*this = ranges::exchange(that, {});
			return *this;
		}
		move_only_string& operator=(move_only_string const&) = delete;

		char operator[](const std::size_t n) noexcept
		{ return subscript_impl(*this, n); }

		char operator[](const std::size_t n) const noexcept
		{ return subscript_impl(*this, n); }

		friend bool operator==(const move_only_string& x, const move_only_string& y) noexcept
		{  return 0 == std::strcmp(x.sz_, y.sz_); }

		friend bool operator!=(const move_only_string& x, const move_only_string& y) noexcept
		{ return !(x == y); }

		friend bool operator<(const move_only_string& x, const move_only_string& y) noexcept
		{ return std::strcmp(x.sz_, y.sz_) < 0; }

		friend bool operator<=(const move_only_string& x, const move_only_string& y) noexcept
		{ return !(y < x); }

		friend bool operator>=(const move_only_string& x, const move_only_string& y) noexcept
		{ return !(x < y); }

		friend bool operator>(const move_only_string& x, const move_only_string& y) noexcept
		{ return y < x; }

		friend std::ostream& operator<<(std::ostream& sout, move_only_string const& str)
		{ return sout << '"' << str.sz_ << '"'; }
	private:
		char const* sz_;
		std::size_t length_;

		template<class Self>
		static char subscript_impl(Self& self, const std::size_t n)
		{
			STL2_EXPECT(n < self.length_);
			return self.sz_[self.length_];
		}
	};
} // namespace cmcstl2_test

#endif // STL2_MOVE_ONLY_STRING_HPP
