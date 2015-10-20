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
    constexpr cursor(node* ptr) noexcept :
      ptr_{ptr} {}
    constexpr cursor(stl2::default_sentinel) noexcept :
      cursor{nullptr} {}

  private:
    stl2::detail::raw_ptr<node> ptr_;

    friend stl2::cursor_access;
    constexpr T& current() const noexcept { return ptr_->data_; }
    constexpr void next() noexcept { ptr_ = ptr_->next_.get(); }
    constexpr bool equal(const cursor& that) const noexcept { return ptr_ == that.ptr_; }
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

  constexpr iterator begin() noexcept { return {head_.get()}; }
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

class ostream_cursor_base {
public:
  ostream_cursor_base() = default;
  constexpr ostream_cursor_base(std::ostream& os, const char* delimiter = nullptr)
    noexcept :
    os_{&os}, delimiter_{delimiter} {}

protected:
  stl2::detail::raw_ptr<std::ostream> os_;
  const char* delimiter_ = nullptr;

  void delimit() const {
    if (delimiter_) {
      *os_ << delimiter_;
    }
  }
};

template <class T>
  requires stl2::Same<T, void>() || stl2::ext::StreamInsertable<T>
class ostream_cursor : public ostream_cursor_base {
public:
  using ostream_cursor_base::ostream_cursor_base;

private:
  friend stl2::cursor_access;
  void write(const T& t) {
    *os_ << t;
    delimit();
  }
};
template <>
class ostream_cursor<void> : public ostream_cursor_base {
public:
  using ostream_cursor_base::ostream_cursor_base;

private:
  friend stl2::cursor_access;
  stl2::ext::StreamInsertable{T}
  void write(const T& t) {
    *os_ << t;
    delimit();
  }
};
template <class T = void>
using ostream_iterator = stl2::basic_iterator<ostream_cursor<T>>;

template <class T>
class pointer_cursor {
public:
  pointer_cursor() = default;
  constexpr pointer_cursor(T* ptr) noexcept :
    ptr_{ptr} {}

  template <class U>
    requires stl2::ConvertibleTo<U*, T*>()
  constexpr pointer_cursor(const pointer_cursor<U>& that) noexcept :
    ptr_{that.ptr_} {}

private:
  T* ptr_;

  friend stl2::cursor_access;
  using is_contiguous = stl2::true_type;
  constexpr auto& current() const noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    return *ptr_;
  }

  constexpr auto next() noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    ++ptr_;
  }

  constexpr auto equal(const pointer_cursor& that) const noexcept {
    return ptr_ == that.ptr_;
  }

  constexpr auto prev() noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    --ptr_;
  }

  constexpr auto advance(std::ptrdiff_t n) noexcept {
    STL2_ASSUME_CONSTEXPR(ptr_);
    ptr_ += n;
  }

  constexpr auto distance_to(const pointer_cursor& that) const noexcept {
    STL2_ASSUME_CONSTEXPR(!that.ptr_ == !ptr_);
    return that.ptr_ - ptr_;
  }
};

template <stl2::Semiregular T, std::size_t N>
class array {
public:
  using value_type = T;
  using iterator = stl2::basic_iterator<pointer_cursor<T>>;
  using const_iterator = stl2::basic_iterator<pointer_cursor<const T>>;

  constexpr iterator begin() noexcept { return {elements_ + 0}; }
  constexpr iterator end() noexcept { return {elements_ + N}; }
  constexpr const_iterator begin() const noexcept { return {elements_ + 0}; }
  constexpr const_iterator end() const noexcept { return {elements_ + N}; }

  T elements_[N];
};

stl2::WeakIterator{I}
class my_counted_cursor {
public:
  using difference_type = stl2::DifferenceType<I>;

  my_counted_cursor() = default;
  constexpr my_counted_cursor(I it, difference_type n)
    noexcept(stl2::is_nothrow_move_constructible<I>::value) :
    it_(stl2::move(it)), n_{n} {}
  constexpr my_counted_cursor(stl2::default_sentinel)
    noexcept(stl2::is_nothrow_default_constructible<I>::value) :
    it_{}, n_{0} {}

  constexpr I base() const
    noexcept(stl2::is_nothrow_copy_constructible<I>::value) {
    return it_;
  }
  constexpr difference_type count() const noexcept { return n_; }

private:
  I it_;
  difference_type n_;

  friend stl2::cursor_access;
  constexpr void next()
    noexcept(noexcept(++stl2::declval<I&>())) {
    ++it_;
    --n_;
  }

  template <class T>
    requires stl2::Writable<I, T>()
  constexpr auto write(T&& t)
    noexcept(noexcept(*stl2::declval<I&>() = stl2::forward<T>(t))) {
    *it_ = stl2::forward<T>(t);
  }

