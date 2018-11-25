// Range v3 library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef STL2_SIMPLE_TEST_HPP
#define STL2_SIMPLE_TEST_HPP

#include <cstdlib>
#include <utility>
#include <iostream>
#include <typeinfo>

#if VALIDATE_RANGES
#include <range/v3/begin_end.hpp>
namespace NS = ::ranges;
#else
#include <stl2/iterator.hpp>
namespace NS = ::__stl2;
#endif

namespace test_impl
{
	inline int &test_failures()
	{
		static int test_failures = 0;
		return test_failures;
	}

	template<typename T>
	struct streamable_base
	{};

	template<typename T>
	std::ostream &operator<<(std::ostream &sout, streamable_base<T> const &)
	{
		return sout << "<non-streamable type>";
	}

	template<typename T>
	struct streamable : streamable_base<T>
	{
	private:
		T const &t_;
	public:
		explicit streamable(T const &t) : t_(t) {}
		template<typename U = T>
		friend auto operator<<(std::ostream &sout, streamable const &s) ->
			decltype(sout << std::declval<U const &>())
		{
			return sout << s.t_;
		}
	};

	template<typename T>
	streamable<T> stream(T const &t)
	{
		return streamable<T>{t};
	}

	template<typename T>
	struct R
	{
	private:
		char const *filename_;
		char const *expr_;
		char const *func_;
		T t_;
		int lineno_;
		bool dismissed_ = false;

		template<typename U>
		void oops(U const &u) const
		{
			std::cerr
				<< "> ERROR: CHECK failed \"" << expr_ << "\"\n"
				<< "> \t" << filename_ << '(' << lineno_ << ')' << '\n'
				<< "> \t in function \"" << func_ << "\"\n";
			if(dismissed_)
				std::cerr
					<< "> \tEXPECTED: " << stream(u) << "\n> \tACTUAL: " << stream(t_) << '\n';
			++test_failures();
		}
		void dismiss()
		{
			dismissed_ = true;
		}
		template<typename V = T>
		auto eval_(int) -> decltype(!std::declval<V>())
		{
			return !t_;
		}
		bool eval_(long)
		{
			return true;
		}
	public:
		R(char const *filename, int lineno, char const *expr, const char* func, T && t)
		  : filename_(filename), expr_(expr), func_(func)
		  , t_(std::forward<T>(t)), lineno_(lineno)
		{}
		R(R const&) = delete;
		R& operator=(R const&) = delete;
		~R()
		{
			if(!dismissed_ && eval_(42))
				this->oops(42);
		}
		template<typename U>
		void operator==(U const &u)
		{
			dismiss();
			if(!(t_ == u)) this->oops(u);
		}
		template<typename U>
		void operator!=(U const &u)
		{
			dismiss();
			if(!(t_ != u)) this->oops(u);
		}
		template<typename U>
		void operator<(U const &u)
		{
			dismiss();
			if(!(t_ < u)) this->oops(u);
		}
		template<typename U>
		void operator<=(U const &u)
		{
			dismiss();
			if(!(t_ <= u)) this->oops(u);
		}
		template<typename U>
		void operator>(U const &u)
		{
			dismiss();
			if(!(t_ > u)) this->oops(u);
		}
		template<typename U>
		void operator>=(U const &u)
		{
			dismiss();
			if(!(t_ >= u)) this->oops(u);
		}
	};

	struct S
	{
	private:
		char const *filename_;
		char const *expr_;
		char const *func_;
		int lineno_;
	public:
		S(char const *filename, int lineno, char const *expr, char const *func)
		  : filename_(filename), expr_(expr), func_(func), lineno_(lineno)
		{}
		template<typename T>
		R<T> operator->*(T && t)
		{
			return {filename_, lineno_, expr_, func_, std::forward<T>(t)};
		}
	};
}

inline int test_result()
{
	return ::test_impl::test_failures() ? EXIT_FAILURE : EXIT_SUCCESS;
}

#define CHECK(...)                                                                                 \
	(void)(::test_impl::S{__FILE__, __LINE__, #__VA_ARGS__, __PRETTY_FUNCTION__} ->* __VA_ARGS__)  \
	/**/

template<typename Rng, typename Rng2>
constexpr void check_equal_(const char* file, int line, const char* lhs, const char* rhs,
	const char* fun, Rng && actual, Rng2&& expected)
{
	auto begin0 = NS::begin(actual);
	auto end0 = NS::end(actual);
	auto begin1 = NS::begin(expected);
	auto end1 = NS::end(expected);

	for(std::size_t i = 0; begin0 != end0 && begin1 != end1; ++begin0, (void)++i, ++begin1) {
		if (*begin0 != *begin1) {
			std::cerr <<
				"> ERROR: CHECK failed \"" << lhs << '[' << i << "] != " << rhs << '[' << i << "]\"\n"
				"> \t" << file << '(' << line << ')' << "\n"
				"> \t in function \"" << fun << "\"\n"
				"> \tEXPECTED: " << test_impl::stream(*begin1) << "\n"
				"> \tACTUAL: " << test_impl::stream(*begin0) << '\n';
			++test_impl::test_failures();
		}
	}

	if (!(begin0 == end0)) {
		std::cerr <<
			"> ERROR: CHECK failed \"begin0 != end0\"\n"
			"> \t" << file << '(' << line << ')' << "\n"
			"> \t in function \"" << fun << "\"\n"
			"> \tEXPECTED: " << test_impl::stream(end0) << "\n"
			"> \tACTUAL: " << test_impl::stream(begin0) << '\n';
		++test_impl::test_failures();
	}

	if (!(begin1 == end1)) {
		std::cerr <<
			"> ERROR: CHECK failed \"begin1 != end1\"\n"
			"> \t" << file << '(' << line << ')' << "\n"
			"> \t in function \"" << fun << "\"\n"
			"> \tEXPECTED: " << test_impl::stream(end1) << "\n"
			"> \tACTUAL: " << test_impl::stream(begin1) << '\n';
		++test_impl::test_failures();
	}
}

template<typename Val, typename Rng>
constexpr void check_equal_(const char* file, int line, const char* lhs, const char* rhs,
	const char* fun, Rng && actual, std::initializer_list<Val>&& expected)
{
	check_equal_(file, line, lhs, rhs, fun, actual, expected);
}

#define CHECK_EQUAL(first, ...) \
	check_equal_(__FILE__, __LINE__, #first, #__VA_ARGS__, __PRETTY_FUNCTION__, first, __VA_ARGS__) \
	/**/

#endif
