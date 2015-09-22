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
#ifndef STL2_RANDOM_HPP
#define STL2_RANDOM_HPP

#include <random>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/urng.hpp>

STL2_OPEN_NAMESPACE {
  using std::linear_congruential_engine;
  using std::mersenne_twister_engine;
  using std::subtract_with_carry_engine;
  using std::discard_block_engine;
  using std::independent_bits_engine;
  using std::shuffle_order_engine;

  using std::minstd_rand0;
  using std::minstd_rand;
  using std::mt19937;
  using std::mt19937_64;
  using std::ranlux24_base;
  using std::ranlux48_base;
  using std::ranlux24;
  using std::ranlux48;
  using std::knuth_b;
  using std::default_random_engine;

  using std::random_device;
  using std::seed_seq;

  using std::generate_canonical;
  using std::uniform_int_distribution;
  using std::uniform_real_distribution;
  using std::bernoulli_distribution;
  using std::binomial_distribution;
  using std::geometric_distribution;
  using std::negative_binomial_distribution;
  using std::poisson_distribution;
  using std::exponential_distribution;
  using std::gamma_distribution;
  using std::weibull_distribution;
  using std::extreme_value_distribution;
  using std::normal_distribution;
  using std::lognormal_distribution;
  using std::chi_squared_distribution;
  using std::cauchy_distribution;
  using std::fisher_f_distribution;
  using std::student_t_distribution;
  using std::discrete_distribution;
  using std::piecewise_constant_distribution;
  using std::piecewise_linear_distribution;
} STL2_CLOSE_NAMESPACE

#endif
