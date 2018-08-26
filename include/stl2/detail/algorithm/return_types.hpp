// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Eric Niebler
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_RETURN_TYPES_HPP
#define STL2_DETAIL_ALGORITHM_RETURN_TYPES_HPP

#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	template<class I1, class I2>
	struct __in1_in2_result {
		I1 in1;
		I2 in2;
	};

	template<class I, class O>
	struct __in_out_result {
		I in;
		O out;
	};

	template<class I1, class I2, class O>
	struct __in1_in2_out_result {
		I1 in1;
		I2 in2;
		O out;
	};

	template<class I, class O1, class O2>
	struct __in_out1_out2_result {
		I in;
		O1 out1;
		O2 out2;
	};

	template<class I, class Fun>
	struct __in_fun_result {
		I in;
		Fun fun;
	};

	template<class T>
	struct minmax_result {
		T min;
		T max;
	};
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_ALGORITHM_RETURN_TYPES_HPP
