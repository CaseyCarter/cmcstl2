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
#ifndef STL2_DETAIL_ALGORITHM_MOVE_HPP
#define STL2_DETAIL_ALGORITHM_MOVE_HPP

#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/algorithm/results.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// move [alg.move]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using move_result = __in_out_result<I, O>;

	struct __move_fn : private __niebloid {
		template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
		requires IndirectlyMovable<I, O>
		constexpr move_result<I, O>
		operator()(I first, S last, O result) const {
			for (; first != last; (void) ++first, (void) ++result) {
				*result = iter_move(first);
			}
			return {std::move(first), std::move(result)};
		}

		template<InputRange R, WeaklyIncrementable O>
		requires IndirectlyMovable<iterator_t<R>, O>
		constexpr move_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result) const {
			return (*this)(begin(r), end(r), std::move(result));
		}
	};

	inline constexpr __move_fn move {};

	namespace ext {
		struct __move_fn : private __niebloid {
			template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
			requires IndirectlyMovable<I, O>
			constexpr move_result<I, O>
			operator()(I first, S last, O result) const {
				for (; first != last; (void) ++first, (void) ++result) {
					*result = iter_move(first);
				}
				return {std::move(first), std::move(result)};
			}

			template<InputRange R, class O>
			requires !Range<O> && WeaklyIncrementable<__f<O>>
				&& IndirectlyMovable<iterator_t<R>, __f<O>>
			constexpr move_result<safe_iterator_t<R>, __f<O>>
			operator()(R&& r, O&& result_) const {
				auto result = std::forward<O>(result_);
				return (*this)(begin(r), end(r), std::move(result));
			}

			// Extension
			template<InputIterator I1, Sentinel<I1> S1,
				Iterator I2, Sentinel<I2> S2>
			requires IndirectlyMovable<I1, I2>
			constexpr move_result<I1, I2>
			operator()(I1 first1, S1 last1, I2 first2, S2 last2) const {
				while (true) {
					if (first1 == last1) break;
					if (first2 == last2) break;
					*first2 = iter_move(first1);
					++first1;
					++first2;
				}
				return {std::move(first1), std::move(first2)};
			}

			// Extension
			template<InputRange R1, Range R2>
			requires IndirectlyMovable<iterator_t<R1>, iterator_t<R2>>
			constexpr move_result<safe_iterator_t<R1>, safe_iterator_t<R2>>
			operator()(R1&& r1, R2&& r2) const {
				return (*this)(begin(r1), end(r1), begin(r2), end(r2));
			}
		};

		inline constexpr __move_fn move {};
	}
} STL2_CLOSE_NAMESPACE

#endif
