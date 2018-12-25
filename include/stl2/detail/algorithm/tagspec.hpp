// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_TAGSPEC_HPP
#define STL2_DETAIL_ALGORITHM_TAGSPEC_HPP

#include <utility>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/concepts/core.hpp>

STL2_OPEN_NAMESPACE {
	// tag specifiers [alg.tagspec]
	namespace tag {
		STL2_DEFINE_GETTER(in)
		STL2_DEFINE_GETTER(in1)
		STL2_DEFINE_GETTER(in2)
		STL2_DEFINE_GETTER(out)
		STL2_DEFINE_GETTER(out1)
		STL2_DEFINE_GETTER(out2)
		STL2_DEFINE_GETTER(fun)
		STL2_DEFINE_GETTER(min)
		STL2_DEFINE_GETTER(max)
		STL2_DEFINE_GETTER(begin)
		STL2_DEFINE_GETTER(end)
		STL2_DEFINE_GETTER(count) // Extension
	}

	template<class I, class O>
	struct __in_out_result {
		I in;
		O out;

		// Extension: the dangling story actually works.
		template<class I2, class O2>
		requires ConvertibleTo<const I&, I2> && ConvertibleTo<const O&, O2>
		operator __in_out_result<I2, O2>() const& {
			return {in, out};
		}
		template<class I2, class O2>
		requires ConvertibleTo<I, I2> && ConvertibleTo<O, O2>
		operator __in_out_result<I2, O2>() && {
			return {std::move(in), std::move(out)};
		}
	};

	template<class I1, class I2>
	struct __in_in_result {
		I1 in1;
		I2 in2;

		// Extension: the dangling story actually works.
		template<class II1, class II2>
		requires ConvertibleTo<const I1&, II1> && ConvertibleTo<const I2&, II2>
		operator __in_in_result<II1, II2>() const& {
			return {in1, in2};
		}
		template<class II1, class II2>
		requires ConvertibleTo<I1, II1> && ConvertibleTo<I2, II2>
		operator __in_in_result<II1, II2>() && {
			return {std::move(in1), std::move(in2)};
		}
	};

	template<class I1, class I2, class O>
	struct __in_in_out_result {
		I1 in1;
		I2 in2;
		O out;

		// Extension: the dangling story actually works.
		template<class II1, class II2, class OO>
		requires ConvertibleTo<const I1&, II1> &&
			ConvertibleTo<const I2&, II2> && ConvertibleTo<const O&, OO>
		operator __in_in_out_result<II1, II2, OO>() const& {
			return {in1, in2, out};
		}
		template<class II1, class II2, class OO>
		requires ConvertibleTo<I1, II1> &&
			ConvertibleTo<I2, II2> && ConvertibleTo<O, OO>
		operator __in_in_out_result<II1, II2, OO>() && {
			return {std::move(in1), std::move(in2), std::move(out)};
		}
	};

	template<class I, class O1, class O2>
	struct __in_out_out_result {
		I in;
		O1 out1;
		O2 out2;

		// Extension: the dangling story actually works.
		template<class II, class OO1, class OO2>
		requires ConvertibleTo<const I&, II> &&
			ConvertibleTo<const O1&, OO1> && ConvertibleTo<const O2&, OO2>
		operator __in_out_out_result<II, OO1, OO2>() const& {
			return {in, out1, out2};
		}
		template<class II, class OO1, class OO2>
		requires ConvertibleTo<I, II> &&
			ConvertibleTo<O1, OO1> && ConvertibleTo<O2, OO2>
		operator __in_out_out_result<II, OO1, OO2>() && {
			return {std::move(in), std::move(out1), std::move(out2)};
		}
	};

	template<class I, class F>
	struct __in_fun_result {
		I in;
		F fun;

		// Extension: the dangling story actually works.
		template<class I2, class F2>
		requires ConvertibleTo<const I&, I2> && ConvertibleTo<const F&, F2>
		operator __in_fun_result<I2, F2>() const& {
			return {in, fun};
		}
		template<class I2, class F2>
		requires ConvertibleTo<I, I2> && ConvertibleTo<F, F2>
		operator __in_fun_result<I2, F2>() && {
			return {std::move(in), std::move(fun)};
		}
	};

	template<class T>
	struct minmax_result {
		T min;
		T max;

		// Extension: the dangling story actually works.
		template<class T2>
		requires ConvertibleTo<const T&, T2>
		operator minmax_result<T2>() const& {
			return {min, max};
		}
		template<class T2>
		requires ConvertibleTo<T, T2>
		operator minmax_result<T2>() && {
			return {std::move(min), std::move(max)};
		}
	};
} STL2_CLOSE_NAMESPACE

#endif
