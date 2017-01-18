// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#include <numeric>
#include <stl2/iterator.hpp>
#include <stl2/functional.hpp>
#include <stl2/detail/algorithm/copy.hpp>
#include <stl2/detail/algorithm/equal.hpp>
#include <stl2/view/iota.hpp>
#include <stl2/view/take_exactly.hpp>
#include <stl2/view/repeat_n.hpp>
#include <vector>
#include <memory>
#include "../simple_test.hpp"

namespace ranges = __stl2;

struct A {
	static std::size_t copy_count;
	static std::size_t move_count;
	int i{0};

	static void clear() { copy_count = move_count = 0; }

	A() = default;
	A(int i) : i(i) {}
	A(const A& that) : i(that.i) { ++copy_count; }
	A(A&& that) noexcept : i(that.i) { ++move_count; that.i = -1; }
	A& operator=(const A& that) & { ++copy_count; i = that.i; return *this; }
	A& operator=(A&& that) & noexcept {
		++move_count;
		i = that.i;
		that.i = -1;
		return *this;
	}
	friend bool operator==(A a, A b) {
		return a.i == b.i;
	}
	friend bool operator!=(A a, A b) {
		return !(a == b);
	}
};

std::size_t A::copy_count;
std::size_t A::move_count;

void test_move_iterator() {
	static constexpr std::size_t N = 42;
	std::vector<A> vec(N);
	std::vector<A> vec2;
	vec2.reserve(ranges::size(vec));

	A::clear();

	{
		auto first = ranges::make_move_iterator(ranges::begin(vec)),
			last = ranges::make_move_iterator(ranges::end(vec));
		static_assert(ranges::models::RandomAccessIterator<decltype(ranges::begin(vec))>);
		static_assert(ranges::models::InputIterator<decltype(first)>);
		static_assert(!ranges::models::ForwardIterator<decltype(first)>);
		auto out = ranges::back_inserter(vec2);

		for (; first != last; ++first, ++out) {
			*out = *first;
		}
	}

	CHECK(ranges::size(vec2) == N);
	CHECK(A::copy_count == std::size_t{0});
	CHECK(A::move_count == N);
}

void test_iter_move() {
	static constexpr std::size_t N = 42;
	std::vector<A> vec(N);
	std::vector<A> vec2;
	vec2.reserve(ranges::size(vec));

	A::clear();

	{
		auto first = ranges::begin(vec),
			last = ranges::end(vec);
		auto out = ranges::back_inserter(vec2);

		for (; first != last; ++first, ++out) {
			*out = ranges::iter_move(first);
		}
	}

	CHECK(ranges::size(vec2) == N);
	CHECK(A::copy_count == std::size_t{0});
	CHECK(A::move_count == N);
}

void test_both() {
	static constexpr std::size_t N = 42;
	std::vector<A> vec(N);
	std::vector<A> vec2;
	vec2.reserve(ranges::size(vec));

	A::clear();

	{
		auto first = ranges::make_move_iterator(ranges::begin(vec)),
			last = ranges::make_move_iterator(ranges::end(vec));
		auto out = ranges::back_inserter(vec2);

		for (; first != last; ++first, ++out) {
			*out = ranges::iter_move(first);
		}
	}

	CHECK(ranges::size(vec2) == N);
	CHECK(A::copy_count == std::size_t{0});
	CHECK(A::move_count == N);
}

template <class T>
class proxy_iterator {
public:
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using iterator_category = ranges::input_iterator_tag;

	proxy_iterator() = default;
	explicit proxy_iterator(T* p) :
		ptr_{std::make_shared<T*>(p)} {}

	struct readable_proxy {
		using value_type = T;
		mutable T cache_;
		T& operator*() const noexcept {
			return cache_;
		}
		friend T&& iter_move(const readable_proxy& p) noexcept {
			return std::move(p.cache_);
		}
	};

	ranges::reference_wrapper<T> operator*() const {
		return ranges::reference_wrapper<T>{**ptr_};
	}

	bool operator==(const proxy_iterator& that) const {
		return *ptr_ == *that.ptr_;
	}
	bool operator!=(const proxy_iterator& that) const {
		return !(*this == that);
	}

	proxy_iterator& operator++() & {
		// Input iterator is destructive!
		(*ptr_)->i = -1;
		++*ptr_;
		return *this;
	}
	readable_proxy operator++(int) & {
		readable_proxy tmp{__stl2::iter_move(*ptr_)};
		++*this;
		return tmp;
	}

