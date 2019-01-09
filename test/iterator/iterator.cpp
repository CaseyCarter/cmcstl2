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
#include <cstring>
#include <iostream>

#include <stl2/iterator.hpp>
#include <stl2/memory.hpp>
#include <stl2/detail/raw_ptr.hpp>

#include "../simple_test.hpp"

namespace ranges = __stl2;

template<class T>
struct reference_wrapper {
private:
	ranges::detail::raw_ptr<T> ptr_;

	static T* f(T& t) noexcept { return std::addressof(t); }
	static void f(T&&) = delete;
public:
	reference_wrapper() = default;

	template<ranges::_NotSameAs<reference_wrapper> U>
	reference_wrapper(U&& u)
	noexcept(noexcept(f(static_cast<U&&>(u))))
	requires requires { f(static_cast<U&&>(u)); }
	: ptr_{f(static_cast<U&&>(u))} {}

	T& get() const noexcept {
		return *ptr_;
	}

	reference_wrapper& operator=(const T& t)
		noexcept(std::is_nothrow_copy_assignable<T>::value) {
		get() = t;
		return *this;
	}
	reference_wrapper& operator=(T&& t)
		noexcept(std::is_nothrow_move_assignable<T>::value) {
		get() = std::move(t);
		return *this;
	}

	const reference_wrapper& operator=(const T& t) const
		noexcept(std::is_nothrow_copy_assignable<T>::value) {
		get() = t;
		return *this;
	}
	const reference_wrapper& operator=(T&& t) const
		noexcept(std::is_nothrow_move_assignable<T>::value) {
		get() = std::move(t);
		return *this;
	}

	operator T&() const noexcept { return get(); }
};

template<class T, std::size_t N>
struct array {
	T e_[N];

	using reference = reference_wrapper<T>;
	using const_reference = reference_wrapper<const T>;

	reference operator[](const std::size_t n) noexcept {
		return {e_[n]};
	}
	const_reference operator[](const std::size_t n) const noexcept {
		return {e_[n]};
	}

	struct iterator {
		using iterator_category = ranges::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;

		T* ptr_;

		iterator() = default;
		iterator(T* p) noexcept : ptr_{p} {}

		reference operator*() const noexcept {
			STL2_EXPECT(ptr_);
			return {*ptr_};
		}

		T* operator->() const noexcept
		requires std::is_class<T>::value || std::is_union<T>::value
		{
			STL2_EXPECT(ptr_);
			return ptr_;
		}

		iterator& operator++() noexcept {
			++ptr_;
			return *this;
		}

		iterator operator++(int) noexcept {
			auto tmp = *this;
			++*this;
			return tmp;
		}

		bool operator==(const iterator& other) const noexcept {
			return ptr_ == other.ptr_;
		}

		bool operator!=(const iterator& other) const noexcept {
			return !(*this == other);
		}

		iterator operator+(std::ptrdiff_t n) const noexcept {
			return {ptr_ + n};
		}

		friend T&& iter_move(iterator i) noexcept {
			//std::cout << "iter_move(" << static_cast<void*>(i.ptr_) << ")\n";
			STL2_EXPECT(i.ptr_);
			return static_cast<T&&>(*i.ptr_);
		}
	};

	iterator begin() { return {&e_[0]}; }
	iterator end() { return {&e_[0] + N}; }
};

enum class category {
	none, output, input, forward, bidirectional, random_access, contiguous
};

std::ostream& operator<<(std::ostream& sout, category c) {
	switch(c) {
	case category::output:
		return sout << "category::output";
	case category::input:
		return sout << "category::input";
	case category::forward:
		return sout << "category::forward";
	case category::bidirectional:
		return sout << "category::bidirectional";
	case category::random_access:
		return sout << "category::random_access";
	case category::contiguous:
		return sout << "category::contiguous";
	default:
		return sout << "category::none";
	}
}

