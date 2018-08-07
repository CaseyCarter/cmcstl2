// cmcstl2 - A concept-enabled C++ standard library
//
// Copyright Tomasz Kamiński 2016
// Copyright Eric Niebler 2018
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2

#include <vector>
#include <algorithm>
#include <random>
#include <array>
#include <benchmark/benchmark.h>
#include <experimental/ranges/ranges>
#include <experimental/ranges/algorithm>

namespace ranges = std::experimental::ranges;

namespace not_ranges
{
    template <class C, class T>
    struct __lower_bound_fn {
        std::decay_t<C> comp_;
        const T& value_;

        constexpr __lower_bound_fn(C comp, const T& value) :
            comp_(std::ref(comp)), value_(value) {}

        constexpr bool operator()(auto&& i) const {
            return ranges::invoke(comp_, i, value_);
        }
    };

    template <class I, class T, class Comp = ranges::less<>, class Proj = ranges::identity>
    std::decay_t<I> lower_bound_n(I&& first, ranges::iter_difference_t<std::decay_t<I>> n,
        const T& value, Comp comp = Comp{}, Proj proj = Proj{})
    {
        return ranges::ext::partition_point_n(
            std::forward<I>(first), n,
            __lower_bound_fn<Comp, T>{std::ref(comp), value},
            std::ref(proj));
    }

    template <class C, class T>
    struct __upper_bound_fn {
        std::decay_t<C> comp_;
        const T& value_;

        constexpr __upper_bound_fn(C comp, const T& value)
        : comp_(std::ref(comp)), value_(value) {}

        constexpr bool operator()(auto&& i) const {
            return !ranges::invoke(comp_, value_, i);
        }
    };

    template <class I, class T, class Comp = ranges::less<>, class Proj = ranges::identity>
    std::decay_t<I> upper_bound_n(I&& first, ranges::iter_difference_t<std::decay_t<I>> n, const T& value,
        Comp comp = Comp{}, Proj proj = Proj{})
    {
        return ranges::ext::partition_point_n(std::forward<I>(first), n,
            __upper_bound_fn<Comp, T>{std::ref(comp), value},
            std::ref(proj));
    }

    template <class I, class T,
        class Comp = ranges::less<>, class Proj = ranges::identity>
    ranges::ext::subrange<I>
    equal_range_n(I first, ranges::iter_difference_t<I> dist, const T& value,
        Comp comp = Comp{}, Proj proj = Proj{})
    {
        if (0 < dist) {
            do {
                auto half = dist / 2;
                auto middle = ranges::next(first, half);
                auto&& v = *middle;
                auto&& pv = ranges::invoke(proj, std::forward<decltype(v)>(v));
                if (ranges::invoke(comp, pv, value)) {
                    first = std::move(middle);
                    ++first;
                    dist -= half + 1;
                } else if (ranges::invoke(comp, value, pv)) {
                    dist = half;
                } else {
                    return {
                        not_ranges::lower_bound_n(
                            std::move(first), half, value,
                            std::ref(comp), std::ref(proj)),
                        not_ranges::upper_bound_n(ranges::next(middle),
                            dist - (half + 1), value,
                            std::ref(comp), std::ref(proj))
                    };
                }
            } while (0 != dist);
        }
        return {first, first};
    }

    template <class I, class S, class T,
        class Comp = ranges::less<>, class Proj = ranges::identity>
    ranges::ext::subrange<I> equal_range(I first, S last, const T& value,
        Comp comp = Comp{}, Proj proj = Proj{})
    {
        // Probe exponentially for either end-of-range, an iterator that
        // is past the equal range (i.e., denotes an element greater
        // than value), or is in the equal range (denotes an element equal
        // to value).
        auto dist = ranges::iter_difference_t<I>{1};
        while (true) {
            auto mid = first;
            auto d = ranges::advance(mid, dist, last);
            STL2_EXPECT(d >= 0);
            if (d || mid == last) {
                // at the end of the input range
                return not_ranges::equal_range_n(
                    std::move(first), dist - d, value,
                    std::ref(comp), std::ref(proj));
            }
            auto&& v = *mid;
            auto&& pv = ranges::invoke(proj, std::forward<decltype(v)>(v));
            // if value < *mid, mid is after the target range.
            if (ranges::invoke(comp, value, pv)) {
                return not_ranges::equal_range_n(
                    std::move(first), dist, value,
                    std::ref(comp), std::ref(proj));
            } else if (!ranges::invoke(comp, pv, value)) {
                // *mid == value: the lower bound is <= mid, and the upper bound is > mid.
                return {
                    not_ranges::lower_bound_n(std::move(first), dist, value,
                        std::ref(comp), std::ref(proj)),
                    ranges::upper_bound(std::move(mid), std::move(last),
                        value, std::ref(comp), std::ref(proj))
                };
            }
            // *mid < value, mid is before the target range.
            first = std::move(mid);
            ++first;
            dist *= 2;
        }
    }

