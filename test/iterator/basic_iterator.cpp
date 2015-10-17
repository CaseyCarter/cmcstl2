#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/algorithm.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <initializer_list>
#include <iostream>
#include <memory>
#include "../simple_test.hpp"

namespace stl2 = __stl2;

template <class> class show_type;

template <class Container>
class front_insert_cursor {
public:
  front_insert_cursor() = default;
  constexpr front_insert_cursor(Container& c) noexcept :
    c_{&c} {}

private:
  stl2::detail::raw_ptr<Container> c_;

  friend stl2::cursor_access;
  template <class T>
    requires requires(Container& c, T&& t) {
      c.push_front(stl2::forward<T>(t));
    }
  void write(T&& t) {
    c_->push_front(stl2::forward<T>(t));
  }
};
template <class Container>
using front_insert_iterator = stl2::basic_iterator<front_insert_cursor<Container>>;

template <class Container>
front_insert_iterator<Container> front_inserter(Container& c) {
  return {c};
}

template <stl2::Semiregular T>
class repeat_view : stl2::view_base, stl2::detail::ebo_box<T> {
  using storage_t = stl2::detail::ebo_box<T>;

  class cursor : stl2::detail::cheap_reference_box_t<const T> {
    using storage_t = stl2::detail::cheap_reference_box_t<const T>;
  public:
    using difference_type = std::ptrdiff_t;
    using reference = meta::if_c<
      stl2::detail::cheaply_copyable<T>, T, const T&>;

    cursor() = default;
    constexpr cursor(const repeat_view& r)
      noexcept(stl2::is_nothrow_constructible<storage_t, const T&>::value) :
      storage_t{r.value()} {}

  private:
    friend stl2::cursor_access;

    constexpr reference current() const
      noexcept(noexcept(stl2::declval<const storage_t&>().get())) {
      return storage_t::get();
    }

    constexpr void next() const noexcept {}
    constexpr void prev() const noexcept {}

    constexpr void advance(difference_type) const noexcept {}
    constexpr bool equal(const cursor&) const noexcept { return true; }

    constexpr difference_type distance_to(const cursor&) const noexcept { return 0; }
  };

public:
  using iterator = stl2::basic_iterator<cursor>;
  using value_type = T;

  repeat_view() = default;
  repeat_view(T value)
    noexcept(stl2::is_nothrow_constructible<storage_t, T>::value) :
    storage_t{stl2::move(value)} {}

  constexpr iterator begin() const
    noexcept(noexcept(iterator{cursor{stl2::declval<const repeat_view&>()}})) {
    return iterator{cursor{*this}};
  }

  constexpr stl2::unreachable end() const noexcept {
    return {};
  }

  constexpr const T& value() const noexcept {
    return storage_t::get();
  }
};

template <stl2::Destructible T>
class forward_list {
  struct node {
    std::unique_ptr<node> next_;
    T data_;

    template <class...Args>
      requires stl2::Constructible<T, Args...>()
    node(Args&&...args)
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
    T& current() const noexcept { return ptr_->data_; }
    void next() noexcept { ptr_ = ptr_->next_.get(); }
    bool equal(const cursor& that) const noexcept { return ptr_ == that.ptr_; }
  };

  std::unique_ptr<node> head_ = nullptr;

public:
  using value_type = T;
  using iterator = stl2::basic_iterator<cursor>;

  forward_list() = default;
  forward_list(std::initializer_list<T> il)
    requires stl2::CopyConstructible<T>() {
    stl2::copy(stl2::rbegin(il), stl2::rend(il), ::front_inserter(*this));
  }

  iterator begin() noexcept { return {head_.get()}; }
  stl2::default_sentinel end() const noexcept { return {}; }

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
  ostream_cursor_base(std::ostream& os, const char* delimiter = nullptr) :
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
  auto& current() const noexcept {
    STL2_ASSUME(ptr_);
    return *ptr_;
  }

  auto next() noexcept {
    STL2_ASSUME(ptr_);
    ++ptr_;
  }

  auto equal(const pointer_cursor& that) const noexcept {
    return ptr_ == that.ptr_;
  }

