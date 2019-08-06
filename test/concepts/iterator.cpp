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
#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ns {
	template<class I>
	using iter_difference_t = ranges::iterator_difference_t<I>;

	template<class I>
	using iterator_category_t = ranges::iterator_category_t<I>;

	template<class I>
	using iter_reference_t = ranges::iterator_reference_t<I>;

	template<class I>
	using iter_rvalue_reference_t = ranges::iterator_rvalue_reference_t<I>;

	template<class I>
	using iter_value_t = ranges::iterator_value_t<I>;

	using ranges::iterator_category;

	using ranges::input_iterator_tag;
	using ranges::forward_iterator_tag;
	using ranges::bidirectional_iterator_tag;
	using ranges::random_access_iterator_tag;

	using ranges::indirect_result_t;
}

#elif VALIDATE_STL2
#include <stl2/iterator.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/core.hpp>

namespace ns = ::__stl2;
#endif

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <valarray>
#include <vector>

#include "../simple_test.hpp"

namespace associated_type_test {
	struct A { using value_type = int; int& operator*() const; };
	struct B : A { using value_type = double; };

	template<class, class = void>
	constexpr bool has_member_value_type = false;
	template<class T>
	constexpr bool has_member_value_type<T, std::void_t<typename T::value_type>> = true;

	CONCEPT_ASSERT(ranges::same_as<int&, ns::iter_reference_t<int*>>);
	CONCEPT_ASSERT(ranges::same_as<int&, ns::iter_reference_t<int[]>>);
	CONCEPT_ASSERT(ranges::same_as<int&, ns::iter_reference_t<int[4]>>);
	CONCEPT_ASSERT(ranges::same_as<int&, ns::iter_reference_t<A>>);
	CONCEPT_ASSERT(ranges::same_as<int&, ns::iter_reference_t<B>>);
	CONCEPT_ASSERT(ranges::same_as<const int&, ns::iter_reference_t<const int*>>);

	CONCEPT_ASSERT(ranges::same_as<int&&, ns::iter_rvalue_reference_t<int*>>);
	CONCEPT_ASSERT(ranges::same_as<int&&, ns::iter_rvalue_reference_t<int[]>>);
	CONCEPT_ASSERT(ranges::same_as<int&&, ns::iter_rvalue_reference_t<int[4]>>);
	CONCEPT_ASSERT(ranges::same_as<int&&, ns::iter_rvalue_reference_t<A>>);
	CONCEPT_ASSERT(ranges::same_as<int&&, ns::iter_rvalue_reference_t<B>>);
	CONCEPT_ASSERT(ranges::same_as<const int&&, ns::iter_rvalue_reference_t<const int*>>);

	CONCEPT_ASSERT(ranges::same_as<int, ns::iter_value_t<int*>>);
	CONCEPT_ASSERT(ranges::same_as<int, ns::iter_value_t<int[]>>);
	CONCEPT_ASSERT(ranges::same_as<int, ns::iter_value_t<int[4]>>);
	CONCEPT_ASSERT(ranges::same_as<int, ns::iter_value_t<A>>);
	CONCEPT_ASSERT(ranges::same_as<double, ns::iter_value_t<B>>);
	CONCEPT_ASSERT(ranges::same_as<int, ns::iter_value_t<const int*>>);
	CONCEPT_ASSERT(!has_member_value_type<ns::readable_traits<void>>);
	CONCEPT_ASSERT(!has_member_value_type<ns::readable_traits<void*>>);
	CONCEPT_ASSERT(ranges::same_as<int, ns::iter_value_t<const int* const>>);
	CONCEPT_ASSERT(ranges::same_as<int, ns::iter_value_t<const int[2]>>);
	struct S { using value_type = int; using element_type = int const; };
	// ns::readable_traits<S> // ill-formed, hard error

	CONCEPT_ASSERT(ranges::same_as<std::ptrdiff_t, ns::iter_difference_t<int*>>);
	CONCEPT_ASSERT(ranges::same_as<std::ptrdiff_t, ns::iter_difference_t<int[]>>);
	CONCEPT_ASSERT(ranges::same_as<std::ptrdiff_t, ns::iter_difference_t<int[4]>>);

	CONCEPT_ASSERT(!meta::is_trait<ns::incrementable_traits<void>>());
	CONCEPT_ASSERT(!meta::is_trait<ns::incrementable_traits<void*>>());

