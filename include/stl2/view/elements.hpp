#ifndef STL2_VIEW_TUPLE_ELEMENTS_HPP
#define STL2_VIEW_TUPLE_ELEMENTS_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

#include <tuple>

STL2_OPEN_NAMESPACE {
namespace ext {
    template<InputRange R, std::make_unsigned_t<std::size_t> N>
    requires View<R> && N < std::tuple_size_v<iter_value_t<iterator_t<R>>>
    class elements_view : public view_interface<elements_view<R, N>> {
    private:
        R base_;
        template<bool Const> struct __iterator;
        template<bool Const> struct __sentinel;
    public:
        elements_view() = default;

       constexpr elements_view(R base)
            : base_(std::move(base))
        {}

        template<Range O>
        requires _ConstructibleFromRange<R, O>
        constexpr elements_view(O&& o)
            : base_(view::all(std::forward<O>(o)))
        {}

        using iterator = __iterator<false>;
        using sentinel = __sentinel<false>;
        using const_iterator = __iterator<true>;
        using const_sentinel = __sentinel<true>;

        constexpr R base() const
        { return base_; }

        constexpr iterator begin()
        { return {*this, __stl2::begin(base_)}; }

        constexpr const_iterator begin() const
        { return {*this, __stl2::begin(base_)}; }

        constexpr sentinel end()
        { return sentinel{__stl2::end(base_)}; }

        constexpr const_sentinel end() const requires Range<R>
        { return const_sentinel{__stl2::end(base_)}; }

        constexpr iterator end() requires CommonRange<R>
        { return {*this, __stl2::end(base_)}; }

        constexpr const_iterator end() const requires CommonRange<R>
        { return {*this, __stl2::end(base_)}; }

        constexpr auto size() requires SizedRange<R>
        { return __stl2::size(base_); }

        constexpr auto size() const requires SizedRange<const R>
        { return __stl2::size(base_); }
    };

    template<class R, std::make_unsigned_t<std::size_t> N>
    template<bool Const>
    class elements_view<R, N>::__iterator {
    private:
        using Parent = __maybe_const<Const, elements_view>;
        using Base = __maybe_const<Const, R>;
        iterator_t<Base> current_ {};
        Parent* parent_ = nullptr;
        friend __iterator<!Const>;
        friend __sentinel<Const>;
    public:
        using iterator_category = iterator_category_t<iterator_t<Base>>;
        using value_type = __uncvref<std::tuple_element_t<N, iter_value_t<iterator_t<Base>>>>;
        using difference_type = iter_difference_t<iterator_t<Base>>;

        __iterator() = default;

        constexpr __iterator(Parent& parent, iterator_t<Base> current)
        : current_(current), parent_(&parent) {}

        constexpr __iterator(__iterator<!Const> i)
        requires Const && ConvertibleTo<iterator_t<R>, iterator_t<Base>>
        : current_(i.current_), parent_(i.parent_) {}

        constexpr iterator_t<Base> base() const
        { return current_; }
        constexpr decltype(auto) operator*() const
        { return std::get<N>(*current_);}

        constexpr __iterator& operator++()
        {
            ++current_;
            return *this;
        }
        constexpr void operator++(int)
        { ++current_; }
        constexpr __iterator operator++(int) requires ForwardRange<Base>
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr __iterator& operator--() requires BidirectionalRange<Base>
        {
            --current_;
            return *this;
        }
        constexpr __iterator operator--(int) requires BidirectionalRange<Base>
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr __iterator& operator+=(difference_type n)
        requires RandomAccessRange<Base>
        {
            current_ += n;
            return *this;
        }
        constexpr __iterator& operator-=(difference_type n)
        requires RandomAccessRange<Base>
        {
            current_ -= n;
            return *this;
        }
        constexpr decltype(auto) operator[](difference_type n) const
        requires RandomAccessRange<Base>
        { return std::get<N>(current_[n]); }

        friend constexpr bool operator==(const __iterator& x, const __iterator& y)
        requires EqualityComparable<iterator_t<Base>>
        { return x.current_ == y.current_; }

        friend constexpr bool operator!=(const __iterator& x, const __iterator& y)
        requires EqualityComparable<iterator_t<Base>>
        { return !(x == y); }

        friend constexpr bool operator<(const __iterator& x, const __iterator& y)
        requires RandomAccessRange<Base>
        { return x.current_ < y.current_; }

        friend constexpr bool operator>(const __iterator& x, const __iterator& y)
        requires RandomAccessRange<Base>
        { return y < x; }

        friend constexpr bool operator<=(const __iterator& x, const __iterator& y)
        requires RandomAccessRange<Base>
        { return !(y < x); }

        friend constexpr bool operator>=(const __iterator& x, const __iterator& y)
        requires RandomAccessRange<Base>
        { return !(x < y); }

        friend constexpr __iterator operator+(__iterator i, difference_type n)
        requires RandomAccessRange<Base>
        { return __iterator{*i.parent_, i.current_ + n}; }

        friend constexpr __iterator operator+(difference_type n, __iterator i)
        requires RandomAccessRange<Base>
        { return __iterator{*i.parent_, i.current_ + n}; }

        friend constexpr __iterator operator-(__iterator i, difference_type n)
        requires RandomAccessRange<Base>
        { return __iterator{*i.parent_, i.current_ - n}; }

        friend constexpr difference_type operator-(const __iterator& x, const __iterator& y)
        requires RandomAccessRange<Base>
        { return x.current_ - y.current_; }

        friend constexpr decltype(auto) iter_move(const __iterator& i) noexcept
        {
            if constexpr(std::is_lvalue_reference_v<decltype(*i)>)
                return std::move(*i);
            else
                return *i;
        }

        friend constexpr void iter_swap(const __iterator& x, const __iterator& y)
            noexcept(noexcept(__stl2::iter_swap(x.current_, y.current_)))
        { __stl2::iter_swap(x.current_, y.current_); }
    };

    template<class R, std::make_unsigned_t<std::size_t> N>
    template<bool Const>
    class elements_view<R, N>::__sentinel {
    private:
        using Parent = meta::if_c<Const, const elements_view, elements_view>;
        using Base = meta::if_c<Const, const R, R>;
        sentinel_t<Base> end_ {};
        friend __sentinel<!Const>;
    public:
        __sentinel() = default;
        explicit constexpr __sentinel(sentinel_t<Base> end)
        : end_(end) {}
        constexpr __sentinel(__sentinel<!Const> i)
        requires Const && ConvertibleTo<sentinel_t<R>, sentinel_t<const R>>
        : end_(i.end_) {}

        constexpr sentinel_t<Base> base() const
        { return end_; }

        friend constexpr bool operator==(const __iterator<Const>& x, const __sentinel& y)
        { return x.current_ == y.end_; }

        friend constexpr bool operator==(const __sentinel& x, const __iterator<Const>& y)
        { return y == x; }

        friend constexpr bool operator!=(const __iterator<Const>& x, const __sentinel& y)
        { return !(x == y); }

        friend constexpr bool operator!=(const __sentinel& x, const __iterator<Const>& y)
        { return !(y == x); }

        friend constexpr iter_difference_t<iterator_t<Base>>
        operator-(const __iterator<Const>& x, const __sentinel& y)
        requires SizedSentinel<sentinel_t<Base>, iterator_t<Base>>
        { return x.current_ - y.end_; }

        friend constexpr iter_difference_t<iterator_t<Base>>
        operator-(const __sentinel& y, const __iterator<Const>& x)
        requires SizedSentinel<sentinel_t<Base>, iterator_t<Base>>
        { return x.end_ - y.current_; }
    };

    namespace view {
        template <std::make_unsigned_t<std::size_t> N>
        struct __elements_fn : detail::__pipeable<__elements_fn<N>> {
            template<InputRange Rng>
            requires N < std::tuple_size_v<iter_value_t<iterator_t<Rng>>>
            constexpr auto operator()(Rng&& rng) const {
                return elements_view<decltype(ranges::view::all(std::forward<Rng>(rng))), N>{rng};
            }
        };

        struct __keys_fn : detail::__pipeable<__keys_fn> {
            template<InputRange Rng>
            requires std::tuple_size_v<iter_value_t<iterator_t<Rng>>> == 2
            constexpr auto operator()(Rng&& rng) const {
                return elements_view<decltype(ranges::view::all(std::forward<Rng>(rng))), 0>{rng};
            }
        };

        struct __values_fn : detail::__pipeable<__values_fn> {
            template<InputRange Rng>
            requires std::tuple_size_v<iter_value_t<iterator_t<Rng>>> == 2
            constexpr auto operator()(Rng&& rng) const {
                return elements_view<decltype(ranges::view::all(std::forward<Rng>(rng))), 1>{rng};
            }
        };

        inline constexpr __keys_fn keys {};
        inline constexpr __values_fn values {};

        template <std::make_unsigned_t<std::size_t> N>
        inline constexpr __elements_fn<N> elements {};
    } // namespace view
  } // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
