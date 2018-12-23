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
#ifndef STL2_DETAIL_ALGORITHM_COPY_HPP
#define STL2_DETAIL_ALGORITHM_COPY_HPP

#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>

///////////////////////////////////////////////////////////////////////////
// copy [alg.copy]
//
STL2_OPEN_NAMESPACE {
	template<class I, class O>
	using copy_result = __in_out_result<I, O>;

	struct __copy_fn : private __niebloid {
		template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
		requires IndirectlyCopyable<I, O>
		constexpr copy_result<I, O>
		operator()(I first, S last, O result) const {
			for (; first != last; (void) ++first, (void) ++result) {
				*result = *first;
			}
			return {std::move(first), std::move(result)};
		}

		template<InputRange R, WeaklyIncrementable O>
		requires IndirectlyCopyable<iterator_t<R>, O>
		constexpr copy_result<safe_iterator_t<R>, O>
		operator()(R&& r, O result) const {
			return (*this)(begin(r), end(r), std::move(result));
		}
	};

	inline constexpr __copy_fn copy {};

	namespace ext {
		struct __copy_fn : private __niebloid {
			template<InputIterator I, Sentinel<I> S, WeaklyIncrementable O>
			requires IndirectlyCopyable<I, O>
			constexpr copy_result<I, O>
			operator()(I first, S last, O result) const {
				for (; first != last; (void) ++first, (void) ++result) {
					*result = *first;
				}
				return {std::move(first), std::move(result)};
			}

			template<InputRange R, class O>
			requires !Range<O> && WeaklyIncrementable<__f<O>>
				&& IndirectlyCopyable<iterator_t<R>, __f<O>>
			constexpr copy_result<safe_iterator_t<R>, __f<O>>
			operator()(R&& r, O&& result) const {
				return (*this)(begin(r), end(r), std::forward<O>(result));
			}

			// Extension
			template<InputIterator I1, Sentinel<I1> S1, Iterator I2, Sentinel<I2> S2>
			requires IndirectlyCopyable<I1, I2>
			constexpr copy_result<I1, I2>
			operator()(I1 first, S1 last, I2 rfirst, S2 rlast) const {
				for (; first != last && rfirst != rlast; (void) ++first, (void)++rfirst) {
					*rfirst = *first;
				}
				return {std::move(first), std::move(rfirst)};
			}

			// Extension
			template<InputRange R1, Range R2>
			requires IndirectlyCopyable<iterator_t<R1>, iterator_t<R2>>
			constexpr copy_result<safe_iterator_t<R1>, safe_iterator_t<R2>>
			operator()(R1&& r1, R2&& r2) const {
				return (*this)(begin(r1), end(r1), begin(r2), end(r2));
			}
		};

		inline constexpr __copy_fn copy {};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