	CONCEPT_ASSERT(ranges::same_as<int, ns::iter_difference_t<int>>);
#if VALIDATE_STL2
	CONCEPT_ASSERT(ranges::same_as<ns::iterator_category_t<int*>, ns::contiguous_iterator_tag>);
	CONCEPT_ASSERT(ranges::same_as<ns::iterator_category_t<const int*>, ns::contiguous_iterator_tag>);
#elif VALIDATE_RANGES
	CONCEPT_ASSERT(ranges::same_as<ns::iterator_category_t<int*>, ns::random_access_iterator_tag>);
	CONCEPT_ASSERT(ranges::same_as<ns::iterator_category_t<const int*>, ns::random_access_iterator_tag>);
#endif

	template<class T>
	struct derive_from : T {};

	template<class T, bool Derive>
	using iterator =
		meta::apply<
			meta::bind_front<
				meta::quote<std::iterator>,
				meta::if_c<Derive, derive_from<T>, T>>,
			meta::if_<
				std::is_same<T, std::output_iterator_tag>,
				meta::list<void, void, void, void>,
				meta::list<int>>>;

	template<class T, bool B, class U>
	using test = std::is_same<ns::iterator_category_t<iterator<T, B>>, U>;

	CONCEPT_ASSERT(!meta::is_trait<ns::iterator_category<void*>>());
	CONCEPT_ASSERT(!meta::is_trait<ns::iterator_category<int(*)()>>());
	CONCEPT_ASSERT(!meta::is_trait<ns::iterator_category<iterator<std::output_iterator_tag, false>>>());
	CONCEPT_ASSERT(!meta::is_trait<ns::iterator_category<iterator<std::output_iterator_tag, true>>>());

	CONCEPT_ASSERT(test<std::input_iterator_tag, false, ns::input_iterator_tag>());
	CONCEPT_ASSERT(test<std::forward_iterator_tag, false, ns::forward_iterator_tag>());
	CONCEPT_ASSERT(test<std::bidirectional_iterator_tag, false, ns::bidirectional_iterator_tag>());
	CONCEPT_ASSERT(test<std::random_access_iterator_tag, false, ns::random_access_iterator_tag>());

	CONCEPT_ASSERT(test<std::input_iterator_tag, true, ns::input_iterator_tag>());
	CONCEPT_ASSERT(test<std::forward_iterator_tag, true, ns::forward_iterator_tag>());
	CONCEPT_ASSERT(test<std::bidirectional_iterator_tag, true, ns::bidirectional_iterator_tag>());
	CONCEPT_ASSERT(test<std::random_access_iterator_tag, true, ns::random_access_iterator_tag>());

	struct foo {};
	CONCEPT_ASSERT(test<foo, false, foo>());

	// Some sanity tests
	struct my_wonky_tag : std::random_access_iterator_tag, ns::random_access_iterator_tag {};
	struct my_wonky_tag2 : std::input_iterator_tag, ns::random_access_iterator_tag {};
	static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag, int>, my_wonky_tag>::value, "");
	static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag, int&>, my_wonky_tag>::value, "");
	static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag2, int>, my_wonky_tag2>::value, "");
	static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag2, int&>, my_wonky_tag2>::value, "");
	struct my_wonky_tag3 : ns::random_access_iterator_tag {};
	static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag3, int>, std::input_iterator_tag>::value, "");
	static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<my_wonky_tag3, int&>, std::random_access_iterator_tag>::value, "");
	static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<ns::input_iterator_tag, int>, std::input_iterator_tag>::value, "");
	static_assert(std::is_same<ns::detail::stl2_to_std_iterator_category<ns::input_iterator_tag, int&>, std::input_iterator_tag>::value, "");
} // namespace associated_type_test

namespace readable_test {
	struct A {
		int operator*() const;
		using value_type = int;
	};

	CONCEPT_ASSERT(!ranges::Readable<void>);
	CONCEPT_ASSERT(!ranges::Readable<void*>);
	CONCEPT_ASSERT(ranges::Readable<int*>);
	CONCEPT_ASSERT(ranges::Readable<const int*>);
	CONCEPT_ASSERT(ranges::Readable<A>);
	CONCEPT_ASSERT(ranges::same_as<ns::iter_value_t<A>,int>);

	struct MoveOnlyReadable {
		using value_type = std::unique_ptr<int>;
		value_type operator*() const;
	};

	CONCEPT_ASSERT(ranges::Readable<MoveOnlyReadable>);

	struct ArrayReadable {
		using value_type = int[2];
		value_type& operator*() const;
	};

	CONCEPT_ASSERT(ranges::Readable<ArrayReadable>);

