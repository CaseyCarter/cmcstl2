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
#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <valarray>
#include <vector>
#include <stl2/iterator.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/core.hpp>

namespace ranges = std::experimental::ranges;

namespace associated_type_test {
	struct A {
		using value_type = int;
		int& operator*() const;
	};
	struct B : A {
		using value_type = double;
	};

	template<class, class = void>
	constexpr bool has_member_value_type = false;
	template<class T>
	constexpr bool has_member_value_type<T, std::void_t<typename T::value_type>> = true;

	static_assert(ranges::same_as<int&, ranges::iter_reference_t<int*>>);
	static_assert(ranges::same_as<int&, ranges::iter_reference_t<int[]>>);
	static_assert(ranges::same_as<int&, ranges::iter_reference_t<int[4]>>);
	static_assert(ranges::same_as<int&, ranges::iter_reference_t<A>>);
	static_assert(ranges::same_as<int&, ranges::iter_reference_t<B>>);
	static_assert(ranges::same_as<const int&, ranges::iter_reference_t<const int*>>);

	static_assert(ranges::same_as<int&&, ranges::iter_rvalue_reference_t<int*>>);
	static_assert(ranges::same_as<int&&, ranges::iter_rvalue_reference_t<int[]>>);
	static_assert(ranges::same_as<int&&, ranges::iter_rvalue_reference_t<int[4]>>);
	static_assert(ranges::same_as<int&&, ranges::iter_rvalue_reference_t<A>>);
	static_assert(ranges::same_as<int&&, ranges::iter_rvalue_reference_t<B>>);
	static_assert(ranges::same_as<const int&&, ranges::iter_rvalue_reference_t<const int*>>);

	static_assert(ranges::same_as<int, ranges::iter_value_t<int*>>);
	static_assert(ranges::same_as<int, ranges::iter_value_t<int[]>>);
	static_assert(ranges::same_as<int, ranges::iter_value_t<int[4]>>);
	static_assert(ranges::same_as<int, ranges::iter_value_t<A>>);
	static_assert(ranges::same_as<double, ranges::iter_value_t<B>>);
	static_assert(ranges::same_as<int, ranges::iter_value_t<const int*>>);
	static_assert(!has_member_value_type<ranges::readable_traits<void>>);
	static_assert(!has_member_value_type<ranges::readable_traits<void*>>);
	static_assert(ranges::same_as<int, ranges::iter_value_t<const int* const>>);
	static_assert(ranges::same_as<int, ranges::iter_value_t<const int[2]>>);
	struct S { using value_type = int; using element_type = int const; };
	// ranges::readable_traits<S> // ill-formed, hard error

	static_assert(ranges::same_as<std::ptrdiff_t, ranges::iter_difference_t<int*>>);
	static_assert(ranges::same_as<std::ptrdiff_t, ranges::iter_difference_t<int[]>>);
	static_assert(ranges::same_as<std::ptrdiff_t, ranges::iter_difference_t<int[4]>>);

	static_assert(!meta::is_trait<ranges::incrementable_traits<void>>());
	static_assert(!meta::is_trait<ranges::incrementable_traits<void*>>());

	static_assert(ranges::same_as<int, ranges::iter_difference_t<int>>);
	static_assert(ranges::same_as<ranges::iterator_category_t<int*>, ranges::contiguous_iterator_tag>);
	static_assert(ranges::same_as<ranges::iterator_category_t<const int*>, ranges::contiguous_iterator_tag>);

	template<class T>
	struct derive_from : T {};

	template<class Cat, class Value, class Distance = std::ptrdiff_t,
		class Pointer = Value*, class Reference = Value&>
	struct not_std_iterator {
		using iterator_category = Cat;
		using value_type = Value;
		using difference_type = Distance;
		using pointer = Pointer;
		using reference = Reference;
	};

	template<class T, bool Derive>
	using iterator =
		not_std_iterator<std::conditional_t<Derive, derive_from<T>, T>,
			std::conditional_t<std::is_same_v<T, std::output_iterator_tag>, int, void>,
			std::conditional_t<std::is_same_v<T, std::output_iterator_tag>, std::ptrdiff_t, void>,
			std::conditional_t<std::is_same_v<T, std::output_iterator_tag>, int*, void>,
			std::conditional_t<std::is_same_v<T, std::output_iterator_tag>, int&, void>>;

	template<class T, bool B, class U>
	using test = std::is_same<ranges::iterator_category_t<iterator<T, B>>, U>;

	static_assert(!meta::is_trait<ranges::iterator_category<void*>>());
	static_assert(!meta::is_trait<ranges::iterator_category<int(*)()>>());
	static_assert(!meta::is_trait<ranges::iterator_category<iterator<std::output_iterator_tag, false>>>());
	static_assert(!meta::is_trait<ranges::iterator_category<iterator<std::output_iterator_tag, true>>>());