  constexpr decltype(auto) current() const
    noexcept(noexcept(*stl2::declval<const I&>()))
    requires stl2::WeakInputIterator<I>() {
    return *it_;
  }
  constexpr auto equal(stl2::default_sentinel) const noexcept {
    return n_ == 0;
  }
  constexpr auto equal(const my_counted_cursor& that) const noexcept {
    return n_ == that.n_;
  }
  constexpr auto prev()
    noexcept(noexcept(--stl2::declval<I&>()))
    requires stl2::BidirectionalIterator<I>() {
    ++n_;
    --it_;
  }
  constexpr auto advance(difference_type n)
    noexcept(noexcept(stl2::declval<I&>() += n))
    requires stl2::RandomAccessIterator<I>() {
    it_ += n;
    n_ -= n;
  }
  constexpr auto distance_to(const my_counted_cursor& that) const noexcept {
    return n_ - that.n_;
  }
  constexpr auto distance_to(stl2::default_sentinel) const noexcept {
    return n_;
  }
};

stl2::WeakIterator{I}
using my_counted_iterator = stl2::basic_iterator<my_counted_cursor<I>>;

template <class T, T Value>
class always_cursor {
  friend stl2::cursor_access;
  constexpr T current() const
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
  requires stl2::ext::StreamInsertable<stl2::ValueType<stl2::IteratorType<R>>> &&
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
  std::cout << list << '\n';
  using R = decltype(list);
  using I = decltype(list.begin());
  static_assert(stl2::models::Copyable<I>);
  static_assert(stl2::models::DefaultConstructible<I>);
  static_assert(stl2::models::Semiregular<I>);
  static_assert(stl2::models::Incrementable<I>);
  static_assert(stl2::models::EqualityComparable<I>);
  static_assert(stl2::models::ForwardIterator<I>);
  static_assert(stl2::models::ForwardRange<R>);
  *stl2::front_inserter(list) = 3.14;
  std::cout << list << '\n';
  CHECK(list.begin() != list.end());
  CHECK(noexcept(list.begin() != list.end()));
  CHECK(!(list.begin() == list.end()));
  CHECK(noexcept(list.begin() == list.end()));
  CHECK(list.begin() != stl2::next(list.begin()));
  CHECK(noexcept(list.begin() != stl2::next(list.begin())));
  CHECK(!(list.begin() == stl2::next(list.begin())));
  CHECK(noexcept(list.begin() == stl2::next(list.begin())));
  static_assert(I{} == I{});
  CHECK(noexcept(I{} == I{}));
  CHECK(noexcept(stl2::front_inserter(list)));
}

void test_rv() {
  stl2::repeat_view<int> rv{42};

  using R = decltype(rv);
  using I = decltype(rv.begin());
  static_assert(stl2::models::RandomAccessIterator<I>);
  static_assert(stl2::models::RandomAccessRange<R>);
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
  CHECK(rv.begin() < rv.end());
  CHECK(!(rv.begin() > rv.end()));
  CHECK(rv.begin() <= rv.end());
  CHECK(!(rv.begin() >= rv.end()));

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

  using R = decltype(a);
  using I = decltype(a.begin());
  CHECK(noexcept(a.begin()));
  static_assert(stl2::models::ContiguousIterator<I>);
  static_assert(stl2::models::RandomAccessRange<R>);
  static_assert(stl2::models::RandomAccessRange<const R>);
  static_assert(stl2::models::BoundedRange<R>);
  static_assert(stl2::models::BoundedRange<const R>);
  CHECK(I{} == I{});
  CHECK(noexcept(I{} == I{}));

  stl2::fill(a, 42);
}

void test_counted() {
  int some_ints[] = {0,1,2,3};
  using I = my_counted_iterator<const int*>;
  static_assert(stl2::models::RandomAccessIterator<I>);
  CHECK(I{} == I{});
  auto first = I{some_ints, 4};
  CHECK(first.base() == some_ints);
  CHECK(first.count() == 4);
  auto last = I{some_ints + 4, 0};
  CHECK(last.base() == some_ints + 4);
  CHECK(last.count() == 0);
  static_assert(stl2::models::SizedIteratorRange<I, I>);
  CHECK((last - first) == 4);
  static_assert(stl2::models::SizedIteratorRange<I, stl2::default_sentinel>);
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
  static_assert(stl2::models::RandomAccessIterator<I>);
  CHECK(I{} == I{});
  stl2::copy_n(i, 13, ::ostream_iterator<>{std::cout, " "});
  std::cout << '\n';
  static_assert(stl2::is_empty<I>());
  static_assert(sizeof(I) == 1);
  CHECK(*i == 42);
  CHECK(*(i + 42) == 42);
  CHECK(*(i - 42) == 42);
  CHECK(i[42] == 42);
}

void test_back_inserter() {
  auto vec = std::vector<int>{};
  auto i = stl2::back_inserter(vec);
  using I = decltype(i);
  static_assert(stl2::models::WeakOutputIterator<I, int>);
  static_assert(stl2::models::Same<std::ptrdiff_t, stl2::DifferenceType<I>>);
  stl2::copy_n(always_iterator<int, 42>{}, 13, i);
  CHECK(vec.size() == 13u);

  struct foo : stl2::back_insert_iterator<std::vector<int>> {
    using base_t = stl2::back_insert_iterator<std::vector<int>>;
    using base_t::base_t;

    typename base_t::container_type* container() {
      return base_t::container;
    }

    base_t& base() & { return *this; }
  };
  auto j = foo{vec};
  CHECK(j.container() == &vec);
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