	struct Abstract {
		virtual void foo() = 0;
	};
	struct AbstractReadable {
		using value_type = Abstract;
		Abstract& operator*() const;
	};

	CONCEPT_ASSERT(ranges::Readable<AbstractReadable>);
}

namespace writable_test {
	struct A {
		int& operator*() const;
	};

	CONCEPT_ASSERT(ranges::Writable<std::unique_ptr<int>*, std::unique_ptr<int>&&>);
	CONCEPT_ASSERT(!ranges::Writable<std::unique_ptr<int>*, std::unique_ptr<int>&>);
	CONCEPT_ASSERT(!ranges::Writable<void, int>);
	CONCEPT_ASSERT(!ranges::Writable<void*, void>);
	CONCEPT_ASSERT(ranges::Writable<int*, int>);
	CONCEPT_ASSERT(ranges::Writable<int*, int&>);
	CONCEPT_ASSERT(ranges::Writable<int*, const int&>);
	CONCEPT_ASSERT(ranges::Writable<int*, const int>);
	CONCEPT_ASSERT(!ranges::Writable<const int*, int>);
	CONCEPT_ASSERT(ranges::Writable<A, int>);
	CONCEPT_ASSERT(ranges::Writable<A, const int&>);
	CONCEPT_ASSERT(ranges::Writable<A, double>);
	CONCEPT_ASSERT(ranges::Writable<A, const double&>);
} // namespace writable_test

CONCEPT_ASSERT(ranges::WeaklyIncrementable<int>);
CONCEPT_ASSERT(ranges::WeaklyIncrementable<unsigned int>);
CONCEPT_ASSERT(!ranges::WeaklyIncrementable<void>);
CONCEPT_ASSERT(ranges::WeaklyIncrementable<int*>);
CONCEPT_ASSERT(ranges::WeaklyIncrementable<const int*>);

CONCEPT_ASSERT(ranges::Incrementable<int>);
CONCEPT_ASSERT(ranges::Incrementable<unsigned int>);
CONCEPT_ASSERT(!ranges::Incrementable<void>);
CONCEPT_ASSERT(ranges::Incrementable<int*>);
CONCEPT_ASSERT(ranges::Incrementable<const int*>);

namespace iterator_sentinel_test {
	struct A {
		using difference_type = signed char;
		using iterator_category = ns::input_iterator_tag;
		using value_type = double;

		A& operator++();
		A operator++(int);
		double operator*() const;

		bool operator == (const A&) const;
		bool operator != (const A&) const;
	};

	CONCEPT_ASSERT(ranges::Iterator<int*>);
	CONCEPT_ASSERT(ranges::Iterator<const int*>);
	CONCEPT_ASSERT(!ranges::Iterator<void*>);
	CONCEPT_ASSERT(ranges::Iterator<A>);
	CONCEPT_ASSERT(ranges::InputIterator<A>);

	CONCEPT_ASSERT(ranges::Iterator<int*>);
	CONCEPT_ASSERT(ranges::Sentinel<int*, int*>);
	CONCEPT_ASSERT(ranges::Sentinel<const int*, const int*>);
	CONCEPT_ASSERT(ranges::Sentinel<const int*, int*>);
	CONCEPT_ASSERT(!ranges::Sentinel<void*, void*>);
	CONCEPT_ASSERT(ranges::Sentinel<A, A>);
} // namespace iterator_sentinel_test

namespace indirectly_callable_test {
	CONCEPT_ASSERT(ranges::ext::IndirectInvocable<std::plus<int>, int*, int*>);
}

namespace indirect_invoke_result_test {
	template<class R, class... Args>
	using fn_t = R(Args...);
	CONCEPT_ASSERT(ranges::same_as<ns::indirect_result_t<fn_t<void, int>&, const int*>, void>);
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

	CONCEPT_ASSERT(ranges::ContiguousIterator<std::array<int, 42>::iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::array<int, 42>::const_iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::string::iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::string::const_iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::basic_string<char, std::char_traits<char>, allocator<char>>::iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::basic_string<char, std::char_traits<char>, allocator<char>>::const_iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::string_view::iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::string_view::const_iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<ranges::iterator_t<std::valarray<double>>>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<ranges::iterator_t<const std::valarray<double>>>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::vector<int>::iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::vector<int>::const_iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::vector<int, allocator<int>>::iterator>);
	CONCEPT_ASSERT(ranges::ContiguousIterator<std::vector<int, allocator<int>>::const_iterator>);
}

int main() {
	return ::test_result();
}
