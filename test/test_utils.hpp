// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef RANGES_TEST_UTILS_HPP
#define RANGES_TEST_UTILS_HPP

#include <stl2/iterator.hpp>

#include <algorithm>
#include <initializer_list>
#include "./test_iterators.hpp"
#include "./simple_test.hpp"

template <typename Expected, typename Actual>
void has_type(Actual &&)
{
	static_assert(std::is_same<Expected, Actual>::value, "Not the same");
}

template <typename T>
T & as_lvalue(T && t)
{
	return t;
}

template <typename T>
struct checker
{
private:
	std::function<void(std::function<void(T)>)> algo_;
public:
	explicit checker(std::function<void(std::function<void(T)>)> algo)
	: algo_(std::move(algo)) {}

	void check(std::function<void(T)> const & check) const {
		algo_(check);
	}
};

template <bool B, typename T>
typename std::conditional<B, T, T const &>::type rvalue_if(T const &t) {
	return t;
}

template <typename Algo, bool RvalueOK = false>
struct test_range_algo_1
{
private:
	Algo algo_;
public:
	explicit test_range_algo_1(Algo algo)
	  : algo_(algo)
	{}
	template <typename I, typename... Rest>
	auto operator()(I begin, I end, Rest &&... rest) const ->
		checker<decltype(algo_(begin, end, rest...))>
	{
		using R = decltype(algo_(begin, end, rest...));
		return checker<R>{[=](std::function<void(R)> const & check)
		{
			using S = typename sentinel_type<I>::type;
			check(algo_(begin, end, rest...));
			check(algo_(begin, S{base(end)}, rest...));
			check(algo_(::rvalue_if<RvalueOK>(__stl2::ext::subrange(begin, end)), rest...));
			check(algo_(::rvalue_if<RvalueOK>(__stl2::ext::subrange(begin, S{base(end)})), rest...));
		}};
	}
};

template <bool RvalueOK = false, typename Algo>
test_range_algo_1<Algo, RvalueOK> make_testable_1(Algo algo)
{
	return test_range_algo_1<Algo, RvalueOK>{algo};
}

template <typename Algo, bool RvalueOK1 = false, bool RvalueOK2 = false>
struct test_range_algo_2
{
private:
	Algo algo_;
public:
	explicit test_range_algo_2(Algo algo)
	  : algo_(algo)
	{}
	template <typename I1, typename I2, typename... Rest>
	auto operator()(I1 begin1, I1 end1, I2 begin2, I2 end2, Rest &&... rest) const ->
		checker<decltype(algo_(begin1, end1, begin2, end2, rest...))>
	{
		using R = decltype(algo_(begin1, end1, begin2, end2, rest...));
		return checker<R>{[=](std::function<void(R)> const & check)
		{
			using S1 = typename sentinel_type<I1>::type;
			using S2 = typename sentinel_type<I2>::type;
			check(algo_(begin1, end1, begin2, end2, rest...));
			check(algo_(begin1, S1{base(end1)}, begin2, S2{base(end2)}, rest...));
			check(algo_(::rvalue_if<RvalueOK1>(__stl2::ext::subrange(begin1, end1)),
						::rvalue_if<RvalueOK2>(__stl2::ext::subrange(begin2, end2)),
						rest...));
			check(algo_(::rvalue_if<RvalueOK1>(__stl2::ext::subrange(begin1, S1{base(end1)})),
						::rvalue_if<RvalueOK2>(__stl2::ext::subrange(begin2, S2{base(end2)})),
						rest...));
		}};
	}
};

template <bool RvalueOK1 = false, bool RvalueOK2 = false, typename Algo>
test_range_algo_2<Algo, RvalueOK1, RvalueOK2> make_testable_2(Algo algo)
{
	return test_range_algo_2<Algo, RvalueOK1, RvalueOK2>{algo};
}

#endif
