#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/algorithm.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/view/repeat.hpp>
#include <initializer_list>
#include <iostream>
#include <memory>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

template <class> class show_type;

template <stl2::Destructible T>
class forward_list {
	struct node {
		std::unique_ptr<node> next_;
		T data_;

		template <class...Args>
		requires stl2::Constructible<T, Args...>()
		constexpr node(Args&&...args)
		noexcept(stl2::is_nothrow_constructible<T, Args...>::value)
		: data_(stl2::forward<Args>(args)...) {}

		node(node&&) = delete;
		node(const node&) = delete;
	};

	template <bool IsConst>
	class cursor {
	public:
		template <bool> friend class cursor;

		cursor() = default;
		template <bool B>
		requires IsConst && !B
		cursor(cursor<B> that) noexcept
		: ptr_{that.ptr_} {}

		struct mixin : protected stl2::basic_mixin<cursor> {
			mixin() = default;
			using stl2::basic_mixin<cursor>::basic_mixin;
			constexpr mixin(stl2::default_sentinel) noexcept
			: mixin{cursor{}}
			{}
		};

		constexpr meta::if_c<IsConst, const T, T>& read() const noexcept {
			return ptr_->data_;
		}
		constexpr void next() noexcept { ptr_ = ptr_->next_.get(); }
		constexpr bool equal(stl2::default_sentinel) const noexcept { return !ptr_; }
		constexpr bool equal(const cursor& that) const noexcept { return ptr_ == that.ptr_; }

	private:
		stl2::detail::raw_ptr<node> ptr_ = nullptr;

		friend forward_list;
		constexpr cursor(node* ptr) noexcept
		: ptr_{ptr} {}
	};

	std::unique_ptr<node> head_ = nullptr;

public:
	using value_type = T;
	using iterator = stl2::basic_iterator<cursor<false>>;
	using const_iterator = stl2::basic_iterator<cursor<true>>;

	forward_list() = default;
	forward_list(std::initializer_list<T> il)
	requires stl2::CopyConstructible<T>()
	{ stl2::reverse_copy(il, stl2::front_inserter(*this)); }

	constexpr iterator begin() noexcept {
		return {cursor<false>{head_.get()}};
	}
	constexpr const_iterator begin() const noexcept {
		return {cursor<true>{head_.get()}};
	}
	constexpr stl2::default_sentinel end() const noexcept { return {}; }

	template <class...Args>
	requires stl2::Constructible<T, Args...>()
	void emplace(Args&&...args) {
		auto p = std::make_unique<node>(stl2::forward<Args>(args)...);
		p->next_ = stl2::move(head_);
		head_ = stl2::move(p);
	}

	void push_front(T&& t)
	requires stl2::MoveConstructible<T>()
	{ emplace(stl2::move(t)); }
	void push_front(const T& t)
	requires stl2::CopyConstructible<T>()
	{ emplace(t); }
};

template <class T>
class pointer_cursor {
public:
	using contiguous = stl2::true_type;
	struct mixin : protected stl2::detail::ebo_box<pointer_cursor> {
		mixin() = default;
		using mixin::ebo_box::ebo_box;
	};

	pointer_cursor() = default;
	constexpr pointer_cursor(T* ptr) noexcept
	: ptr_{ptr} {}

	template <class U>
	requires stl2::ConvertibleTo<U*, T*>()
	constexpr pointer_cursor(const pointer_cursor<U>& that) noexcept
	: ptr_{that.ptr_} {}

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

template <stl2::Semiregular T, std::size_t N>
class array {
public:
	using value_type = T;
	using iterator = stl2::basic_iterator<pointer_cursor<T>>;
	using const_iterator = stl2::basic_iterator<pointer_cursor<const T>>;

	// These should be constexpr, except for PR67376.
	iterator begin() noexcept { return {elements_ + 0}; }
	iterator end() noexcept { return {elements_ + N}; }
	const_iterator begin() const noexcept { return {elements_ + 0}; }
	const_iterator end() const noexcept { return {elements_ + N}; }

	T elements_[N];
};

template <class T, T Value>
struct always_cursor {
	constexpr T read() const
		noexcept(stl2::is_nothrow_copy_constructible<T>::value) {
		return Value;
	}
	constexpr bool equal(always_cursor) const noexcept { return true; }
	constexpr void next() const noexcept {}
	constexpr void prev() const noexcept {}
	constexpr void advance(std::ptrdiff_t) const noexcept {}
	constexpr std::ptrdiff_t distance_to(always_cursor) const noexcept { return 0; }
};
template <class T, T Value>
using always_iterator = stl2::basic_iterator<always_cursor<T, Value>>;

template <class T>
requires stl2::is_object<T>::value
struct proxy_wrapper {
	stl2::detail::raw_ptr<T> ptr_ = nullptr;

