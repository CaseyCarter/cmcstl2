// cmcstl2 - A concept-enabled C++ standard library
// Project home: https://github.com/caseycarter/cmcstl2
//
//  Copyright Casey Carter
//
// SPDX-License-Identifier: (BSL-1.0 OR Apache-2.0 WITH LLVM-exception)
//
#ifndef STL2_DETAIL_ALGORITHM_RESULTS_HPP
#define STL2_DETAIL_ALGORITHM_RESULTS_HPP

#include <utility>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	template<class I, class O>
	struct __in_out_result {
		STL2_NO_UNIQUE_ADDRESS I in;
		STL2_NO_UNIQUE_ADDRESS O out;

		// Extension: the dangling story actually works.
		template<class I2, class O2>
		requires convertible_to<const I&, I2> && convertible_to<const O&, O2>
		operator __in_out_result<I2, O2>() const& {
			return {in, out};
		}
		template<class I2, class O2>
		requires convertible_to<I, I2> && convertible_to<O, O2>
		operator __in_out_result<I2, O2>() && {
			return {std::move(in), std::move(out)};
		}
	};

	template<class I1, class I2>
	struct __in_in_result {
		STL2_NO_UNIQUE_ADDRESS I1 in1;
		STL2_NO_UNIQUE_ADDRESS I2 in2;

		// Extension: the dangling story actually works.
		template<class II1, class II2>
		requires convertible_to<const I1&, II1> && convertible_to<const I2&, II2>
		operator __in_in_result<II1, II2>() const& {
			return {in1, in2};
		}
		template<class II1, class II2>
		requires convertible_to<I1, II1> && convertible_to<I2, II2>
		operator __in_in_result<II1, II2>() && {
			return {std::move(in1), std::move(in2)};
		}
	};

	template<class I1, class I2, class O>
	struct __in_in_out_result {
		STL2_NO_UNIQUE_ADDRESS I1 in1;
		STL2_NO_UNIQUE_ADDRESS I2 in2;
		STL2_NO_UNIQUE_ADDRESS O out;

		// Extension: the dangling story actually works.
		template<class II1, class II2, class OO>
		requires convertible_to<const I1&, II1> &&
			convertible_to<const I2&, II2> && convertible_to<const O&, OO>
		operator __in_in_out_result<II1, II2, OO>() const& {
			return {in1, in2, out};
		}
		template<class II1, class II2, class OO>
		requires convertible_to<I1, II1> &&
			convertible_to<I2, II2> && convertible_to<O, OO>
		operator __in_in_out_result<II1, II2, OO>() && {
			return {std::move(in1), std::move(in2), std::move(out)};
		}
	};

	template<class I, class O1, class O2>
	struct __in_out_out_result {
		STL2_NO_UNIQUE_ADDRESS I in;
		STL2_NO_UNIQUE_ADDRESS O1 out1;
		STL2_NO_UNIQUE_ADDRESS O2 out2;

		// Extension: the dangling story actually works.
		template<class II, class OO1, class OO2>
		requires convertible_to<const I&, II> &&
			convertible_to<const O1&, OO1> && convertible_to<const O2&, OO2>
		operator __in_out_out_result<II, OO1, OO2>() const& {
			return {in, out1, out2};
		}
		template<class II, class OO1, class OO2>
		requires convertible_to<I, II> &&
			convertible_to<O1, OO1> && convertible_to<O2, OO2>
		operator __in_out_out_result<II, OO1, OO2>() && {
			return {std::move(in), std::move(out1), std::move(out2)};
		}
	};

	template<class I, class F>
	struct __in_fun_result {
		STL2_NO_UNIQUE_ADDRESS I in;
		STL2_NO_UNIQUE_ADDRESS F fun;

		// Extension: the dangling story actually works.
		template<class I2, class F2>
		requires convertible_to<const I&, I2> && convertible_to<const F&, F2>
		operator __in_fun_result<I2, F2>() const& {
			return {in, fun};
		}
		template<class I2, class F2>
		requires convertible_to<I, I2> && convertible_to<F, F2>
		operator __in_fun_result<I2, F2>() && {
			return {std::move(in), std::move(fun)};
		}
	};

	template<class T>
	struct minmax_result {
		STL2_NO_UNIQUE_ADDRESS T min;
		STL2_NO_UNIQUE_ADDRESS T max;

		// Extension: the dangling story actually works.
		template<class T2>
		requires convertible_to<const T&, T2>
		operator minmax_result<T2>() const& {
			return {min, max};
		}
		template<class T2>
		requires convertible_to<T, T2>
		operator minmax_result<T2>() && {
			return {std::move(min), std::move(max)};
		}
	};
} STL2_CLOSE_NAMESPACE

#endif