	friend T&& iter_move(const proxy_iterator& p) {
		return ranges::iter_move(*p.ptr_);
	}

private:
	std::shared_ptr<T*> ptr_;
};

void test_proxy_iterator() {
	static constexpr std::size_t N = 42;
	std::vector<A> vec(N);
	std::vector<A> vec2;
	vec2.reserve(ranges::size(vec));

	static_assert(
		ranges::models::Same<
			ranges::reference_t<proxy_iterator<A>>,
			ranges::reference_wrapper<A>>);
	static_assert(
		ranges::models::Same<
			ranges::reference_t<const proxy_iterator<A>>,
			ranges::reference_wrapper<A>>);
	static_assert(
		ranges::models::Same<
			ranges::rvalue_reference_t<proxy_iterator<A>>,
			A&&>);

	{
		static_assert(
			ranges::models::Same<
				ranges::rvalue_reference_t<
					ranges::move_iterator<proxy_iterator<A>>>,
				A&&>);
		auto first = ranges::make_move_iterator(proxy_iterator<A>{ranges::data(vec)}),
			last = ranges::make_move_iterator(proxy_iterator<A>{ranges::data(vec) + ranges::size(vec)});
		auto out = ranges::back_inserter(vec2);

		std::iota(vec.begin(), vec.end(), 0);
		vec2.clear();
		A::clear();
		std::copy(first, last, out);

		CHECK(ranges::size(vec2) == N);
		CHECK(A::copy_count == std::size_t{0});
		CHECK(A::move_count == N);
		CHECK(ranges::equal(vec2, ranges::ext::take_exactly_view<ranges::ext::iota_view<int>>{0, N}, std::equal_to<>{}));
		CHECK(ranges::equal(vec, ranges::ext::repeat_n_view<int>{-1, N}, std::equal_to<>{}));

		first = ranges::make_move_iterator(proxy_iterator<A>{ranges::data(vec)});
		std::iota(vec.begin(), vec.end(), 0);
		vec2.clear();
		A::clear();
		while (first != last) // Test post-increment
			*out++ = *first++;

		CHECK(ranges::size(vec2) == N);
		CHECK(A::copy_count == std::size_t{0});
		CHECK(A::move_count == 2*N);
		CHECK(ranges::equal(vec2, ranges::ext::take_exactly_view<ranges::ext::iota_view<int>>{0, N}, std::equal_to<>{}));
		CHECK(ranges::equal(vec, ranges::ext::repeat_n_view<int>{-1, N}, std::equal_to<>{}));
	}

	{
		static_assert(
			ranges::models::Same<
				ranges::rvalue_reference_t<
					ranges::counted_iterator<proxy_iterator<A>>>,
				A&&>);
		static_assert(
			ranges::models::Same<
				ranges::rvalue_reference_t<
					ranges::move_iterator<
						ranges::counted_iterator<proxy_iterator<A>>>>,
				A&&>);
		auto first = ranges::make_move_iterator(
			ranges::make_counted_iterator(
				proxy_iterator<A>{ranges::data(vec)}, ranges::size(vec)));
		auto out = ranges::back_inserter(vec2);

		std::iota(vec.begin(), vec.end(), 0);
		vec2.clear();
		A::clear();
		ranges::copy(first, ranges::move_sentinel<ranges::default_sentinel>{}, out);

		CHECK(ranges::size(vec2) == N);
		CHECK(A::copy_count == std::size_t{0});
		CHECK(A::move_count == N);
	}

	{
		auto first = ranges::make_counted_iterator(vec.begin(), vec.size());
		auto last = ranges::default_sentinel{};
		static_assert(ranges::models::SizedSentinel<decltype(last), decltype(first)>);
		CHECK((static_cast<std::size_t>(last - first) == vec.size()));
		auto mfirst = ranges::make_move_iterator(first);
		auto mlast = ranges::make_move_sentinel(last);
		static_assert(ranges::models::SizedSentinel<decltype(mlast), decltype(mfirst)>);
		CHECK((static_cast<std::size_t>(mlast - mfirst) == vec.size()));
	}
}

int main() {
	test_move_iterator();
	test_iter_move();
	test_both();
	test_proxy_iterator();
	return ::test_result();
}