	static_assert(test<std::input_iterator_tag, false, ranges::input_iterator_tag>());
	static_assert(test<std::forward_iterator_tag, false, ranges::forward_iterator_tag>());
	static_assert(test<std::bidirectional_iterator_tag, false, ranges::bidirectional_iterator_tag>());
	static_assert(test<std::random_access_iterator_tag, false, ranges::random_access_iterator_tag>());

	static_assert(test<std::input_iterator_tag, true, ranges::input_iterator_tag>());
	static_assert(test<std::forward_iterator_tag, true, ranges::forward_iterator_tag>());
	static_assert(test<std::bidirectional_iterator_tag, true, ranges::bidirectional_iterator_tag>());
	static_assert(test<std::random_access_iterator_tag, true, ranges::random_access_iterator_tag>());

	struct foo {};
	static_assert(test<foo, false, foo>());

#if STL2_HOOK_ITERATOR_TRAITS
	// Some sanity tests
	struct my_wonky_tag : std::random_access_iterator_tag, ranges::random_access_iterator_tag {};
	struct my_wonky_tag2 : std::input_iterator_tag, ranges::random_access_iterator_tag {};
	static_assert(std::is_same<ranges::detail::stl2_to_std_iterator_category<my_wonky_tag, int>, my_wonky_tag>::value, "");
	static_assert(std::is_same<ranges::detail::stl2_to_std_iterator_category<my_wonky_tag, int&>, my_wonky_tag>::value, "");
	static_assert(std::is_same<ranges::detail::stl2_to_std_iterator_category<my_wonky_tag2, int>, my_wonky_tag2>::value, "");
	static_assert(std::is_same<ranges::detail::stl2_to_std_iterator_category<my_wonky_tag2, int&>, my_wonky_tag2>::value, "");
	struct my_wonky_tag3 : ranges::random_access_iterator_tag {};
	static_assert(std::is_same<ranges::detail::stl2_to_std_iterator_category<my_wonky_tag3, int>, std::input_iterator_tag>::value, "");
	static_assert(std::is_same<ranges::detail::stl2_to_std_iterator_category<my_wonky_tag3, int&>, std::random_access_iterator_tag>::value, "");
	static_assert(std::is_same<ranges::detail::stl2_to_std_iterator_category<ranges::input_iterator_tag, int>, std::input_iterator_tag>::value, "");
	static_assert(std::is_same<ranges::detail::stl2_to_std_iterator_category<ranges::input_iterator_tag, int&>, std::input_iterator_tag>::value, "");
#endif // STL2_HOOK_ITERATOR_TRAITS
} // namespace associated_type_test

namespace readable_test {
	struct A {
		int operator*() const;
		using value_type = int;
	};

	static_assert(!ranges::readable<void>);
	static_assert(!ranges::readable<void*>);
	static_assert(ranges::readable<int*>);
	static_assert(ranges::readable<const int*>);
	static_assert(ranges::readable<A>);
	static_assert(ranges::same_as<ranges::iter_value_t<A>,int>);

	struct MoveOnlyReadable {
		using value_type = std::unique_ptr<int>;
		value_type operator*() const;
	};

	static_assert(ranges::readable<MoveOnlyReadable>);

	struct ArrayReadable {
		using value_type = int[2];
		value_type& operator*() const;
	};

	static_assert(ranges::readable<ArrayReadable>);

	struct Abstract {
		virtual void foo() = 0;
	};
	struct AbstractReadable {
		using value_type = Abstract;
		Abstract& operator*() const;
	};

	static_assert(ranges::readable<AbstractReadable>);
}

namespace writable_test {
	struct A {
		int& operator*() const;
	};

	static_assert(ranges::writable<std::unique_ptr<int>*, std::unique_ptr<int>&&>);
	static_assert(!ranges::writable<std::unique_ptr<int>*, std::unique_ptr<int>&>);
	static_assert(!ranges::writable<void, int>);
	static_assert(!ranges::writable<void*, void>);
	static_assert(ranges::writable<int*, int>);
	static_assert(ranges::writable<int*, int&>);
	static_assert(ranges::writable<int*, const int&>);
	static_assert(ranges::writable<int*, const int>);
	static_assert(!ranges::writable<const int*, int>);
	static_assert(ranges::writable<A, int>);
	static_assert(ranges::writable<A, const int&>);
	static_assert(ranges::writable<A, double>);
	static_assert(ranges::writable<A, const double&>);
} // namespace writable_test

static_assert(ranges::weakly_incrementable<int>);
static_assert(ranges::weakly_incrementable<unsigned int>);
static_assert(!ranges::weakly_incrementable<void>);
static_assert(ranges::weakly_incrementable<int*>);
static_assert(ranges::weakly_incrementable<const int*>);

static_assert(ranges::incrementable<int>);
static_assert(ranges::incrementable<unsigned int>);
static_assert(!ranges::incrementable<void>);
static_assert(ranges::incrementable<int*>);
static_assert(ranges::incrementable<const int*>);

namespace iterator_sentinel_test {
	struct A {
		using difference_type = signed char;
		using iterator_category = ranges::input_iterator_tag;
		using value_type = double;

		A& operator++();
		A operator++(int);
		double operator*() const;

