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
} STL2_CLOSE_NAMESPACE

#endif
