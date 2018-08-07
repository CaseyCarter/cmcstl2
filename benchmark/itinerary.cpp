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
#include <experimental/ranges/range>
#include <experimental/ranges/algorithm>

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

namespace ranges = std::experimental::ranges;

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
    { return std::lexicographical_compare(ds.begin(), ds.end(),
                                          i.legs().begin(), i.legs().end(),
                                          LegDepartureComparator()); }

    bool operator()(Itinerary const& i, std::vector<Date> const& ds) const
    { return std::lexicographical_compare(i.legs().begin(), i.legs().end(),
                                          ds.begin(), ds.end(),
                                          LegDepartureComparator()); }

    bool operator()(Itinerary const& i1, Itinerary const& i2) const
    { return std::lexicographical_compare(i1.legs().begin(), i1.legs().end(),
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
            std::equal_range(itineraries.begin(), itineraries.end(),
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
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(
            ranges::equal_range(
                itineraries,
                dates,
                ranges::lexicographical_compare,
                toDates));
    }
}
BENCHMARK_REGISTER_F(ItineraryFixture, STL2)
    -> Ranges({{1<<10, 1<<20}, {1,2}});

BENCHMARK_MAIN();
