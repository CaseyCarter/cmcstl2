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
#ifndef STL2_VIEW_FILTER_HPP
#define STL2_VIEW_FILTER_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <stl2/optional.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/algorithm/find_if.hpp>
#include <stl2/detail/semiregular_box.hpp>

STL2_OPEN_NAMESPACE {
    namespace ext {
        template <InputRange R, IndirectUnaryPredicate<iterator_t<R>> Pred>
        requires View<R>
        class filter_view : view_interface<filter_view<R, Pred>> {
        private:
            R base_;
            detail::semiregular_box<Pred> pred_;
            detail::non_propagating_cache<iterator_t<R>> begin_;
        public:
            filter_view() = default;
            constexpr filter_view(R base, Pred pred)
            : base_(std::move(base)), pred_(std::move(pred)) {}
            template <InputRange O>
            requires Constructible<R, all_view<O>>
            constexpr filter_view(O&& o, Pred pred)
            : base_(view::all(std::forward<O>(o))), pred_(std::move(pred)) {}
            R base() const {
                return base_;
            }
            class iterator;
            class sentinel;
            constexpr iterator begin() {
                if(!begin_)
                    begin_ = find_if(base_, std::ref(pred_.get()));
                return {*this, *begin_};
            }
            constexpr sentinel end() {
                return sentinel{*this};
            }
            constexpr iterator end() requires BoundedRange<R> {
                return {*this, __stl2::end(base_)};
            }
        };

        template <class R, class Pred>
        class filter_view<R, Pred>::iterator {
        private:
            iterator_t<R> current_ {};
            filter_view* parent_ = nullptr;
        public:
            using iterator_category =
                meta::if_c<models::BidirectionalIterator<iterator_t<R>>,
                    __stl2::bidirectional_iterator_tag,
                meta::if_c<models::ForwardIterator<iterator_t<R>>,
                    __stl2::forward_iterator_tag,
                    __stl2::input_iterator_tag>>;
            using value_type = value_type_t<iterator_t<R>>;
            using difference_type = difference_type_t<iterator_t<R>>;

            iterator() = default;
            constexpr iterator(filter_view& parent, iterator_t<R> current)
            : current_(current), parent_(&parent) {}

            constexpr iterator_t<R> base() const
            { return current_; }

            constexpr reference_t<iterator_t<R>> operator*() const
            { return *current_; }

            constexpr iterator& operator++() {
                current_ = find_if(++current_, __stl2::end(parent_->base_), std::ref(parent_->pred_.get()));
                return *this;
            }
            constexpr void operator++(int) {
                ++*this;
            }
            constexpr iterator operator++(int) requires ForwardRange<R> {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            constexpr iterator& operator--() requires BidirectionalRange<R> {
                do
                    --current_;
                while(invoke(parent_->pred_.get(), *current_));
                return *this;                
            }
            constexpr iterator operator--(int) requires BidirectionalRange<R> {
                auto tmp = *this;
                --*this;
                return tmp;
            }

            friend constexpr bool operator==(const iterator& x, const iterator& y)
            requires EqualityComparable<iterator_t<R>>
            { return x.current_ == y.current_; }
            friend constexpr bool operator!=(const iterator& x, const iterator& y)
            requires EqualityComparable<iterator_t<R>>
            { return !(x == y); }

            friend constexpr rvalue_reference_t<iterator_t<R>>
            iter_move(const iterator& i)
            noexcept(noexcept(__stl2::iter_move(i.current_)))
            { return __stl2::iter_move(i.current_); }
            friend constexpr void iter_swap(const iterator& x, const iterator& y)
            noexcept(noexcept(__stl2::iter_swap(x.current_, y.current_)))
            { __stl2::iter_swap(x.current_, y.current_); }
        };

        template <class R, class Pred>
        class filter_view<R, Pred>::sentinel {
        private:
            sentinel_t<R> end_;
        public:
            sentinel() = default;
            explicit constexpr sentinel(filter_view& parent)
            : end_(__stl2::end(parent)) {}

            constexpr sentinel_t<R> base() const
            { return end_; }

            friend constexpr bool operator==(const iterator& x, const sentinel& y)
            { return x.current_ == y.end_; }
            friend constexpr bool operator==(const sentinel& x, const iterator& y)
            { return y == x; }
            friend constexpr bool operator!=(const iterator& x, const sentinel& y)
            { return !(x == y); }
            friend constexpr bool operator!=(const sentinel& x, const iterator& y)
            { return !(y == x); }
        };

        template <class R, class Pred>
        filter_view(R&&, Pred) -> filter_view<all_view<R>, Pred>;
    } // namespace ext

    namespace view {
        namespace __filter {
            struct fn {
            private:
                template <class Pred>
                struct curry {
                    Pred pred_;

                    template <InputRange Rng>
                    requires IndirectUnaryPredicate<Pred, iterator_t<Rng>> &&
                      (std::is_lvalue_reference_v<Rng> || View<__f<Rng>>)
                    friend constexpr auto operator|(Rng&& rng, curry&& c) {
                        return fn{}(std::forward<Rng>(rng), std::move(c.pred_));
                    }
                };
            public:
                template <InputRange Rng, IndirectUnaryPredicate<iterator_t<Rng>> Pred>
                requires std::is_lvalue_reference_v<Rng> || View<__f<Rng>>
                constexpr auto operator()(Rng&& rng, Pred pred) const {
                    return ext::filter_view{std::forward<Rng>(rng), std::move(pred)};
                }

                template <class Pred>
                constexpr curry<Pred> operator()(Pred pred) const {
                    return {std::move(pred)};
                }
            };
        }

        inline constexpr __filter::fn filter {};
    }
} STL2_CLOSE_NAMESPACE

#endif