template<class>
constexpr category iterator_dispatch() { return category::none; }
template<ranges::OutputIterator<const int&> I>
requires !ranges::InputIterator<I>
constexpr category iterator_dispatch() { return category::output; }
template<ranges::InputIterator>
constexpr category iterator_dispatch() { return category::input; }
template<ranges::ForwardIterator>
constexpr category iterator_dispatch() { return category::forward; }
template<ranges::BidirectionalIterator>
constexpr category iterator_dispatch() { return category::bidirectional; }
template<ranges::RandomAccessIterator>
constexpr category iterator_dispatch() { return category::random_access; }
template<ranges::ContiguousIterator>
constexpr category iterator_dispatch() { return category::contiguous; }

template<class C, bool EC, class R = int&>
struct arbitrary_iterator {
	using difference_type = std::ptrdiff_t;

	arbitrary_iterator& operator*();
	arbitrary_iterator& operator=(std::remove_reference_t<R>);

	arbitrary_iterator& operator++();
	arbitrary_iterator& operator++(int);

	bool operator==(arbitrary_iterator) const
	requires EC;
	bool operator!=(arbitrary_iterator) const
	requires EC;
};

template<ranges::DerivedFrom<ranges::input_iterator_tag> C, bool EC, class R>
struct arbitrary_iterator<C, EC, R> {
	using iterator_category = C;
	using value_type = std::remove_reference_t<R>;
	using difference_type = std::ptrdiff_t;

	R operator*() const;

	arbitrary_iterator& operator++();
	arbitrary_iterator operator++(int);

	bool operator==(arbitrary_iterator) const
	requires EC || ranges::DerivedFrom<C, ranges::forward_iterator_tag>;
	bool operator!=(arbitrary_iterator) const
	requires EC || ranges::DerivedFrom<C, ranges::forward_iterator_tag>;

	arbitrary_iterator& operator--()
	requires ranges::DerivedFrom<C, ranges::bidirectional_iterator_tag>;
	arbitrary_iterator operator--(int)
	requires ranges::DerivedFrom<C, ranges::bidirectional_iterator_tag>;

	bool operator<(arbitrary_iterator) const
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;
	bool operator>(arbitrary_iterator) const
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;
	bool operator<=(arbitrary_iterator) const
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;
	bool operator>=(arbitrary_iterator) const
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;

	arbitrary_iterator& operator+=(difference_type)
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;
	arbitrary_iterator& operator-=(difference_type)
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;

	arbitrary_iterator operator-(difference_type) const
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;
	difference_type operator-(arbitrary_iterator) const
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;

	value_type& operator[](difference_type) const
	requires ranges::DerivedFrom<C, ranges::random_access_iterator_tag>;
};

template<ranges::DerivedFrom<ranges::random_access_iterator_tag> C, bool B, class R>
arbitrary_iterator<C, B, R> operator+(
	arbitrary_iterator<C, B, R>, typename arbitrary_iterator<C, B, R>::difference_type);

template<ranges::DerivedFrom<ranges::random_access_iterator_tag> C, bool B, class R>
arbitrary_iterator<C, B, R> operator+(
	typename arbitrary_iterator<C, B, R>::difference_type, arbitrary_iterator<C, B, R>);

void test_iterator_dispatch() {
	CHECK(iterator_dispatch<void>() == category::none);
	static_assert(ranges::ContiguousIterator<int*>);
	CHECK(iterator_dispatch<int*>() == category::contiguous);

	{
		static_assert(ranges::Readable<int* const>);
	}

	{
		using I = arbitrary_iterator<ranges::input_iterator_tag, false>;
		static_assert(ranges::InputIterator<I>);
		CHECK(iterator_dispatch<I>() == category::input);
	}
	{
		using I = arbitrary_iterator<ranges::input_iterator_tag, true>;
		static_assert(ranges::InputIterator<I>);
		static_assert(ranges::EqualityComparable<I>);
		CHECK(iterator_dispatch<I>() == category::input);
	}
	{
		using I = arbitrary_iterator<ranges::forward_iterator_tag, true>;
		static_assert(ranges::ForwardIterator<I>);
		CHECK(iterator_dispatch<I>() == category::forward);
	}
	{
		using I = arbitrary_iterator<ranges::bidirectional_iterator_tag, true>;
		static_assert(ranges::BidirectionalIterator<I>);
		CHECK(iterator_dispatch<I>() == category::bidirectional);
	}
	{
		using I = arbitrary_iterator<ranges::random_access_iterator_tag, true>;
		static_assert(ranges::RandomAccessIterator<I>);
		CHECK(iterator_dispatch<I>() == category::random_access);
	}
	{
		using I = arbitrary_iterator<ranges::contiguous_iterator_tag, true>;
		static_assert(ranges::ContiguousIterator<I>);
		CHECK(iterator_dispatch<I>() == category::contiguous);
	}

	{
		using I = arbitrary_iterator<void, false>;
		static_assert(ranges::OutputIterator<I, const int&>);
		static_assert(!ranges::InputIterator<I>);
		CHECK(iterator_dispatch<I>() == category::output);
	}
	{
		using I = arbitrary_iterator<void, true>;
		static_assert(ranges::OutputIterator<I, const int&>);
		static_assert(ranges::EqualityComparable<I>);
		static_assert(!ranges::InputIterator<I>);
		CHECK(iterator_dispatch<I>() == category::output);
	}
}

