#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/algorithm.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/view/repeat.hpp>
#include <initializer_list>
#include <iostream>
#include <memory>
#include "../simple_test.hpp"

namespace ranges = __stl2;

template<class> class show_type;

template<ranges::destructible T>
class forward_list {
	struct node {
		std::unique_ptr<node> next_;
		T data_;

		template<class... Args>
		requires ranges::constructible_from<T, Args...>
		constexpr node(Args&&... args)
		noexcept(std::is_nothrow_constructible<T, Args...>::value)
		: data_(std::forward<Args>(args)...) {}

		node(node&&) = delete;
		node(const node&) = delete;
	};

	template<bool IsConst>
	class cursor {
	public:
		template<bool> friend class cursor;

		cursor() = default;
		template<bool B>
		requires (IsConst && !B)
		cursor(cursor<B> that) noexcept
		: ptr_{that.ptr_} {}

		struct mixin : protected ranges::basic_mixin<cursor> {
			mixin() = default;
			using ranges::basic_mixin<cursor>::basic_mixin;
			constexpr mixin(ranges::default_sentinel) noexcept
			: mixin{cursor{}}
			{}
		};

		constexpr meta::if_c<IsConst, const T, T>& read() const noexcept {
			return ptr_->data_;
		}
		constexpr void next() noexcept { ptr_ = ptr_->next_.get(); }
		constexpr bool equal(ranges::default_sentinel) const noexcept { return !ptr_; }
		constexpr bool equal(const cursor& that) const noexcept { return ptr_ == that.ptr_; }

	private:
		ranges::detail::raw_ptr<node> ptr_ = nullptr;

		friend forward_list;
		constexpr cursor(node* ptr) noexcept
		: ptr_{ptr} {}
	};

	std::unique_ptr<node> head_ = nullptr;

public:
	using value_type = T;
	using iterator = ranges::basic_iterator<cursor<false>>;
	using const_iterator = ranges::basic_iterator<cursor<true>>;

	forward_list() = default;
	forward_list(std::initializer_list<T> il)
	requires ranges::copy_constructible<T>
	{ ranges::reverse_copy(il, ranges::front_inserter(*this)); }

	constexpr iterator begin() noexcept {
		return iterator{cursor<false>{head_.get()}};
	}
	constexpr const_iterator begin() const noexcept {
		return const_iterator{cursor<true>{head_.get()}};
	}
	constexpr ranges::default_sentinel end() const noexcept { return {}; }

	template<class... Args>
	requires ranges::constructible_from<T, Args...>
	void emplace(Args&&... args) {
		auto p = std::make_unique<node>(std::forward<Args>(args)...);
		p->next_ = std::move(head_);
		head_ = std::move(p);
	}

	void push_front(T&& t)
	requires ranges::move_constructible<T>
	{ emplace(std::move(t)); }
	void push_front(const T& t)
	requires ranges::copy_constructible<T>
	{ emplace(t); }
};

template<class T>
class pointer_cursor {
public:
	using contiguous = std::true_type;
	class mixin : protected ranges::basic_mixin<pointer_cursor> {
		using base_t = ranges::basic_mixin<pointer_cursor>;
	public:
		mixin() = default;
		constexpr mixin(T* ptr) noexcept
		: base_t{pointer_cursor{ptr}}
		{}
		using base_t::base_t;
	};

	pointer_cursor() = default;
	constexpr pointer_cursor(T* ptr) noexcept
	: ptr_{ptr} {}

	template<class U>
	requires ranges::convertible_to<U*, T*>
	constexpr pointer_cursor(const pointer_cursor<U>& that) noexcept
	: ptr_{that.arrow()} {}

	constexpr T& read() const noexcept {
		STL2_EXPECT(ptr_);
		return *ptr_;
	}

	constexpr T* arrow() const noexcept {
		STL2_EXPECT(ptr_);
		return ptr_;
	}

	constexpr void next() noexcept {
		STL2_EXPECT(ptr_);
		++ptr_;
	}