  auto prev() noexcept {
    STL2_ASSUME(ptr_);
    --ptr_;
  }

  auto advance(std::ptrdiff_t n) noexcept {
    STL2_ASSUME(ptr_);
    ptr_ += n;
  }

  auto distance_to(const pointer_cursor& that) const noexcept {
    STL2_ASSUME(that.ptr_);
    STL2_ASSUME(ptr_);
    return that.ptr_ - ptr_;
  }
};

template <stl2::Semiregular T, std::size_t N>
class array {
  T elements_[N];

public:
  using value_type = T;
  using iterator = stl2::basic_iterator<pointer_cursor<T>>;
  using const_iterator = stl2::basic_iterator<pointer_cursor<const T>>;

  iterator begin() { return {elements_ + 0}; }
  iterator end() { return {elements_ + N}; }
  const_iterator begin() const { return {elements_ + 0}; }
  const_iterator end() const { return {elements_ + N}; }
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

  constexpr I base() const { return it_; }
  constexpr difference_type count() const { return n_; }

private:
  I it_;
  difference_type n_;

  friend stl2::cursor_access;
  constexpr void next() {
    ++it_;
    --n_;
  }
  template <class T>
    requires stl2::Writable<I, T>()
  constexpr auto write(T&& t) {
    *it_ = stl2::forward<T>(t);
  }
  constexpr decltype(auto) current() const
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
    requires stl2::BidirectionalIterator<I>() {
    ++n_;
    --it_;
  }
  constexpr auto advance(difference_type n)
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
public:
  int foo() const { return 42; }

private:
  friend stl2::cursor_access;
  constexpr T current() const { return Value; }
  constexpr bool equal(always_cursor) const { return true; }
  constexpr void next() const {}
  constexpr void prev() const {}
  constexpr void advance(std::ptrdiff_t) const {}
  constexpr std::ptrdiff_t distance_to(always_cursor) const { return 0; }
};
template <class T, T Value>
using always_iterator = stl2::basic_iterator<always_cursor<T, Value>>;

template <class Container>
class back_insert_cursor {
public:
  using container_type = Container;

  constexpr back_insert_cursor() noexcept = default;
  constexpr explicit back_insert_cursor(Container& c) noexcept :
    container{&c} {}

protected:
  Container* container = nullptr;

private:
  friend stl2::cursor_access;
  void write(typename Container::value_type&& v) {
    container->push_back(stl2::move(v));
  }
  void write(const typename Container::value_type& v) {
    container->push_back(v);
  }
};
template <class Container>
using back_insert_iterator = stl2::basic_iterator<back_insert_cursor<Container>>;
template <class Container>
auto back_inserter(Container& c) {
  return back_insert_iterator<Container>{c};
}

std::ostream& operator<<(std::ostream& os, forward_list<int>& lst) {
  os << '{';
  auto i = lst.begin();
  if (i != lst.end()) {
    for (;;) {
      os << *i;
      if (++i == lst.end()) break;
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
  *::front_inserter(list) = 3.14;
  std::cout << list << '\n';
  CHECK(list.begin() != list.end());
  CHECK(!(list.begin() == list.end()));
  CHECK(list.begin() != stl2::next(list.begin()));
  CHECK(!(list.begin() == stl2::next(list.begin())));
  CHECK(I{} == I{});
}

void test_rv() {
  repeat_view<int> rv{42};

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
  static_assert(stl2::models::RandomAccessIterator<I>);
  static_assert(stl2::models::RandomAccessRange<R>);
  static_assert(stl2::models::RandomAccessRange<const R>);
  CHECK(I{} == I{});

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
  // Iterates over life, the universe, and everything:
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
  auto i = ::back_inserter(vec);
  using I = decltype(i);
  static_assert(stl2::models::WeakOutputIterator<I, int>);
  static_assert(stl2::models::Same<std::ptrdiff_t, stl2::DifferenceType<I>>);
  stl2::copy_n(always_iterator<int, 42>{}, 13, i);
  CHECK(vec.size() == 13u);

  struct foo : ::back_insert_iterator<std::vector<int>> {
    using base_t = ::back_insert_iterator<std::vector<int>>;
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
