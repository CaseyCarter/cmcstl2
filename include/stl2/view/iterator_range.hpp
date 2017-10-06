// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_ITERATOR_RANGE_HPP
#define STL2_VIEW_ITERATOR_RANGE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/detail/algorithm/tagspec.hpp>
#include <stl2/utility.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
        template <Iterator I, Sentinel<I> S = I>
        class iterator_range
            : tagged_pair<tag::begin(I), tag::end(S)>,
              view_interface<iterator_range<I, S>> {
        public:
            using iterator = I;
            using sentinel = S;
            iterator_range() = default;
            constexpr iterator_range(I i, S s)
                : tagged_pair<tag::begin(I), tag::end(S)>{i, s} {}
            template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
            constexpr iterator_range(iterator_range<X, Y> r)
                : tagged_pair<tag::begin(I), tag::end(S)>{r.begin(), r.end()} {}
            template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
            constexpr iterator_range(pair<X, Y> r)
                : tagged_pair<tag::begin(I), tag::end(S)>{r.first, r.second} {}
            template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
            constexpr iterator_range& operator=(iterator_range<X, Y> r) {
                this->first = r.begin();
                this->second = r.end();
                return *this;
            }
            template <Constructible<const I&> X, Constructible<const S&> Y>
            constexpr operator pair<X, Y>() const {
                return {this->first, this->second};
            }
            constexpr bool empty() const {
                return this->first == this->second;
            }
        };

        template <Iterator I, Sentinel<I> S = I>
        class sized_iterator_range
            : view_interface<sized_iterator_range<I, S>> {
        private:
            iterator_range<I, S> rng_; // exposition only
            difference_type_t<I> size_; // exposition only
        public:
            using iterator = I;
            using sentinel = S;
            sized_iterator_range() = default;
            constexpr sized_iterator_range(I i, S s, difference_type_t<I> n)
                : rng_{i, s}, size_{n} {}
            template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
            constexpr sized_iterator_range(pair<X, Y> r, difference_type_t<I> n)
                : rng_{r.first, r.second}, size_{n} {}
            template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
            constexpr sized_iterator_range(iterator_range<X, Y> r, difference_type_t<I> n)
                : rng_{r.begin(), r.end()}, size_{n} {}
            template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
            constexpr sized_iterator_range(sized_iterator_range<X, Y> r)
                : rng_{r.begin(), r.end()}, size_{r.size()} {}
            
            template <ConvertibleTo<I> X, ConvertibleTo<S> Y>
            constexpr sized_iterator_range& operator=(sized_iterator_range<X, Y> r) {
                rng_.first = r.begin();
                rng_.second = r.end();
                size_ = r.size();
                return *this;
            }
            template <Constructible<const I&> X, Constructible<const S&> Y>
            constexpr operator pair<X, Y>() const {
                return {rng_.first, rng_.second};
            }
            template <Constructible<const I&> X, Constructible<const S&> Y>
            constexpr operator iterator_range<X, Y>() const {
                return {rng_.first, rng_.second};
            }
            constexpr operator iterator_range<I, S> const &() const & noexcept {
                return rng_;
            }
            constexpr I begin() const {
                return rng_.first;
            }
            constexpr S end() const {
                return rng_.second;
            }
            constexpr difference_type_t<I> size() const noexcept {
                return size_;
            }
        };
    }
} STL2_CLOSE_NAMESPACE

#endif