	constexpr bool equal(const pointer_cursor& that) const noexcept {
		return ptr_ == that.ptr_;
	}

	constexpr void prev() noexcept {
		STL2_EXPECT(ptr_);
		--ptr_;
	}

	constexpr void advance(std::ptrdiff_t n) noexcept {
		STL2_EXPECT(ptr_);
		ptr_ += n;
	}

	constexpr std::ptrdiff_t distance_to(const pointer_cursor& that) const noexcept {
		STL2_EXPECT(!that.ptr_ == !ptr_);
		return that.ptr_ - ptr_;
	}

private:
	T* ptr_;
};

template<ranges::semiregular T, std::size_t N>
class array {
public:
	using value_type = T;
	using iterator = ranges::basic_iterator<pointer_cursor<T>>;
	using const_iterator = ranges::basic_iterator<pointer_cursor<const T>>;

	// These should be constexpr, except for PR67376.
	iterator begin() noexcept { return {elements_ + 0}; }
	iterator end() noexcept { return {elements_ + N}; }
	const_iterator begin() const noexcept { return {elements_ + 0}; }
	const_iterator end() const noexcept { return {elements_ + N}; }

	T elements_[N];
};

template<class T, T Value>
struct always_cursor {
	constexpr T read() const
		noexcept(std::is_nothrow_copy_constructible<T>::value) {
		return Value;
	}
	constexpr bool equal(always_cursor) const noexcept { return true; }
	constexpr void next() const noexcept {}
	constexpr void prev() const noexcept {}
	constexpr void advance(std::ptrdiff_t) const noexcept {}
	constexpr std::ptrdiff_t distance_to(always_cursor) const noexcept { return 0; }
};
template<class T, T Value>
using always_iterator = ranges::basic_iterator<always_cursor<T, Value>>;

template<ranges::ext::Object T>
struct proxy_wrapper {
	ranges::detail::raw_ptr<T> ptr_ = nullptr;

	proxy_wrapper() = default;
	proxy_wrapper(T& t) noexcept : ptr_{std::addressof(t)} {}
	proxy_wrapper(T&&) = delete; // LWG 2993?

	T& get() const noexcept { return *ptr_; }

	proxy_wrapper& operator=(const T& t)
	noexcept(std::is_nothrow_copy_assignable<T>::value)
	requires ranges::copy_constructible<T>
	{
		get() = t;
		return *this;
	}

	proxy_wrapper& operator=(T&& t)
	noexcept(std::is_nothrow_move_assignable<T>::value)
	requires ranges::move_constructible<T>
	{
		get() = std::move(t);
		return *this;
	}

	proxy_wrapper const& operator=(const T& t) const
	noexcept(std::is_nothrow_copy_assignable<T>::value)
	requires ranges::copyable<T>
	{
		get() = t;
		return *this;
	}

	proxy_wrapper const& operator=(T&& t) const
	noexcept(std::is_nothrow_move_assignable<T>::value)
	requires ranges::movable<T>
	{
		get() = std::move(t);
		return *this;
	}

	operator T&() const noexcept { return get(); }
};

STL2_OPEN_NAMESPACE {
	template<class T, class U, template<class> class TQual,
		template<class> class UQual>
	struct basic_common_reference<::proxy_wrapper<T>, U, TQual, UQual>
	: common_reference<T&, UQual<U>> {};

	template<class T, class U, template<class> class TQual,
		template<class> class UQual>
	struct basic_common_reference<T, ::proxy_wrapper<U>, TQual, UQual>
	: common_reference<TQual<T>, U&> {};

	template<class T, class U, template<class> class TQual,
		template<class> class UQual>
	struct basic_common_reference<
		::proxy_wrapper<T>, ::proxy_wrapper<U>, TQual, UQual>
	: common_reference<T&, U&> {};
} STL2_CLOSE_NAMESPACE

// std::array-ish container with proxy iterators.
template<class T, std::size_t N>
struct proxy_array {
	template<bool IsConst>
	struct cursor {
		using value_type = T;

		using O = meta::if_c<IsConst, const T, T>;
		O* ptr_;

