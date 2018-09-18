// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//

#ifndef STL2_DETAIL_RANDOM_HPP
#define STL2_DETAIL_RANDOM_HPP

#include <cstdint>
#include <initializer_list>
#include <random>
#include <stl2/random.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/algorithm/generate.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		namespace random {
			struct seeder // Brutal SeedSequence hack
			{
				using result_type = std::uint32_t;

				seeder() = default;
				seeder(std::initializer_list<result_type>) {}
				template<InputIterator I, Sentinel<I> S>
				seeder(I, S) {}

				template<RandomAccessIterator I, SizedSentinel<I> S>
				void generate(I first, S last) const {
					std::random_device rd{};
					__stl2::generate(first, last, std::ref(rd));
				}

				static constexpr std::size_t size() noexcept { return ~std::size_t{0}; }

				template<OutputIterator<result_type> I>
				void param(I) noexcept {}
			};
		}

		using default_random_engine =
			meta::if_c<sizeof(void*) >= 8, std::mt19937_64, std::mt19937>;
		template<class = void>
		inline default_random_engine& get_random_engine()
		{
			thread_local default_random_engine engine = []{
				random::seeder seed;
				return default_random_engine{seed};
			}();
			return engine;
		}
	}
} STL2_CLOSE_NAMESPACE

#endif