    template <class I, ranges::SizedSentinel<I> S, class T,
        class Comp = ranges::less<>, class Proj = ranges::identity>
    ranges::ext::subrange<I> equal_range(I first, S last, const T& value,
        Comp comp = Comp{}, Proj proj = Proj{})
    {
        auto len = ranges::distance(first, std::move(last));
        return not_ranges::equal_range_n(std::move(first), len, value,
            std::ref(comp), std::ref(proj));
    }

    template <class I1, class S1, class I2, class S2, class Comp = ranges::less<>,
        class Proj1 = ranges::identity, class Proj2 = ranges::identity>
    constexpr bool lexicographical_compare(I1 first1, S1 last1, I2 first2, S2 last2,
        Comp comp = Comp{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
    {
        for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
            if (ranges::invoke(comp, ranges::invoke(proj1, *first1), ranges::invoke(proj2, *first2))) {
                return true;
            }
            if (ranges::invoke(comp, ranges::invoke(proj2, *first2), ranges::invoke(proj1, *first1))) {
                return false;
            }
        }
        return first1 == last1 && first2 != last2;
    }
}

namespace std::experimental::ranges
{
    template<class Ref>
    struct any_view
    {
        template<Range R>
            requires Same<iter_reference_t<iterator_t<R>>, Ref>
        any_view(R&&)
        {}
        std::add_pointer_t<Ref> begin() const { return 0; }
        std::add_pointer_t<Ref> end() const { return 0; }
    };
    template <Range R1, Range R2>
        requires CommonReference<iter_reference_t<iterator_t<R1>>,
                                 iter_reference_t<iterator_t<R2>>>
    struct common_type<R1, R2>
    {
        using type =
            any_view<common_reference_t<iter_reference_t<iterator_t<R1>>,
                                        iter_reference_t<iterator_t<R2>>>>;
    };
}

namespace sort_of_ranges {
    template <ranges::ForwardRange Rng, class T,
        class Comp = ranges::less<>, class Proj = ranges::identity>
    requires
        ranges::IndirectStrictWeakOrder<
            Comp, const T*, ranges::projected<ranges::iterator_t<Rng>, Proj>>
    ranges::ext::safe_subrange_t<Rng>
    equal_range(Rng&& rng, const T& value, Comp comp = Comp{}, Proj proj = Proj{})
    {
        return not_ranges::equal_range(
            ranges::begin(rng), ranges::end(rng), value,
            std::ref(comp), std::ref(proj));
    }

    template <ranges::ForwardRange Rng, class T, class Comp = ranges::less<>,
        class Proj = ranges::identity>
    requires
        ranges::IndirectStrictWeakOrder<
            Comp, const T*, ranges::projected<ranges::iterator_t<Rng>, Proj>> &&
        ranges::SizedRange<Rng>
    ranges::ext::safe_subrange_t<Rng>
    equal_range(Rng&& rng, const T& value, Comp comp = Comp{}, Proj proj = Proj{})
    {
        return not_ranges::equal_range_n(
            ranges::begin(rng), ranges::size(rng), value,
            std::ref(comp), std::ref(proj));
    }