		cursor(O* p = nullptr) : ptr_{p} {}
		template<bool B>
		requires (IsConst && !B)
		cursor(const cursor<B>& that) : ptr_{that.ptr_} {}

		template<bool B>
		bool equal(const cursor<B>& that) const noexcept { return ptr_ == that.ptr_; }
		proxy_wrapper<O> read() const noexcept { return {*ptr_}; }
		void next() noexcept { ++ptr_; }
		void prev() noexcept { --ptr_; }
		void advance(std::ptrdiff_t n) noexcept { ptr_ += n; }
		template<bool B>
		std::ptrdiff_t distance_to(const cursor<B>& that) const noexcept {
			return that.ptr_ - ptr_;
		}
		O&& indirect_move() const noexcept { return std::move(*ptr_); }
	};

	static_assert(ranges::cursor::Readable<cursor<false>>);
	static_assert(ranges::cursor::IndirectMove<cursor<false>>);
	static_assert(ranges::same_as<T&&, ranges::cursor::rvalue_reference_t<cursor<false>>>);

	static_assert(ranges::cursor::Readable<cursor<true>>);
	static_assert(ranges::cursor::IndirectMove<cursor<true>>);
	static_assert(ranges::same_as<const T&&, ranges::cursor::rvalue_reference_t<cursor<true>>>);

	using iterator = ranges::basic_iterator<cursor<false>>;
	using const_iterator = ranges::basic_iterator<cursor<true>>;
	using reference = proxy_wrapper<T>;
	using const_reference = proxy_wrapper<const T>;

	T e_[N];

	auto begin() { return iterator{&e_[0]}; }
	auto end() { return iterator{&e_[0] + N}; }

	auto begin() const { return const_iterator{&e_[0]}; }
	auto end() const { return const_iterator{&e_[0] + N}; }
	auto cbegin() const { return begin(); }
	auto cend() const { return end(); }

	reference operator[](const std::size_t n) noexcept {
		return {e_[n]};
	}
	const_reference operator[](const std::size_t n) const noexcept {
		return {e_[n]};
	}
};

template<ranges::InputRange R>
requires
	(ranges::StreamInsertable<ranges::iter_value_t<ranges::iterator_t<R>>> &&
	 !ranges::same_as<char, std::remove_cv_t<
		std::remove_all_extents_t<std::remove_reference_t<R>>>>)
std::ostream& operator<<(std::ostream& os, R&& rng) {
	os << '{';
	auto i = rng.begin();
	auto e = rng.end();
	if (i != e) {
		for (;;) {
			os << *i;
			if (++i == e) break;
			os << ", ";
		}
	}
	os << '}';
	return os;
}

void test_fl() {
	std::cout << "test_fl:\n";
	::forward_list<int> list = {0, 1, 2, 3};
	using Rng = decltype(list);
	using I = decltype(list.begin());
	using S = decltype(list.end());
	static_assert(ranges::WeaklyIncrementable<I>);
	static_assert(ranges::same_as<ranges::iter_difference_t<I>, std::ptrdiff_t>);
	static_assert(ranges::Readable<I>);
	static_assert(ranges::same_as<ranges::iter_value_t<I>, int>);
	static_assert(ranges::same_as<ranges::iter_reference_t<I>, int&>);
	static_assert(ranges::same_as<ranges::iter_rvalue_reference_t<I>, int&&>);
	static_assert(ranges::copyable<I>);
	static_assert(ranges::default_initializable<I>);
	static_assert(ranges::semiregular<I>);
	static_assert(ranges::Incrementable<I>);
	static_assert(ranges::equality_comparable<I>);
	static_assert(ranges::ForwardIterator<I>);
	static_assert(ranges::Sentinel<S, I>);
	static_assert(ranges::ForwardRange<Rng>);
	static_assert(ranges::common_with<S, I>);
	static_assert(ranges::same_as<ranges::common_type_t<S, I>, I>);
	std::cout << list << '\n';
	*ranges::front_inserter(list) = 3.14;
	std::cout << list << '\n';
	CHECK(list.begin() != list.end());
	CHECK(noexcept(list.begin() != list.end()));
	CHECK(!(list.begin() == list.end()));
	CHECK(noexcept(list.begin() == list.end()));
	CHECK(list.begin() != ranges::next(list.begin()));
	CHECK(!(list.begin() == ranges::next(list.begin())));
	static_assert(I{} == I{});
	CHECK(noexcept(I{} == I{}));
	CHECK(noexcept(ranges::front_inserter(list)));
	CHECK(sizeof(I) == sizeof(void*));
	CHECK(ranges::begin(list) == ranges::cbegin(list));
	{ auto i = ranges::cbegin(list); i = ranges::begin(list); }
	{ auto i = ranges::begin(list); i = ranges::end(list); }
	{ auto i = ranges::cbegin(list); i = ranges::end(list); }
}