	proxy_wrapper() = default;
	proxy_wrapper(T& t) noexcept : ptr_{stl2::addressof(t)} {}
	proxy_wrapper(T&&) = delete;

	T& get() const noexcept { return *ptr_; }

	proxy_wrapper& operator=(const T& t)
	noexcept(std::is_nothrow_copy_assignable<T>::value)
	{
		get() = t;
		return *this;
	}

	proxy_wrapper& operator=(T&& t)
	noexcept(std::is_nothrow_move_assignable<T>::value)
	{
		get() = stl2::move(t);
		return *this;
	}

	operator T&() const noexcept { return get(); }
};

STL2_OPEN_NAMESPACE {
	template <class T, class U, template <class> class TQual,
		template <class> class UQual>
	struct basic_common_reference<::proxy_wrapper<T>, U, TQual, UQual>
	: common_reference<T&, UQual<U>> {};

	template <class T, class U, template <class> class TQual,
		template <class> class UQual>
	struct basic_common_reference<T, ::proxy_wrapper<U>, TQual, UQual>
	: common_reference<TQual<T>, U&> {};

	template <class T, class U, template <class> class TQual,
		template <class> class UQual>
	struct basic_common_reference<
		::proxy_wrapper<T>, ::proxy_wrapper<U>, TQual, UQual>
	: common_reference<T&, U&> {};
} STL2_CLOSE_NAMESPACE

// std::array-ish container with proxy iterators.
template <class T, std::size_t N>
struct proxy_array {
	template <bool IsConst>
	struct cursor {
		using value_type = T;

		using O = meta::if_c<IsConst, const T, T>;
		O* ptr_;

		cursor(O* p = nullptr) : ptr_{p} {}
		template <bool B>
		requires IsConst && !B
		cursor(const cursor<B>& that) : ptr_{that.ptr_} {}

		template <bool B>
		bool equal(const cursor<B>& that) const noexcept { return ptr_ == that.ptr_; }
		proxy_wrapper<O> read() const noexcept { return {*ptr_}; }
		void next() noexcept { ++ptr_; }
		void prev() noexcept { --ptr_; }
		void advance(std::ptrdiff_t n) noexcept { ptr_ += n; }
		template <bool B>
		std::ptrdiff_t distance_to(const cursor<B>& that) const noexcept {
			return that.ptr_ - ptr_;
		}
		O&& indirect_move() const noexcept { return stl2::move(*ptr_); }
	};

	static_assert(stl2::cursor::Readable<cursor<false>>());
	static_assert(stl2::cursor::IndirectMove<cursor<false>>());
	static_assert(stl2::models::Same<T&&, stl2::cursor::rvalue_reference_t<cursor<false>>>);

	static_assert(stl2::cursor::Readable<cursor<true>>());
	static_assert(stl2::cursor::IndirectMove<cursor<true>>());
	static_assert(stl2::models::Same<const T&&, stl2::cursor::rvalue_reference_t<cursor<true>>>);

	using iterator = stl2::basic_iterator<cursor<false>>;
	using const_iterator = stl2::basic_iterator<cursor<true>>;
	using reference = proxy_wrapper<T>;
	using const_reference = proxy_wrapper<const T>;

	T e_[N];

	iterator begin() { return {&e_[0]}; }
	iterator end() { return {&e_[0] + N}; }

	const_iterator begin() const { return {&e_[0]}; }
	const_iterator end() const { return {&e_[0] + N}; }
	const_iterator cbegin() const { return begin(); }
	const_iterator cend() const { return end(); }

	reference operator[](const std::size_t n) noexcept {
		return {e_[n]};
	}
	const_reference operator[](const std::size_t n) const noexcept {
		return {e_[n]};
	}
};

template <stl2::InputRange R>
requires
	stl2::ext::StreamInsertable<stl2::value_type_t<stl2::iterator_t<R>>> &&
	!stl2::Same<char, stl2::remove_cv_t<
		stl2::remove_all_extents_t<stl2::remove_reference_t<R>>>>()
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

template <stl2::Iterator I>
class output_cursor {
	class post_increment_proxy {
	public:
		post_increment_proxy(output_cursor& self) noexcept
		: self_(self) {}