    template <ranges::InputRange Rng1, ranges::InputRange Rng2, class Comp = ranges::less<>,
        class Proj1 = ranges::identity, class Proj2 = ranges::identity>
    requires
        ranges::IndirectStrictWeakOrder<Comp,
            ranges::projected<ranges::iterator_t<Rng1>, Proj1>,
            ranges::projected<ranges::iterator_t<Rng2>, Proj2>>
    constexpr bool lexicographical_compare(Rng1&& rng1, Rng2&& rng2,
        Comp comp = Comp{}, Proj1 proj1 = Proj1{}, Proj2 proj2 = Proj2{})
    {
        return not_ranges::lexicographical_compare(
            ranges::begin(rng1), ranges::end(rng1),
            ranges::begin(rng2), ranges::end(rng2),
            std::ref(comp),
            std::ref(proj1),
            std::ref(proj2));
    }
}

struct Date
{
    int v;
};

bool operator==(Date d1, Date d2) { return d1.v == d2.v; }
bool operator!=(Date t1, Date t2) { return !(t1 == t2); }

bool operator<(Date d1, Date d2) { return d1.v < d2.v; }
bool operator>(Date t1, Date t2) { return t1 < t2; }
bool operator>=(Date t1, Date t2) { return !(t1 < t2); }
bool operator<=(Date t1, Date t2) { return !(t2 < t1); }

struct Leg
{
    Date d;
    Date departure_date() const { return d; }
};

struct Itinerary
{
    std::vector<Leg> l;
    std::vector<Leg> const& legs() const { return l; }
};

struct LegDepartureComparator
{
    bool operator()(Date d, Leg const& l) const
    { return d < l.departure_date(); }

    bool operator()(Leg const& l, Date d) const
    { return  l.departure_date() < d; }

    bool operator()(Leg const& l1, Leg const& l2) const
    { return l1.departure_date() < l2.departure_date(); }
};

struct ItineraryDepartureComparator
{
    bool operator()(std::vector<Date> const& ds, Itinerary const& i) const
    { return not_ranges::lexicographical_compare(ds.begin(), ds.end(),
                                                 i.legs().begin(), i.legs().end(),
                                                 LegDepartureComparator()); }

    bool operator()(Itinerary const& i, std::vector<Date> const& ds) const
    { return not_ranges::lexicographical_compare(i.legs().begin(), i.legs().end(),
                                                 ds.begin(), ds.end(),
                                                 LegDepartureComparator()); }

    bool operator()(Itinerary const& i1, Itinerary const& i2) const
    { return not_ranges::lexicographical_compare(i1.legs().begin(), i1.legs().end(),
                                                 i2.legs().begin(), i2.legs().end(),
                                                 LegDepartureComparator()); }
};

class ItineraryFixture : public ::benchmark::Fixture {
public:
    void SetUp(const ::benchmark::State& st)
    {
        std::size_t const elements = st.range(0);
        std::size_t const legs = st.range(1);

        std::mt19937 gen;
        std::uniform_int_distribution<int> dist(0, 30);

        dates.resize(legs);
        for (Date& d : dates)
            d.v = dist(gen);

        itineraries.resize(elements);
        for (Itinerary& itin : itineraries)
        {
            itin.l.resize(legs);
            for (Leg& leg : itin.l)
                leg.d.v = dist(gen);
        }
        std::sort(itineraries.begin(), itineraries.end(),
                  ItineraryDepartureComparator());
    }

    void TearDown(const ::benchmark::State&)
    {
        itineraries.clear();
        dates.clear();
    }

    std::vector<Itinerary> itineraries;
    std::vector<Date> dates;
};

BENCHMARK_DEFINE_F(ItineraryFixture, STL1)(benchmark::State& state)
{
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            not_ranges::equal_range(itineraries.begin(), itineraries.end(),
                                    dates, ItineraryDepartureComparator()));
    }
}
BENCHMARK_REGISTER_F(ItineraryFixture, STL1)
    -> Ranges({{1<<10, 1<<20}, {1,2}});

inline constexpr auto toDate = [](const Leg& l)
{
    return l.departure_date();
};
inline constexpr auto toDates = [](Itinerary& i)
{
    return i.legs() | ranges::view::transform(toDate);
};

BENCHMARK_DEFINE_F(ItineraryFixture, STL2)(benchmark::State& state)
{
    auto const cmp = [](auto const& x, auto const& y)
    {
        return sort_of_ranges::lexicographical_compare(x, y);
    };

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            sort_of_ranges::equal_range(itineraries, dates, cmp, toDates));
    }
}
BENCHMARK_REGISTER_F(ItineraryFixture, STL2)
    -> Ranges({{1<<10, 1<<20}, {1,2}});

BENCHMARK_MAIN();