template<ranges::InputIterator I, ranges::Sentinel<I> S, class O>
requires ranges::IndirectlyCopyable<I, O>
bool copy(I first, S last, O o) {
	for (; first != last; ++first, ++o) {
		*o = *first;
	}
	return false;
}

template<ranges::ContiguousIterator I, ranges::SizedSentinel<I> S,
	ranges::ContiguousIterator O>
requires
	ranges::IndirectlyCopyable<I, O> &&
	ranges::Same<ranges::iter_value_t<I>, ranges::iter_value_t<O>> &&
	std::is_trivially_copyable<ranges::iter_value_t<I>>::value
bool copy(I first, S last, O o) {
	auto n = last - first;
	STL2_EXPECT(n >= 0);
	if (n) {
		std::memmove(std::addressof(*o), std::addressof(*first),
			n * sizeof(ranges::iter_value_t<I>));
	}
	return true;
}

void test_copy() {
	{
		struct A {
			int value;
			A(int i) : value{i} {}
			A(const A& that) :
				value{that.value} {}
			A& operator=(const A&) = default;
		};
		A a[] = {0,1,2,3}, b[] = {4,5,6,7};
		CHECK(!copy(ranges::begin(a) + 1, ranges::end(a) - 1, ranges::begin(b) + 1));
		CHECK(b[0].value == 4);
		CHECK(b[1].value == 1);
		CHECK(b[2].value == 2);
		CHECK(b[3].value == 7);
	}

	{
		int a[] = {0,1,2,3,4,5,6,7}, b[] = {7,6,5,4,3,2,1,0};
		CHECK(copy(ranges::begin(a) + 2, ranges::end(a) - 2, ranges::begin(b) + 2));
		CHECK(b[0] == 7);
		CHECK(b[1] == 6);
		CHECK(b[2] == 2);
		CHECK(b[3] == 3);
		CHECK(b[4] == 4);
		CHECK(b[5] == 5);
		CHECK(b[6] == 1);
		CHECK(b[7] == 0);
	}
}

void test_iter_swap2() {
	{
		int a[] = { 42, 13 };
		ranges::iter_swap(a + 0, a + 1);
		CHECK(a[0] == 13);
		CHECK(a[1] == 42);
		ranges::iter_swap(a + 0, a + 1);
		CHECK(a[0] == 42);
		CHECK(a[1] == 13);
	}

	{
		auto a = std::make_unique<int>(42);
		auto b = std::make_unique<int>(13);
		using I = decltype(a);
		static_assert(ranges::Same<I, decltype(b)>);
		static_assert(ranges::Readable<I>);
		using R = ranges::iter_reference_t<I>;
		static_assert(ranges::Same<int&, R>);
		using RR = ranges::iter_rvalue_reference_t<I>;
		static_assert(ranges::Same<int&&, RR>);
		static_assert(ranges::SwappableWith<R, R>);

		// Swappable<R, R> is true, calls the first overload of
		// iter_swap (which delegates to swap(*a, *b)):
		ranges::iter_swap(a, b);
		CHECK(*a == 13);
		CHECK(*b == 42);
		ranges::iter_swap(a, b);
		CHECK(*a == 42);
		CHECK(*b == 13);
	}

	{
		auto a = array<int, 4>{0,1,2,3};
		using I = decltype(a.begin());
		static_assert(ranges::Readable<I>);
		using V = ranges::iter_value_t<I>;
		static_assert(ranges::Same<int, V>);
		using R = ranges::iter_reference_t<I>;
		static_assert(ranges::Same<reference_wrapper<int>, R>);
		using RR = ranges::iter_rvalue_reference_t<I>;
		static_assert(ranges::Same<int&&, RR>);

		static_assert(ranges::Same<I, decltype(a.begin() + 2)>);
		static_assert(ranges::CommonReference<const R&, const R&>);
		static_assert(!ranges::SwappableWith<R, R>);
		static_assert(ranges::IndirectlyMovableStorable<I, I>);

		// Swappable<R, R> is not satisfied, and
		// IndirectlyMovableStorable<I, I> is satisfied,
		// so this should resolve to the second overload of iter_swap.
		ranges::iter_swap(a.begin() + 1, a.begin() + 3);
		CHECK(a[0] == 0);
		CHECK(a[1] == 3);
		CHECK(a[2] == 2);
		CHECK(a[3] == 1);
	}
}