		post_increment_proxy& operator*() noexcept {
			return *this;
		}

		template <class T>
		requires stl2::OutputIterator<I, const T&>()
		post_increment_proxy& operator=(const T &t)
		{
			self_.write(t);
			return *this;
		}

		output_cursor& self_;
	};

public:
	struct mixin : protected stl2::detail::ebo_box<output_cursor> {
		mixin() = default;
		mixin(const post_increment_proxy &p)
		: mixin(p.self_) {}
		using mixin::ebo_box::ebo_box;
	};

	output_cursor() = default;
	constexpr output_cursor(I i) noexcept
	: i_{i} {}

	void next() noexcept {
	}

	post_increment_proxy post_increment() noexcept {
		return post_increment_proxy{*this};
	}

	template <class T>
	requires stl2::OutputIterator<I, const T&>()
	constexpr void write(const T& t) noexcept {
		*i_++ = t;
	}

private:
	I i_;
};
template <stl2::Iterator I>
using output_iterator = stl2::basic_iterator<output_cursor<I>>;

void test_fl() {
	std::cout << "test_fl:\n";
	::forward_list<int> list = {0, 1, 2, 3};
	using Rng = decltype(list);
	using I = decltype(list.begin());
	using S = decltype(list.end());
	static_assert(stl2::models::WeaklyIncrementable<I>);
	static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
	static_assert(stl2::models::Readable<I>);
	static_assert(stl2::models::Same<stl2::value_type_t<I>, int>);
	static_assert(stl2::models::Same<stl2::reference_t<I>, int&>);
	static_assert(stl2::models::Same<stl2::rvalue_reference_t<I>, int&&>);
	static_assert(stl2::models::Copyable<I>);
	static_assert(stl2::models::DefaultConstructible<I>);
	static_assert(stl2::models::Semiregular<I>);
	static_assert(stl2::models::Incrementable<I>);
	static_assert(stl2::models::EqualityComparable<I>);
	static_assert(stl2::models::ForwardIterator<I>);
	static_assert(stl2::models::Sentinel<S, I>);
	static_assert(stl2::models::ForwardRange<Rng>);
	static_assert(stl2::models::Common<S, I>);
	static_assert(stl2::models::Same<stl2::common_type_t<S, I>, I>);
	static_assert(stl2::models::Same<
		stl2::ext::range<I, I>,
		decltype(stl2::ext::make_bounded_range(list.begin(), list.end()))>);
	std::cout << list << '\n';
	*stl2::front_inserter(list) = 3.14;
	std::cout << list << '\n';
	CHECK(list.begin() != list.end());
	CHECK(noexcept(list.begin() != list.end()));
	CHECK(!(list.begin() == list.end()));
	CHECK(noexcept(list.begin() == list.end()));
	CHECK(list.begin() != stl2::next(list.begin()));
	CHECK(!(list.begin() == stl2::next(list.begin())));
	static_assert(I{} == I{});
	CHECK(noexcept(I{} == I{}));
	CHECK(noexcept(stl2::front_inserter(list)));
	CHECK(sizeof(I) == sizeof(void*));
	CHECK(stl2::begin(list) == stl2::cbegin(list));
	{ auto i = stl2::cbegin(list); i = stl2::begin(list); }
	{ auto i = stl2::begin(list); i = stl2::end(list); }
	{ auto i = stl2::cbegin(list); i = stl2::end(list); }
}

void test_rv() {
	std::cout << "test_rv:\n";
	stl2::ext::repeat_view<int> rv{42};

	using Rng = decltype(rv);
	using I = decltype(rv.begin());
	static_assert(stl2::models::WeaklyIncrementable<I>);
	static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
	static_assert(stl2::models::Readable<I>);
	static_assert(stl2::models::Same<stl2::value_type_t<I>, int>);
	static_assert(stl2::models::Same<stl2::reference_t<I>, int>);
	static_assert(stl2::models::Same<stl2::rvalue_reference_t<I>, int>);
	static_assert(stl2::models::RandomAccessIterator<I>);
	static_assert(stl2::models::RandomAccessRange<Rng>);
	CHECK(I{} == I{});

	auto i = rv.begin();
	for (auto n = 0; i != rv.end() && n < 13; ++i, ++n) {
		std::cout << *i << ' ';
	}
	std::cout << '\n';

	stl2::copy_n(rv.begin(), 13, stl2::ostream_iterator<>{std::cout, " "});
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
	static_assert(stl2::models::WeaklyIncrementable<I>);
	static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
	static_assert(stl2::models::Readable<I>);
	static_assert(stl2::models::Same<stl2::value_type_t<I>, int>);
	static_assert(stl2::models::Same<stl2::reference_t<I>, int&>);
	static_assert(stl2::models::Same<stl2::rvalue_reference_t<I>, int&&>);
	static_assert(stl2::models::ContiguousIterator<I>);
	static_assert(stl2::models::RandomAccessRange<Rng>);
	static_assert(stl2::models::RandomAccessRange<const Rng>);
	static_assert(stl2::models::BoundedRange<Rng>);
	static_assert(stl2::models::BoundedRange<const Rng>);
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
		auto first = stl2::begin(a);
		auto last = stl2::cend(a) - 1;
		CHECK(!(first == last));
		CHECK((first != last));
		CHECK((first < last));
		CHECK(!(first > last));
		CHECK((first <= last));
		CHECK(!(first >= last));
	}

