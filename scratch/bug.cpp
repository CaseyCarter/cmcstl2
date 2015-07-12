template <class T>
concept bool C =
  requires (T& a, T& b) {
    a - b;
  };

template <class>
constexpr bool f() { return false; }
template <C>
constexpr bool f() { return true; }

struct A;

static_assert(f<int>(), "");
static_assert(f<char*>(), "");
static_assert(!f<void>(), "");

// Hard error when T is a pointer to incomplete type.
static_assert(!f<A*>(), "");
static_assert(!f<void*>(), "");
// Not exactly a bug?
