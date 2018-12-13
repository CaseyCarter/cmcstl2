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
// Copyright (c) 2009 Alexander Stepanov and Paul McJones
//
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation. The authors make no
// representations about the suitability of this software for any
// purpose. It is provided "as is" without express or implied
// warranty.
//
// Algorithms from
// Elements of Programming
// by Alexander Stepanov and Paul McJones
// Addison-Wesley Professional, 2009
//
#ifndef STL2_DETAIL_ALGORITHM_FORWARD_SORT_HPP
#define STL2_DETAIL_ALGORITHM_FORWARD_SORT_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/temporary_vector.hpp>
#include <stl2/detail/algorithm/lower_bound.hpp>
#include <stl2/detail/algorithm/merge.hpp>
#include <stl2/detail/algorithm/rotate.hpp>
#include <stl2/detail/algorithm/upper_bound.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// sort [Extension]
//
// Algorithm for forward iterators from EoP.
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		struct __fsort_n_fn {
			template<class I, class Comp = less, class Proj = identity>
			requires Sortable<I, Comp, Proj>
			I operator()(I first, const iter_difference_t<I> n,
				Comp comp = {}, Proj proj = {}) const
			{
				STL2_EXPECT(0 <= n);
				auto ufirst = ext::uncounted(first);
				static_assert(Same<iter_value_t<I>, iter_value_t<decltype(ufirst)>>);
				using buf_t = temporary_buffer<iter_value_t<I>>;
				// TODO: tune this threshold.
				auto buf = n / 2 >= 16 ? buf_t{n / 2} : buf_t{};
				auto last = sort_n_adaptive(std::move(ufirst), n,
					buf, comp, proj);
				return ext::recounted(first, std::move(last), n);
			}
		private:
			template<class I>
			using buf_t = temporary_buffer<iter_value_t<I>>;

			template<class I, class Comp, class Proj>
			requires Sortable<I, Comp, Proj>
			static I merge_n_with_buffer(I f0, iter_difference_t<I> n0,
				I f1, iter_difference_t<I> n1,
				buf_t<I>& buf, Comp& comp, Proj& proj)
			{
				STL2_EXPECT(0 <= n0);
				STL2_EXPECT(0 <= n1);
				STL2_EXPECT(n0 <= buf.size());
				auto&& vec = detail::make_temporary_vector(buf);
				__stl2::move(counted_iterator{f0, n0},
					default_sentinel{}, __stl2::back_inserter(vec));
				return __stl2::merge(
					__stl2::make_move_iterator(begin(vec)),
					__stl2::make_move_iterator(end(vec)),
					__stl2::make_move_iterator(counted_iterator{std::move(f1), n1}),
					move_sentinel<default_sentinel>{},
					std::move(f0), __stl2::ref(comp),
					__stl2::ref(proj), __stl2::ref(proj)).out();
			}

			template<class I, class Comp, class Proj>
			requires Sortable<I, Comp, Proj>
			static constexpr void merge_n_step_0(I f0, iter_difference_t<I> n0,
				I f1, iter_difference_t<I> n1,
				Comp& comp, Proj& proj,
				I& f0_0, iter_difference_t<I>& n0_0,
				I& f0_1, iter_difference_t<I>& n0_1,
				I& f1_0, iter_difference_t<I>& n1_0,
				I& f1_1, iter_difference_t<I>& n1_1)
			{
				STL2_EXPECT(0 <= n0);
				STL2_EXPECT(0 <= n1);
				f0_0 = f0;
				n0_0 = n0 / 2;
				f0_1 = next(f0_0, n0_0);
				f1_1 = __stl2::ext::lower_bound_n(f1, n1, __stl2::invoke(proj, *f0_1),
					__stl2::ref(comp), __stl2::ref(proj));
				f1_0 = __stl2::rotate(f0_1, f1, f1_1).begin();
				n0_1 = distance(f0_1, f1_0);
				f1_0 = next(f1_0);
				n1_0 = n0 - n0_0 - 1;
				n1_1 = n1 - n0_1;
			}

			template<class I, class Comp, class Proj>
			requires Sortable<I, Comp, Proj>
			static constexpr void merge_n_step_1(I f0, iter_difference_t<I> n0,
				I f1, iter_difference_t<I> n1,
				Comp& comp, Proj& proj,
				I& f0_0, iter_difference_t<I>& n0_0,
				I& f0_1, iter_difference_t<I>& n0_1,
				I& f1_0, iter_difference_t<I>& n1_0,
				I& f1_1, iter_difference_t<I>& n1_1)
			{
				STL2_EXPECT(0 <= n0);
				STL2_EXPECT(0 <= n1);
				f0_0 = f0;
				n0_1 = n1 / 2;
				f1_1 = next(f1, n0_1);
				f0_1 = __stl2::ext::upper_bound_n(f0, n0, __stl2::invoke(proj, *f1_1),
					__stl2::ref(comp), __stl2::ref(proj));
				f1_1 = next(f1_1);
				f1_0 = __stl2::rotate(f0_1, f1, f1_1).begin();
				n0_0 = distance(f0_0, f0_1);
				n1_0 = n0 - n0_0;
				n1_1 = n1 - n0_1 - 1;
			}

			template<class I, class Comp, class Proj>
			requires Sortable<I, Comp, Proj>
			static I merge_n_adaptive(I f0, iter_difference_t<I> n0,
				I f1, iter_difference_t<I> n1,
				buf_t<I>& buf, Comp& comp, Proj& proj)
			{
				STL2_EXPECT(0 <= n0);
				STL2_EXPECT(0 <= n1);
				if (!n0 || !n1) {
					return next(f0, n0 + n1);
				}
				if (n0 <= buf.size()) {
					return merge_n_with_buffer(f0, n0, f1, n1, buf, comp, proj);
				}
				I f0_0, f0_1, f1_0, f1_1;
				iter_difference_t<I> n0_0, n0_1, n1_0, n1_1;

				if (n0 < n1) {
					merge_n_step_0(f0, n0, f1, n1, comp, proj,
						f0_0, n0_0, f0_1, n0_1,
						f1_0, n1_0, f1_1, n1_1);
				} else {
					merge_n_step_1(f0, n0, f1, n1, comp, proj,
						f0_0, n0_0, f0_1, n0_1,
						f1_0, n1_0, f1_1, n1_1);
				}
				merge_n_adaptive(f0_0, n0_0, f0_1, n0_1, buf, comp, proj);
				return merge_n_adaptive(f1_0, n1_0, f1_1, n1_1, buf, comp, proj);
			}

			template<class I, class Comp, class Proj>
			requires Sortable<I, Comp, Proj>
			static I sort_n_adaptive(I first, const iter_difference_t<I> n, buf_t<I>& buf,
				Comp& comp, Proj& proj)
			{
				STL2_EXPECT(0 <= n);
				auto half_n = n / 2;
				if (!half_n) {
					return next(first, n);
				}
				I middle = sort_n_adaptive(first, half_n, buf, comp, proj);
				sort_n_adaptive(middle, n - half_n, buf, comp, proj);
				return merge_n_adaptive(first, half_n, middle, n - half_n,
					buf, comp, proj);
			}
		};

		inline constexpr __fsort_n_fn fsort_n {};
	}
} STL2_CLOSE_NAMESPACE

#endif