		bool operator == (const A&) const;
		bool operator != (const A&) const;
	};

	static_assert(ranges::input_or_output_iterator<int*>);
	static_assert(ranges::input_or_output_iterator<const int*>);
	static_assert(!ranges::input_or_output_iterator<void*>);
	static_assert(ranges::input_or_output_iterator<A>);
	static_assert(ranges::input_iterator<A>);

	static_assert(ranges::input_or_output_iterator<int*>);
	static_assert(ranges::sentinel_for<int*, int*>);
	static_assert(ranges::sentinel_for<const int*, const int*>);
	static_assert(ranges::sentinel_for<const int*, int*>);
	static_assert(!ranges::sentinel_for<void*, void*>);
	static_assert(ranges::sentinel_for<A, A>);
} // namespace iterator_sentinel_test

namespace indirectly_callable_test {
	static_assert(ranges::ext::indirect_invocable<std::plus<int>, int*, int*>);
}

namespace indirect_invoke_result_test {
	template<class R, class... Args>
	using fn_t = R(Args...);
	static_assert(ranges::same_as<ranges::indirect_result_t<fn_t<void, int>&, const int*>, void>);
}

namespace contiguous_test {
	template<class T>
	struct fancy {
		using iterator_category = std::random_access_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::remove_cv_t<T>;
		using pointer = T*;
		using reference = std::add_lvalue_reference_t<T>;

		static fancy pointer_to(T& t) { return fancy{std::addressof(t)}; }

		fancy() = default;
		fancy(std::nullptr_t) : ptr_{} {}
		explicit fancy(T* ptr) : ptr_{ptr} {}

		explicit operator bool() const { return ptr_ != nullptr; }

		reference operator*() const { return *ptr_; }
		T* operator->() const { return ptr_; }

		fancy& operator++() { ++ptr_; return *this; }
		fancy operator++(int) { auto tmp = *this; ++*this; return tmp; }
		fancy& operator--() { --ptr_; return *this; }
		fancy operator--(int) { auto tmp = *this; --*this; return tmp; }

		fancy& operator+=(difference_type n) { ptr_ += n; return *this; }
		fancy operator+(difference_type n) const { return fancy{ptr_ + n}; }
		friend fancy operator+(difference_type n, fancy x) { return x + n; }
		fancy& operator-=(difference_type n) { *this += -n; return *this; }
		fancy operator-(difference_type n) const { return fancy{ptr_ - n}; }
		difference_type operator-(fancy x) const { return ptr_ - x.ptr_; }

		bool operator==(fancy x) const { return ptr_ == x.ptr_; }
		bool operator!=(fancy x) const { return !(*this == x); }
		bool operator< (fancy x) const { return ptr_ < x.ptr_; }
		bool operator> (fancy x) const { return x < *this; }
		bool operator<=(fancy x) const { return !(x < *this); }
		bool operator>=(fancy x) const { return !(*this < x); }

		T* ptr_;
	};

	template<class T>
	struct allocator {
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = std::add_lvalue_reference_t<T>;
		using const_reference = std::add_lvalue_reference_t<const T>;
		using pointer = fancy<T>;
		using const_pointer = fancy<const T>;
		using void_pointer = fancy<void>;
		using const_void_pointer = fancy<const void>;

		template<class U> struct rebind {
			using other = allocator<U>;
		};

		allocator() = default;
		template<class U>
		allocator(const allocator<U>&);

		fancy<T> allocate(std::size_t);
		void deallocate(fancy<T>, std::size_t) noexcept;

		template<class U>
		bool operator==(allocator<U>);
		template<class U>
		bool operator!=(allocator<U>);
	};

	static_assert(ranges::contiguous_iterator<std::array<int, 42>::iterator>);
	static_assert(ranges::contiguous_iterator<std::array<int, 42>::const_iterator>);
	static_assert(ranges::contiguous_iterator<std::string::iterator>);
	static_assert(ranges::contiguous_iterator<std::string::const_iterator>);
	static_assert(ranges::contiguous_iterator<std::basic_string<char, std::char_traits<char>, allocator<char>>::iterator>);
	static_assert(ranges::contiguous_iterator<std::basic_string<char, std::char_traits<char>, allocator<char>>::const_iterator>);
	static_assert(ranges::contiguous_iterator<std::string_view::iterator>);
	static_assert(ranges::contiguous_iterator<std::string_view::const_iterator>);
	static_assert(ranges::contiguous_iterator<ranges::iterator_t<std::valarray<double>>>);
	static_assert(ranges::contiguous_iterator<ranges::iterator_t<const std::valarray<double>>>);
	static_assert(ranges::contiguous_iterator<std::vector<int>::iterator>);
	static_assert(ranges::contiguous_iterator<std::vector<int>::const_iterator>);
	static_assert(ranges::contiguous_iterator<std::vector<int, allocator<int>>::iterator>);
	static_assert(ranges::contiguous_iterator<std::vector<int, allocator<int>>::const_iterator>);
}

int main() {}