void test_rv() {
	std::cout << "test_rv:\n";
	ranges::ext::repeat_view rv{42};

	using Rng = decltype(rv);
	using I = decltype(rv.begin());
	static_assert(ranges::WeaklyIncrementable<I>);
	static_assert(ranges::same_as<ranges::iter_difference_t<I>, std::ptrdiff_t>);
	static_assert(ranges::Readable<I>);
	static_assert(ranges::same_as<ranges::iter_value_t<I>, int>);
	static_assert(ranges::same_as<ranges::iter_reference_t<I>, int&>);
	static_assert(ranges::same_as<ranges::iter_rvalue_reference_t<I>, int&&>);
	static_assert(ranges::RandomAccessIterator<I>);
	static_assert(ranges::RandomAccessRange<Rng>);
	CHECK(I{} == I{});

	auto i = rv.begin();
	for (auto n = 0; i != rv.end() && n < 13; ++i, ++n) {
		std::cout << *i << ' ';
	}
	std::cout << '\n';

	ranges::copy_n(rv.begin(), 13, ranges::ostream_iterator<>{std::cout, " "});
	std::cout << '\n';
	CHECK(rv.begin() != rv.end());
	CHECK(!(rv.begin() == rv.end()));

	CHECK(rv.begin() == rv.begin());
	CHECK(!(rv.begin() != rv.begin()));
	CHECK(rv.begin() == rv.begin() + 1);
	CHECK(rv.begin() == rv.begin() - 1);
	CHECK(!(rv.begin() < rv.begin()));
	CHECK(!(rv.begin() > rv.begin()));
	CHECK(rv.begin() <= rv.begin());
	CHECK(rv.begin() >= rv.begin());
}

void test_array() {
	std::cout << "test_array:\n";

	::array<int, 13> a;

	using Rng = decltype(a);
	using I = decltype(a.begin());
	CHECK(noexcept(a.begin()));
	static_assert(ranges::WeaklyIncrementable<I>);
	static_assert(ranges::same_as<ranges::iter_difference_t<I>, std::ptrdiff_t>);
	static_assert(ranges::Readable<I>);
	static_assert(ranges::same_as<ranges::iter_value_t<I>, int>);
	static_assert(ranges::same_as<ranges::iter_reference_t<I>, int&>);
	static_assert(ranges::same_as<ranges::iter_rvalue_reference_t<I>, int&&>);
	static_assert(ranges::ContiguousIterator<I>);
	static_assert(ranges::RandomAccessRange<Rng>);
	static_assert(ranges::RandomAccessRange<const Rng>);
	static_assert(ranges::CommonRange<Rng>);
	static_assert(ranges::CommonRange<const Rng>);
	CHECK(I{} == I{});
	CHECK(noexcept(I{} == I{}));

	{
		auto first = a.begin();
		auto last = a.end() - 1;
		CHECK(!(first == last));
		CHECK((first != last));
		CHECK((first < last));
		CHECK(!(first > last));
		CHECK((first <= last));
		CHECK(!(first >= last));
	}
	{
		auto first = ranges::begin(a);
		auto last = ranges::cend(a) - 1;
		CHECK(!(first == last));
		CHECK((first != last));
		CHECK((first < last));
		CHECK(!(first > last));
		CHECK((first <= last));
		CHECK(!(first >= last));
	}

	ranges::fill(a, 42);
}

