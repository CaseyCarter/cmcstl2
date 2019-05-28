// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler
//  Copyright Casey Carter
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <stl2/detail/algorithm/sort.hpp>
#include <stl2/detail/to.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/take.hpp>
#include <stl2/view/transform.hpp>
#include "simple_test.hpp"
#include "test_utils.hpp"

namespace ranges = __stl2;

template<class T>
struct my_alloc {
	using value_type = T;

	constexpr my_alloc(int x = 0) noexcept : x{x} {}
	template<class U>
	constexpr my_alloc(my_alloc<U> const& that) noexcept : x{that.x} {}

	T* allocate(std::size_t const n) const {
		return std::allocator<T>{}.allocate(n);
	}
	void deallocate(T* const ptr, std::size_t const n) const noexcept {
		std::allocator<T>{}.deallocate(ptr, n);
	}

	template<class U>
	constexpr bool operator==(my_alloc<U> const& that) const noexcept {
		return x == that.x;
	}
	template<class U>
	constexpr bool operator!=(my_alloc<U> const& that) const noexcept {
		return x != that.x;
	}

	int x;
};

template<class T>
struct vector_like : std::vector<T> {
	using std::vector<T>::vector;

	using typename std::vector<T>::size_type;

	size_type last_reservation{};
	size_type reservation_count{};

	void reserve(size_type n)
	{
		std::vector<T>::reserve(n);
		last_reservation = n;
		++reservation_count;
	}
};

#if 0 // zip unsupported
template<
	typename Rng,
	typename CI = ranges::range_common_iterator_t<Rng>,
	typename = decltype(std::map{CI{}, CI{}})>
void test_zip_to_map(Rng &&rng, int) {
	using namespace ranges;
	auto m = static_cast<Rng &&>(rng) | to<std::map>;
	static_assert(Same<decltype(m), std::map<int, int>>);
}
template<typename Rng>
void test_zip_to_map(Rng &&, long)
{}
#endif

constexpr auto to_string = ranges::view::transform([](auto x) { return std::to_string(x); });