	stl2::fill(a, 42);
}

void test_counted() {
	using stl2::counted_iterator;
	std::cout << "test_counted:\n";
	int some_ints[] = {0,1,2,3};
	using I = counted_iterator<const int*>;
	static_assert(stl2::models::WeaklyIncrementable<I>);
	static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
	static_assert(stl2::models::Readable<I>);
	static_assert(stl2::models::Same<stl2::value_type_t<I>, int>);
	static_assert(stl2::models::Same<stl2::reference_t<I>, const int&>);
	static_assert(stl2::models::Same<stl2::rvalue_reference_t<I>, const int&&>);
	static_assert(stl2::models::RandomAccessIterator<I>);
	static_assert(stl2::models::ContiguousIterator<I>);
	CHECK(I{} == I{});
	auto first = I{some_ints, 4};
	CHECK(first.base() == some_ints);
	CHECK(first.count() == 4);
	auto last = I{some_ints + 4, 0};
	CHECK(last.base() == some_ints + 4);
	CHECK(last.count() == 0);
	static_assert(stl2::models::SizedSentinel<I, I>);
	CHECK((last - first) == 4);
	static_assert(stl2::models::SizedSentinel<stl2::default_sentinel, I>);
	CHECK((stl2::default_sentinel{} - first) == 4);
	auto out = stl2::ostream_iterator<>{std::cout, " "};
	stl2::copy(first, last, out);
	std::cout << '\n';
	stl2::copy(first, stl2::default_sentinel{}, out);
	std::cout << '\n';

	auto second = first + 1;
	CHECK(*first == 0);
	CHECK(*second == 1);
	CHECK(*++first == 1);
	CHECK(*first-- == 1);
	CHECK(*first == 0);

	{
		auto one = counted_iterator<const int*>{some_ints + 1, stl2::size(some_ints) - 1};
		auto three = counted_iterator<int*>{some_ints + 3, stl2::size(some_ints) - 3};
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
	static_assert(stl2::is_empty<I>());
	static_assert(sizeof(I) == 1);
	static_assert(stl2::models::WeaklyIncrementable<I>);
	static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
	static_assert(stl2::models::Readable<I>);
	static_assert(stl2::models::Same<stl2::value_type_t<I>, int>);
	static_assert(stl2::models::Same<stl2::reference_t<I>, int>);
	static_assert(stl2::models::Same<stl2::rvalue_reference_t<I>, int>);
	static_assert(stl2::models::RandomAccessIterator<I>);
	CHECK(I{} == I{});
	stl2::copy_n(i, 13, stl2::ostream_iterator<>{std::cout, " "});
	std::cout << '\n';
	CHECK(*i == 42);
	CHECK(*(i + 42) == 42);
	CHECK(*(i - 42) == 42);
	CHECK(i[42] == 42);
}

void test_back_inserter() {
	std::cout << "test_back_inserter:\n";
	auto vec = std::vector<int>{};
	auto i = stl2::back_inserter(vec);
	using I = decltype(i);
	static_assert(stl2::models::WeaklyIncrementable<I>);
	static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
	static_assert(stl2::models::Same<stl2::reference_t<I>, I&>);
	static_assert(!stl2::models::Readable<I>);
	static_assert(stl2::models::OutputIterator<I, int>);
	static_assert(!stl2::models::InputIterator<I>);
	static_assert(!stl2::models::EqualityComparable<I>);
	stl2::copy_n(always_iterator<int, 42>{}, 13, i);
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

	using I = stl2::iterator_t<Rng>;
	static_assert(stl2::models::WeaklyIncrementable<I>);
	static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
	static_assert(stl2::models::Readable<I>);
	using R = stl2::reference_t<I>;
	static_assert(stl2::models::Same<proxy_wrapper<int>, R>);
	using V = stl2::value_type_t<I>;
	static_assert(stl2::models::Same<int, V>);
	static_assert(stl2::models::Same<int&&, decltype(iter_move(stl2::declval<const I&>()))>);
	static_assert(stl2::models::Same<int&&, decltype(iter_move(stl2::declval<I&>()))>);

	static_assert(stl2::__iter_move::has_customization<const I&>);
	static_assert(stl2::__iter_move::has_customization<I&>);
	using RR = stl2::rvalue_reference_t<I>;
	static_assert(stl2::models::Same<int&&, RR>);
	static_assert(stl2::models::RandomAccessIterator<I>);
	static_assert(!stl2::models::ContiguousIterator<I>);
	static_assert(stl2::models::RandomAccessRange<Rng>);
	static_assert(stl2::models::BoundedRange<Rng>);

	using CI = stl2::iterator_t<const Rng>;
	static_assert(stl2::models::WeaklyIncrementable<CI>);
	static_assert(stl2::models::Same<stl2::difference_type_t<CI>, std::ptrdiff_t>);
	using CR = stl2::reference_t<CI>;
	static_assert(stl2::models::Same<proxy_wrapper<const int>, CR>);
	using CV = stl2::value_type_t<CI>;
	static_assert(stl2::models::Same<int, CV>);

	static_assert(stl2::__iter_move::has_customization<const CI&>);
	static_assert(stl2::__iter_move::has_customization<CI&>);
	using CRR = stl2::rvalue_reference_t<CI>;
	static_assert(stl2::models::Same<const int&&, CRR>);

	static_assert(stl2::models::CommonReference<CR, CV&>);
	static_assert(stl2::models::CommonReference<CR, CRR>);
	static_assert(stl2::models::CommonReference<CRR, const CV&>);
	static_assert(stl2::models::Readable<CI>);
	static_assert(stl2::models::Same<const int&&, decltype(iter_move(stl2::declval<const CI&>()))>);
	static_assert(stl2::models::Same<const int&&, decltype(iter_move(stl2::declval<CI&>()))>);

	static_assert(stl2::models::RandomAccessIterator<CI>);
	static_assert(!stl2::models::ContiguousIterator<CI>);
	static_assert(stl2::models::RandomAccessRange<const Rng>);
	static_assert(stl2::models::BoundedRange<const Rng>);

	static_assert(stl2::models::Same<I, decltype(a.begin() + 2)>);
	static_assert(stl2::models::CommonReference<const R&, const R&>);
	static_assert(!stl2::models::Swappable<R, R>);
	static_assert(stl2::models::IndirectlyMovableStorable<I, I>);

	// Swappable<R, R>() is not satisfied, and
	// IndirectlyMovableStorable<I, I>() is satisfied,
	// so this should resolve to the second overload of iter_swap.
	stl2::iter_swap(a.begin() + 1, a.begin() + 3);
	CHECK(a[0] == 0);
	CHECK(a[1] == 3);
	CHECK(a[2] == 2);
	CHECK(a[3] == 1);
}

void test_output() {
	std::cout << "test_output:\n";

	auto vec = std::vector<int>{0,0,0};
	auto wi = vec.begin();

	using I = ::output_iterator<decltype(wi)>;
	static_assert(stl2::models::WeaklyIncrementable<I>);
	static_assert(!stl2::models::Incrementable<I>);
	static_assert(!stl2::models::Decrementable<I>);
	static_assert(!stl2::models::Readable<I>);
	static_assert(stl2::models::Writable<I, int>);
	static_assert(stl2::models::DefaultConstructible<I>);
	static_assert(stl2::models::Copyable<I>);
	static_assert(stl2::models::Semiregular<I>);
	static_assert(stl2::models::Iterator<I>);
	static_assert(!stl2::models::InputIterator<I>);
	static_assert(stl2::models::OutputIterator<I, int>);

	I i{wi};
	*i++ = 1;
	*i++ = 2;
	I i2 = i++;
	*i2 = 3;
	CHECK(vec[0] == 1);
	CHECK(vec[1] == 2);
	CHECK(vec[2] == 3);
}

int main() {
	test_rv();
	test_fl();
	test_array();
	test_counted();
	test_always();
	test_back_inserter();
	test_proxy_array();
	test_output();
	return ::test_result();
}