void test_counted() {
	using ranges::counted_iterator;
	std::cout << "test_counted:\n";
	int some_ints[] = {0,1,2,3};
	using I = counted_iterator<const int*>;
	static_assert(ranges::WeaklyIncrementable<I>);
	static_assert(ranges::same_as<ranges::iter_difference_t<I>, std::ptrdiff_t>);
	static_assert(ranges::Readable<I>);
	static_assert(ranges::same_as<ranges::iter_value_t<I>, int>);
	static_assert(ranges::same_as<ranges::iter_reference_t<I>, const int&>);
	static_assert(ranges::same_as<ranges::iter_rvalue_reference_t<I>, const int&&>);
	static_assert(ranges::RandomAccessIterator<I>);
	static_assert(ranges::ContiguousIterator<I>);
	CHECK(I{} == I{});
	auto first = I{some_ints, 4};
	CHECK(first.base() == some_ints);
	CHECK(first.count() == 4);
	auto last = I{some_ints + 4, 0};
	CHECK(last.base() == some_ints + 4);
	CHECK(last.count() == 0);
	static_assert(ranges::SizedSentinel<I, I>);
	CHECK((last - first) == 4);
	static_assert(ranges::SizedSentinel<ranges::default_sentinel, I>);
	CHECK((ranges::default_sentinel{} - first) == 4);
	auto out = ranges::ostream_iterator<>{std::cout, " "};
	ranges::copy(first, last, out);
	std::cout << '\n';
	ranges::copy(first, ranges::default_sentinel{}, out);
	std::cout << '\n';

	auto second = first + 1;
	CHECK(*first == 0);
	CHECK(*second == 1);
	CHECK(*++first == 1);
	CHECK(*first-- == 1);
	CHECK(*first == 0);

	{
		auto one = counted_iterator<const int*>{some_ints + 1, ranges::distance(some_ints) - 1};
		auto three = counted_iterator<int*>{some_ints + 3, ranges::distance(some_ints) - 3};
		CHECK(!(one == three));
		CHECK((one != three));
		CHECK((one < three));
		CHECK(!(one > three));
		CHECK((one <= three));
		CHECK(!(one >= three));
	}
 }

void test_always() {
	std::cout << "test_always:\n";
	// Iterates over life, the universe, and everything.
	auto i = always_iterator<int, 42>{};
	using I = decltype(i);
	static_assert(std::is_empty<I>());
	static_assert(sizeof(I) == 1);
	static_assert(ranges::WeaklyIncrementable<I>);
	static_assert(ranges::same_as<ranges::iter_difference_t<I>, std::ptrdiff_t>);
	static_assert(ranges::Readable<I>);
	static_assert(ranges::same_as<ranges::iter_value_t<I>, int>);
	static_assert(ranges::same_as<ranges::iter_reference_t<I>, int>);
	static_assert(ranges::same_as<ranges::iter_rvalue_reference_t<I>, int>);
	static_assert(ranges::RandomAccessIterator<I>);
	CHECK(I{} == I{});
	ranges::copy_n(i, 13, ranges::ostream_iterator<>{std::cout, " "});
	std::cout << '\n';
	CHECK(*i == 42);
	CHECK(*(i + 42) == 42);
	CHECK(*(i - 42) == 42);
	CHECK(i[42] == 42);
}

void test_back_inserter() {
	std::cout << "test_back_inserter:\n";
	auto vec = std::vector<int>{};
	auto i = ranges::back_inserter(vec);
	using I = decltype(i);
	static_assert(ranges::WeaklyIncrementable<I>);
	static_assert(ranges::same_as<ranges::iter_difference_t<I>, std::ptrdiff_t>);
	static_assert(ranges::same_as<ranges::iter_reference_t<I>, I&>);
	static_assert(!ranges::Readable<I>);
	static_assert(ranges::OutputIterator<I, int>);
	static_assert(!ranges::InputIterator<I>);
	static_assert(!ranges::equality_comparable<I>);
	ranges::copy_n(always_iterator<int, 42>{}, 13, i);
	CHECK(vec.size() == 13u);
}

