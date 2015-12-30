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
      noexcept(stl2::is_nothrow_constructible<T, Args...>::value) :
      data_(stl2::forward<Args>(args)...) {}

    node(node&&) = delete;
    node(const node&) = delete;
  };

  class cursor {
  public:
    cursor() = default;

    struct mixin : protected stl2::basic_mixin<cursor> {
      mixin() = default;
      using stl2::basic_mixin<cursor>::basic_mixin;
      constexpr mixin(stl2::default_sentinel) noexcept
      : mixin{cursor{nullptr}}
      {}
    };

    constexpr T& read() const noexcept { return ptr_->data_; }
    constexpr void next() noexcept { ptr_ = ptr_->next_.get(); }
    constexpr bool equal(stl2::default_sentinel) const noexcept { return !ptr_; }
    constexpr bool equal(const cursor& that) const noexcept { return ptr_ == that.ptr_; }

  private:
    stl2::detail::raw_ptr<node> ptr_;

    friend forward_list;
    constexpr cursor(node* ptr) noexcept :
      ptr_{ptr} {}
  };

  std::unique_ptr<node> head_ = nullptr;

public:
  using value_type = T;
  using iterator = stl2::basic_iterator<cursor>;

  forward_list() = default;
  forward_list(std::initializer_list<T> il)
    requires stl2::CopyConstructible<T>() {
    stl2::copy(stl2::rbegin(il), stl2::rend(il), stl2::front_inserter(*this));
  }

  constexpr iterator begin() noexcept { return {cursor{head_.get()}}; }
  constexpr stl2::default_sentinel end() const noexcept { return {}; }

  template <class...Args>
    requires stl2::Constructible<T, Args...>()
  void emplace(Args&&...args) {
    auto p = std::make_unique<node>(stl2::forward<Args>(args)...);
    p->next_ = stl2::move(head_);
    head_ = stl2::move(p);
  }

  void push_front(T&& t)
    requires stl2::MoveConstructible<T>() {
    emplace(stl2::move(t));
  }
  void push_front(const T& t)
    requires stl2::CopyConstructible<T>() {
    emplace(t);
  }
};

template <class T>
  requires stl2::Same<T, void>() || stl2::ext::StreamInsertable<T>
class ostream_cursor {
public:
  struct mixin : protected stl2::detail::ebo_box<ostream_cursor> {
    mixin() = default;
    using mixin::ebo_box::ebo_box;
  };

  constexpr ostream_cursor() noexcept = default;
  constexpr ostream_cursor(std::ostream& os, const char* delimiter = nullptr)
  noexcept
  : os_{&os}, delimiter_{delimiter}
  {}

  template <class U = T>
  requires stl2::Same<T, U>()
  void write(const U& t) {
    *os_ << t;
    delimit();
  }

  stl2::ext::StreamInsertable{U}
  void write(const U& u)
  requires stl2::Same<T, void>()
  {
    *os_ << u;
    delimit();
  }

private:
  stl2::detail::raw_ptr<std::ostream> os_ = nullptr;
  const char* delimiter_ = nullptr;

  void delimit() const {
    if (delimiter_) {
      *os_ << delimiter_;
    }
  }
};

template <class T = void>
using ostream_iterator = stl2::basic_iterator<ostream_cursor<T>>;

template <class T>
class pointer_cursor {
public:
  using contiguous = stl2::true_type;
  struct mixin : protected stl2::detail::ebo_box<pointer_cursor> {
    mixin() = default;
    using mixin::ebo_box::ebo_box;
  };

  pointer_cursor() = default;
  constexpr pointer_cursor(T* ptr) noexcept :
    ptr_{ptr} {}

  template <class U>
    requires stl2::ConvertibleTo<U*, T*>()
  constexpr pointer_cursor(const pointer_cursor<U>& that) noexcept :
    ptr_{that.ptr_} {}