int main() {
	using namespace ranges;

	{
		constexpr auto rng = view::iota(0) |
			view::transform([](int i) { return i * i; }) |
			view::take(10);
		constexpr auto expected = {0,1,4,9,16,25,36,49,64,81};
		{
			// Container specified as type [trivial]
			auto lst0 = ranges::to<std::list<int>>(rng);
			static_assert(Same<decltype(lst0), std::list<int>>);
			CHECK_EQUAL(lst0, expected);
			auto lst1 = rng | ranges::to<std::list<int>>;
			static_assert(Same<decltype(lst1), std::list<int>>);
			CHECK_EQUAL(lst1, expected);
			auto lst2 = rng | ranges::to<std::list<int>>();
			static_assert(Same<decltype(lst2), std::list<int>>);
			CHECK_EQUAL(lst2, expected);

			auto lst3 = ranges::to<std::list<int, my_alloc<int>>>(rng, my_alloc<int>(42));
			static_assert(Same<decltype(lst3), std::list<int, my_alloc<int>>>);
			CHECK_EQUAL(lst3, expected);
			CHECK(lst3.get_allocator().x == 42);
			auto lst4 = rng | ranges::to<std::list<int, my_alloc<int>>>(my_alloc<int>(42));
			static_assert(Same<decltype(lst4), std::list<int, my_alloc<int>>>);
			CHECK_EQUAL(lst4, expected);
			CHECK(lst4.get_allocator().x == 42);
		}
		{
			// Container specified as type [nontrivial]
			auto lst0 = ranges::to<std::list<std::string>>(rng | to_string);
			static_assert(Same<decltype(lst0), std::list<std::string>>);
			CHECK_EQUAL(lst0, expected | to_string);
			auto lst1 = rng | to_string | ranges::to<std::list<std::string>>;
			static_assert(Same<decltype(lst1), std::list<std::string>>);
			CHECK_EQUAL(lst1, expected | to_string);
			auto lst2 = rng | to_string | ranges::to<std::list<std::string>>();
			static_assert(Same<decltype(lst2), std::list<std::string>>);
			CHECK_EQUAL(lst2, expected | to_string);

			auto lst3 = ranges::to<std::list<std::string, my_alloc<std::string>>>(
				rng | to_string, my_alloc<std::string>(42));
			static_assert(Same<decltype(lst3), std::list<std::string, my_alloc<std::string>>>);
			CHECK_EQUAL(lst3, expected | to_string);
			CHECK(lst3.get_allocator().x == 42);
			auto lst4 = rng | to_string |
				ranges::to<std::list<std::string, my_alloc<std::string>>>(my_alloc<int>(42));
			static_assert(Same<decltype(lst4), std::list<std::string, my_alloc<std::string>>>);
			CHECK_EQUAL(lst4, expected | to_string);
			CHECK(lst4.get_allocator().x == 42);
		}

		{
			// Container specified as template [trivial]
			auto lst0 = ranges::to<std::list>(rng);
			static_assert(Same<decltype(lst0), std::list<int>>);
			CHECK_EQUAL(lst0, expected);
			auto lst1 = rng | ranges::to<std::list>;
			static_assert(Same<decltype(lst1), std::list<int>>);
			CHECK_EQUAL(lst1, expected);
			auto lst2 = rng | ranges::to<std::list>();
			static_assert(Same<decltype(lst2), std::list<int>>);
			CHECK_EQUAL(lst2, expected);

			auto lst3 = ranges::to<std::list>(rng, my_alloc<int>(42));
			static_assert(Same<decltype(lst3), std::list<int, my_alloc<int>>>);
			CHECK_EQUAL(lst3, expected);
			CHECK(lst3.get_allocator().x == 42);
			auto lst4 = rng | ranges::to<std::list>(my_alloc<int>(42));
			static_assert(Same<decltype(lst4), std::list<int, my_alloc<int>>>);
			CHECK_EQUAL(lst4, expected);
			CHECK(lst4.get_allocator().x == 42);
		}
		{
			// Container specified as template [nontrivial]
			auto lst0 = ranges::to<std::list>(rng | to_string);
			static_assert(Same<decltype(lst0), std::list<std::string>>);
			CHECK_EQUAL(lst0, expected | to_string);
			auto lst1 = rng | to_string | ranges::to<std::list>;
			static_assert(Same<decltype(lst1), std::list<std::string>>);
			CHECK_EQUAL(lst1, expected | to_string);
			auto lst2 = rng | to_string | ranges::to<std::list>();
			static_assert(Same<decltype(lst2), std::list<std::string>>);
			CHECK_EQUAL(lst2, expected | to_string);

			auto lst3 = ranges::to<std::list>(rng | to_string, my_alloc<std::string>(42));
			static_assert(Same<decltype(lst3), std::list<std::string, my_alloc<std::string>>>);
			CHECK_EQUAL(lst3, expected | to_string);
			CHECK(lst3.get_allocator().x == 42);
			auto lst4 = rng | to_string | ranges::to<std::list>(my_alloc<std::string>(42));
			static_assert(Same<decltype(lst4), std::list<std::string, my_alloc<std::string>>>);
			CHECK_EQUAL(lst4, expected | to_string);
			CHECK(lst4.get_allocator().x == 42);
		}
	}

	{
		auto lst = view::iota(0) | view::transform([](int i){return i*i;}) | view::take(10)
			| to<std::list>;
		static_assert(Same<decltype(lst), std::list<int>>);
		CHECK_EQUAL(lst, {0,1,4,9,16,25,36,49,64,81});
	}

#if 0 // to_vector not supported
	{
		auto vec = view::iota(0) | view::transform([](int i){return i*i;}) | view::take(10)
			| to_vector;
		ranges::sort(vec, std::greater{});
		static_assert(Same<decltype(vec), std::vector<int>>);
		CHECK_EQUAL(vec, {81,64,49,36,25,16,9,4,1,0});
	}
#endif

	{
		auto vec = view::iota(0) | view::transform([](int i){return i*i;}) | view::take(10)
			| to<std::vector<long>>();
		ranges::sort(vec, std::greater{});
		static_assert(Same<decltype(vec), std::vector<long>>);
		CHECK_EQUAL(vec, {81,64,49,36,25,16,9,4,1,0});
	}

	{
		auto vec = view::iota(0) | view::transform([](int i){return i*i;}) | view::take(10)
			| to<std::vector<long>>;
		ranges::sort(vec, std::greater{});
		static_assert(Same<decltype(vec), std::vector<long>>);
		CHECK_EQUAL(vec, {81,64,49,36,25,16,9,4,1,0});
	}

	{
		const std::size_t N = 4096;
		auto vl = view::iota(0, int{N}) | to<vector_like<int>>();
		static_assert(Same<decltype(vl), vector_like<int>>);
		CHECK(vl.reservation_count == std::size_t{1});
		CHECK(vl.last_reservation == N);
	}

#if 0 // zip unsupported
	// https://github.com/ericniebler/range-v3/issues/1145
	{
		auto r1 = view::indices( std::uintmax_t{ 100 } );
		auto r2 = view::zip( r1, r1 );

		std::map<std::uintmax_t, std::uintmax_t> m = r2;
		(void) m;
		(void)(r2 | ranges::to<std::map<std::uintmax_t, std::uintmax_t>>);
	}

	test_zip_to_map(view::zip(view::iota(0), view::iota(0, 10)), 0);
#endif

	{
		// Examples from P1206
		std::list<int> l;
		std::map<int, int> m;
#if 1 // Added for completeness
		using Alloc = my_alloc<int>;
		my_alloc<int> alloc(42);
#endif
		// copy a list to a vector of the same type
#if 0 // P1141 syntax not supported
		Same<std::vector<int>> auto a = ranges::to<std::vector<int>>(l);
#else
		auto a = ranges::to<std::vector<int>>(l);
		static_assert(Same<decltype(a), std::vector<int>>);
#endif
		// Specify an allocator
#if 0 // P1141 syntax not supported
		Same<std::vector<int, Alloc>> auto b = ranges::to<std::vector<int, Alloc>(l, alloc);
#else
		auto b = ranges::to<std::vector<int, Alloc>>(l, alloc);
		static_assert(Same<decltype(b), std::vector<int, Alloc>>);
#endif
		// copy a list to a vector of the same type, deducing value_type
#if 0 // P1141 syntax not supported
		Same<std::vector<int>> auto c = ranges::to<std::vector>(l);
#else
		auto c = ranges::to<std::vector>(l);
		static_assert(Same<decltype(c), std::vector<int>>);
#endif
		// copy to a container of types ConvertibleTo
#if 0 // P1141 syntax not supported
		Same<std::vector<long>> auto d = ranges::to<std::vector<long>>(l);
#else
		auto d = ranges::to<std::vector<long>>(l);
		static_assert(Same<decltype(d), std::vector<long>>);
#endif
		// Supports converting associative container to sequence containers
#if 0 // P1141 syntax not supported
		Same<std::vector<std::pair<const int, int>>>
			auto f = ranges::to<vector<std::pair<const int, int>>>(m);
#else
		auto f = ranges::to<std::vector<std::pair<const int, int>>>(m);
		static_assert(Same<decltype(f), std::vector<std::pair<const int, int>>>);
#endif
		// Removing the const from the key by default
#if 0 // P1141 syntax not supported
		Same<std::vector<std::pair<int, int>>> auto e = ranges::to<std::vector>(m);
#else
		auto e = ranges::to<std::vector>(m);
#if 0 // Per the Kona minutes: "[stripping of const from first element in pair should be removed]"
		static_assert(Same<decltype(e), std::vector<std::pair<int, int>>>);
#else
		static_assert(Same<decltype(e), std::vector<std::pair<const int, int>>>);
#endif
#endif
		// Supports converting sequence containers to associative ones
#if 0 // P1141 syntax not supported
		Same<std::map<int, int>> auto g = f | ranges::to<map>();
#else
		auto g1 = f | ranges::to<std::map>();
		static_assert(Same<decltype(g1), std::map<int, int>>);
#endif
		// Pipe syntax
#if 0 // P1141 syntax not supported
		Same<std::vector<int>> auto g = l | ranges::view::take(42) | ranges::to<std::vector>();
#else
		auto g2 = l | ranges::view::take(42) | ranges::to<std::vector>();
		static_assert(Same<decltype(g2), std::vector<int>>);
#endif
		// Pipe syntax with allocator
		auto h = l | ranges::view::take(42) | ranges::to<std::vector>(alloc);
#if 1 // Additional validation
		static_assert(Same<decltype(h), std::vector<int, Alloc>>);
		CHECK(h.get_allocator().x == 42);
#endif
		// The pipe syntax also support specifying the type and conversions
		auto i = l | ranges::view::take(42) | ranges::to<std::vector<long>>();
		// Parentheses are optional for template...
#if 0 // P1141 syntax not supported
		Same<std::vector<int>> auto j = l | ranges::view::take(42) | ranges::to<std::vector>;
#else
		auto j = l | ranges::view::take(42) | ranges::to<std::vector>;
		static_assert(Same<decltype(j), std::vector<int>>);
#endif
		// ...and types
		auto k = l | ranges::view::take(42) | ranges::to<std::vector<long>>;
	}

	return ::test_result();
}