void test_proxy_array() {
	std::cout << "test_proxy_array:\n";

	auto a = proxy_array<int, 4>{0,1,2,3};

	{
		auto ci = a.cbegin();
		CHECK((ci == a.begin()));
		ci = a.begin();
	}

	using Rng = decltype(a);

	using I = ranges::iterator_t<Rng>;
	static_assert(ranges::WeaklyIncrementable<I>);
	static_assert(ranges::same_as<ranges::iter_difference_t<I>, std::ptrdiff_t>);
	static_assert(ranges::Readable<I>);
	using R = ranges::iter_reference_t<I>;
	static_assert(ranges::same_as<proxy_wrapper<int>, R>);
	using V = ranges::iter_value_t<I>;
	static_assert(ranges::same_as<int, V>);
	static_assert(ranges::same_as<int&&, decltype(iter_move(std::declval<const I&>()))>);
	static_assert(ranges::same_as<int&&, decltype(iter_move(std::declval<I&>()))>);

	static_assert(ranges::__iter_move::has_customization<const I&>);
	static_assert(ranges::__iter_move::has_customization<I&>);
	using RR = ranges::iter_rvalue_reference_t<I>;
	static_assert(ranges::same_as<int&&, RR>);
	static_assert(ranges::RandomAccessIterator<I>);
	static_assert(!ranges::ContiguousIterator<I>);
	static_assert(ranges::RandomAccessRange<Rng>);
	static_assert(ranges::CommonRange<Rng>);

	using CI = ranges::iterator_t<const Rng>;
	static_assert(ranges::WeaklyIncrementable<CI>);
	static_assert(ranges::same_as<ranges::iter_difference_t<CI>, std::ptrdiff_t>);
	using CR = ranges::iter_reference_t<CI>;
	static_assert(ranges::same_as<proxy_wrapper<const int>, CR>);
	using CV = ranges::iter_value_t<CI>;
	static_assert(ranges::same_as<int, CV>);

	static_assert(ranges::__iter_move::has_customization<const CI&>);
	static_assert(ranges::__iter_move::has_customization<CI&>);
	using CRR = ranges::iter_rvalue_reference_t<CI>;
	static_assert(ranges::same_as<const int&&, CRR>);

	static_assert(ranges::common_reference_with<CR, CV&>);
	static_assert(ranges::common_reference_with<CR, CRR>);
	static_assert(ranges::common_reference_with<CRR, const CV&>);
	static_assert(ranges::Readable<CI>);
	static_assert(ranges::same_as<const int&&, decltype(iter_move(std::declval<const CI&>()))>);
	static_assert(ranges::same_as<const int&&, decltype(iter_move(std::declval<CI&>()))>);

	static_assert(ranges::RandomAccessIterator<CI>);
	static_assert(!ranges::ContiguousIterator<CI>);
	static_assert(ranges::RandomAccessRange<const Rng>);
	static_assert(ranges::CommonRange<const Rng>);

	static_assert(ranges::same_as<I, decltype(a.begin() + 2)>);
	static_assert(ranges::common_reference_with<const R&, const R&>);
	static_assert(!ranges::swappable_with<R, R>);
	static_assert(ranges::IndirectlyMovableStorable<I, I>);

	// swappable<R, R> is not satisfied, and
	// IndirectlyMovableStorable<I, I> is satisfied,
	// so this should resolve to the second overload of iter_swap.
	ranges::iter_swap(a.begin() + 1, a.begin() + 3);
	CHECK(a[0] == 0);
	CHECK(a[1] == 3);
	CHECK(a[2] == 2);
	CHECK(a[3] == 1);
}

int main() {
	test_rv();
	test_fl();
	test_array();
	test_counted();
	test_always();
	test_back_inserter();
	test_proxy_array();
	return ::test_result();
}