template<class T>
constexpr bool has_category = false;
template<class T>
requires requires { typename T::iterator_category; }
constexpr bool has_category<T> = true;

void test_std_traits() {
	using WO = arbitrary_iterator<void, false>;
	static_assert(ranges::Same<std::iterator_traits<WO>::iterator_category,
		std::output_iterator_tag>);

	using O = arbitrary_iterator<void, true>;
	static_assert(ranges::Same<std::iterator_traits<O>::iterator_category,
		std::output_iterator_tag>);

	using WI = arbitrary_iterator<ranges::input_iterator_tag, false>;
	static_assert(!has_category<std::iterator_traits<WI>>);

	using I = arbitrary_iterator<ranges::input_iterator_tag, true>;
	static_assert(ranges::Same<std::iterator_traits<I>::iterator_category,
		std::input_iterator_tag>);

	using F = arbitrary_iterator<ranges::forward_iterator_tag, true>;
	static_assert(ranges::Same<std::iterator_traits<F>::iterator_category,
		std::forward_iterator_tag>);

	using B = arbitrary_iterator<ranges::bidirectional_iterator_tag, true>;
	static_assert(ranges::Same<std::iterator_traits<B>::iterator_category,
		std::bidirectional_iterator_tag>);

	using R = arbitrary_iterator<ranges::random_access_iterator_tag, true>;
	static_assert(ranges::Same<std::iterator_traits<R>::iterator_category,
		std::random_access_iterator_tag>);

	using C = arbitrary_iterator<ranges::contiguous_iterator_tag, true>;
	static_assert(ranges::Same<std::iterator_traits<C>::iterator_category,
		std::random_access_iterator_tag>);

	using IV = arbitrary_iterator<ranges::input_iterator_tag, true, int>;
	static_assert(ranges::Same<std::iterator_traits<IV>::iterator_category,
		std::input_iterator_tag>);

	using FV = arbitrary_iterator<ranges::forward_iterator_tag, true, int>;
	static_assert(ranges::Same<std::iterator_traits<FV>::iterator_category,
		std::input_iterator_tag>);

	using BV = arbitrary_iterator<ranges::bidirectional_iterator_tag, true, int>;
	static_assert(ranges::Same<std::iterator_traits<BV>::iterator_category,
		std::input_iterator_tag>);

	using RV = arbitrary_iterator<ranges::random_access_iterator_tag, true, int>;
	static_assert(ranges::Same<std::iterator_traits<RV>::iterator_category,
		std::input_iterator_tag>);
}

struct MakeString {
	using value_type = std::string;
	std::string operator*() const;
};

static_assert(ranges::Readable<MakeString>);
static_assert(!ranges::Writable<MakeString, std::string>);
static_assert(!ranges::Writable<MakeString, const std::string &>);
static_assert(!ranges::IndirectlyMovable<MakeString, MakeString>);
static_assert(!ranges::IndirectlyCopyable<MakeString, MakeString>);
static_assert(!ranges::IndirectlySwappable<MakeString, MakeString>);

int main() {
	test_iter_swap2();
	test_iterator_dispatch();
	test_copy();
	test_std_traits();

	return ::test_result();
}