  constexpr T& read() const noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    return *ptr_;
  }

  constexpr T* arrow() const noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    return ptr_;
  }

  constexpr void next() noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    ++ptr_;
  }

  constexpr bool equal(const pointer_cursor& that) const noexcept {
    return ptr_ == that.ptr_;
  }

  constexpr void prev() noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    --ptr_;
  }

  constexpr void advance(std::ptrdiff_t n) noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    ptr_ += n;
  }

  constexpr std::ptrdiff_t distance_to(const pointer_cursor& that) const noexcept {
    STL2_ASSUME_CONSTEXPR(!that.ptr_ == !ptr_);
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

stl2::Iterator{I}
class my_counted_cursor {
public:
  using difference_type = stl2::difference_type_t<I>;

private:
  I it_;
  difference_type n_;

public:
  struct mixin : protected stl2::detail::ebo_box<my_counted_cursor> {
    mixin() = default;
    using mixin::ebo_box::ebo_box;

    constexpr I base() const
    noexcept(stl2::is_nothrow_copy_constructible<I>::value)
    {
      return mixin::ebo_box::get().it_;
    }
    constexpr difference_type count() const noexcept {
      return mixin::ebo_box::get().n_;
    }
  };

  my_counted_cursor() = default;

  constexpr my_counted_cursor(I&& it, difference_type n)
  noexcept(stl2::is_nothrow_move_constructible<I>::value)
  : it_(stl2::move(it)), n_{n} {}

  constexpr my_counted_cursor(const I& it, difference_type n)
  noexcept(stl2::is_nothrow_copy_constructible<I>::value)
  : it_(it), n_{n} {}

  constexpr my_counted_cursor(stl2::default_sentinel)
  noexcept(stl2::is_nothrow_default_constructible<I>::value)
  requires !stl2::models::BidirectionalIterator<I>
  : it_{}, n_{0} {}

  constexpr void next()
  noexcept(noexcept(++it_))
  {
    ++it_;
    --n_;
  }

  template <class T>
  requires stl2::Writable<I, T&&>()
  constexpr auto write(T&& t)
  STL2_NOEXCEPT_RETURN(
    *it_ = stl2::forward<T>(t)
  )

  constexpr decltype(auto) read() const
  noexcept(noexcept(*it_))
  requires stl2::InputIterator<I>()
  {
    return *it_;
  }

  constexpr auto equal(stl2::default_sentinel) const noexcept {
    return n_ == 0;
  }
  constexpr auto equal(const my_counted_cursor& that) const noexcept {
    return n_ == that.n_;
  }

  constexpr auto prev()
  noexcept(noexcept(--it_))
  requires stl2::BidirectionalIterator<I>()
  {
    ++n_;
    --it_;
  }

  constexpr auto advance(difference_type n)
  noexcept(noexcept(it_ += n))
  requires stl2::RandomAccessIterator<I>()
  {
    it_ += n;
    n_ -= n;
  }

  constexpr auto distance_to(const my_counted_cursor& that) const noexcept {
    return n_ - that.n_;
  }
  constexpr auto distance_to(stl2::default_sentinel) const noexcept {
    return n_;
  }

  // FIXME: test
  constexpr decltype(auto) move() const
  STL2_NOEXCEPT_RETURN(
    stl2::iter_move(it_)
  )
};

stl2::Iterator{I}
using my_counted_iterator = stl2::basic_iterator<my_counted_cursor<I>>;

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

template <stl2::InputRange R>
  requires stl2::ext::StreamInsertable<stl2::value_type_t<stl2::iterator_t<R>>> &&
    !stl2::Same<char, stl2::remove_cv_t<stl2::remove_all_extents_t<stl2::remove_reference_t<R>>>>()
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
}

void test_rv() {
  stl2::repeat_view<int> rv{42};

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

  stl2::copy_n(rv.begin(), 13, ::ostream_iterator<>{std::cout, " "});
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

  stl2::fill(a, 42);
}

void test_counted() {
  int some_ints[] = {0,1,2,3};
  using I = my_counted_iterator<const int*>;
  static_assert(stl2::models::WeaklyIncrementable<I>);
  static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
  static_assert(stl2::models::Readable<I>);
  static_assert(stl2::models::Same<stl2::value_type_t<I>, int>);
  static_assert(stl2::models::Same<stl2::reference_t<I>, const int&>);
  static_assert(stl2::models::Same<stl2::rvalue_reference_t<I>, const int&&>);
  static_assert(stl2::models::RandomAccessIterator<I>);
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
  auto out = ::ostream_iterator<>{std::cout, " "};
  stl2::copy(first, last, out);
  std::cout << '\n';
  stl2::copy(first, stl2::default_sentinel{}, out);
  std::cout << '\n';
}

void test_always() {
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
  stl2::copy_n(i, 13, ::ostream_iterator<>{std::cout, " "});
  std::cout << '\n';
  CHECK(*i == 42);
  CHECK(*(i + 42) == 42);
  CHECK(*(i - 42) == 42);
  CHECK(i[42] == 42);
}

void test_back_inserter() {
  auto vec = std::vector<int>{};
  auto i = stl2::back_inserter(vec);
  using I = decltype(i);
  static_assert(stl2::models::WeaklyIncrementable<I>);
  static_assert(stl2::models::Same<stl2::difference_type_t<I>, std::ptrdiff_t>);
  // FIXME: static_assert(!stl2::models::Readable<I>);
  // FIXME: static_assert(stl2::models::Same<stl2::reference_t<I>, I&>);
  static_assert(stl2::models::OutputIterator<I, int>);
  // FIXME: static_assert(!stl2::models::InputIterator<I>);
  static_assert(!stl2::models::EqualityComparable<I>);
  stl2::copy_n(always_iterator<int, 42>{}, 13, i);
  CHECK(vec.size() == 13u);
}

int main() {
  test_rv();
  test_fl();
  test_array();
  test_counted();
  test_always();
  test_back_inserter();
  return ::test_result();
}
