// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2014-2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_BASIC_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_BASIC_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/operator_arrow.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>

// TODO:
// * Think long and hard about the various proxies and reference
//   validity requirements.
// * Determine if the code complexity incurred by not having
//   basic_sentinel in the design is actually enabling anything useful.

STL2_OPEN_NAMESPACE {
  template <Destructible T>
  class basic_mixin : protected detail::ebo_box<T> {
    using box_t = detail::ebo_box<T>;
  public:
    constexpr basic_mixin()
    noexcept(is_nothrow_default_constructible<T>::value)
    requires DefaultConstructible<T>()
    : box_t{}
    {}
    constexpr basic_mixin(const T& t)
    noexcept(is_nothrow_copy_constructible<T>::value)
    requires CopyConstructible<T>()
    : box_t(t)
    {}
    constexpr basic_mixin(T&& t)
    noexcept(is_nothrow_move_constructible<T>::value)
    requires MoveConstructible<T>()
    : box_t(__stl2::move(t))
    {}
  };

  namespace cursor {
    class access {
      template <class T>
      struct mixin_base {
        using type = basic_mixin<T>;
      };
      template <class T>
      requires requires { typename T::mixin; }
      struct mixin_base<T> {
        using type = typename T::mixin;
      };

    public:
      template <class>
      struct reference_type {};
      template <class C>
      requires
        requires(const C& c) { STL2_DEDUCE_AUTO_REF_REF(c.read()); }
      struct reference_type<C> {
        using type = decltype(declval<const C&>().read());
      };
      template <class C>
      using reference_t = meta::_t<reference_type<C>>;

      // Not a bool variable template due to GCC PR68666.
      template <class>
      struct single_pass : false_type {};
      template <class C>
      requires
        requires {
          typename C::single_pass;
          requires bool(C::single_pass::value);
        }
      struct single_pass<C> : true_type {};

      // Not a bool variable template due to GCC PR68666.
      template <class> struct contiguous : false_type {};
      template <class C>
      requires
        requires {
          typename C::contiguous;
          requires bool(C::contiguous::value);
          requires is_reference<reference_t<C>>::value;
        }
      struct contiguous<C> : true_type {};

      template <class T>
      using mixin_t = meta::_t<mixin_base<T>>;

      template <class>
      struct difference_type {
        using type = std::ptrdiff_t;
      };
      template <detail::MemberDifferenceType C>
      struct difference_type<C> {
        using type = typename C::difference_type;
        static_assert(models::SignedIntegral<type>,
          "Cursor's member difference_type is not a signed integer type.");
      };
      template <class C>
      requires
        !detail::MemberDifferenceType<C> &&
        requires(const C& lhs, const C& rhs) { rhs.distance_to(lhs); }
      struct difference_type<C> {
        using type = decltype(declval<const C&>().distance_to(declval<const C&>()));
        static_assert(models::SignedIntegral<type>,
          "Return type of Cursor's member distance_to is not a signed integer type.");
      };
      template <class C>
      requires
        SignedIntegral<meta::_t<difference_type<C>>>()
      using difference_type_t = meta::_t<difference_type<C>>;

      template <class>
      struct value_type {};
      template <detail::MemberValueType C>
      struct value_type<C> {
        using type = typename C::value_type;
        static_assert(detail::IsValueType<type>,
          "Cursor's member value_type is not a value type.");
      };
      template <class C>
      requires
        !detail::MemberValueType<C> &&
        requires { typename reference_t<C>; }
      struct value_type<C> {
        using type = decay_t<reference_t<C>>;
        static_assert(detail::IsValueType<type>,
          "Cursor's reference_t does not decay to a value type.");
      };
      template <class C>
      requires
        detail::IsValueType<meta::_t<value_type<C>>>
      using value_type_t = meta::_t<value_type<C>>;

      template <class C>
      requires
        requires(const C& c) { c.read(); }
      static constexpr reference_t<C> read(const C& c)
      STL2_NOEXCEPT_RETURN(c.read())

      template <class C>
      requires
        requires(const C& c) { c.arrow(); }
      static constexpr decltype(auto) arrow(const C& c)
      STL2_NOEXCEPT_RETURN(c.arrow())

      template <class C, class T>
      requires
        requires(C& c, T&& t) { c.write(static_cast<T&&>(t)); }
      static constexpr void write(C& c, T&& t)
      STL2_NOEXCEPT_RETURN((void)c.write(static_cast<T&&>(t)))

      template <class C>
      requires
        requires(C& c) { c.next(); }
      static constexpr void next(C& c)
      STL2_NOEXCEPT_RETURN((void)c.next())

      template <class C>
      requires
        requires(C& c) { c.prev(); }
      static constexpr void prev(C& c)
      STL2_NOEXCEPT_RETURN((void)c.prev())

      template <class C, class Other>
      requires
        requires(const C& lhs, const Other& rhs) {
          { lhs.equal(rhs) } -> bool;
        }
      static constexpr bool equal(const C& lhs, const Other& rhs)
      STL2_NOEXCEPT_RETURN(static_cast<bool>(lhs.equal(rhs)))

      template <class C>
      requires
        requires(C& c, difference_type_t<C> n) { c.advance(n); }
      static constexpr void advance(C& c, difference_type_t<C> n)
      STL2_NOEXCEPT_RETURN((void)c.advance(n))

      template <class C, class Other>
      requires
        requires(const C& lhs, const Other& rhs) {
          STL2_EXACT_TYPE_CONSTRAINT(lhs.distance_to(rhs), difference_type_t<C>);
        }
      static constexpr difference_type_t<C>
      distance(const C& lhs, const Other& rhs)
      STL2_NOEXCEPT_RETURN(lhs.distance_to(rhs))

      template <class C>
      requires
        requires(const C& c) { c.imove(); }
      static constexpr decltype(auto) imove(const C& c)
      STL2_NOEXCEPT_RETURN(c.imove())

      template <class> struct rvalue_reference {};
      template <class C>
      requires
        requires(const C& c) { access::read(c); }
      struct rvalue_reference<C> {
        using type = meta::if_<
          is_reference<reference_t<C>>,
          remove_reference_t<reference_t<C>>&&,
          decay_t<reference_t<C>>>;
      };
      template <class C>
      requires
        requires(const C& c) { access::read(c); access::imove(c); }
      struct rvalue_reference<C> {
        using type = decltype(access::imove(declval<const C&>()));
      };

      template <class C>
      using rvalue_reference_t = meta::_t<rvalue_reference<C>>;

      template <class C, class O>
      requires
        requires(const C& c, const O& o) {
          access::read(c);
          access::read(o);
          c.iswap(o);
        }
      static constexpr void iswap(const C& c, const O& o)
      STL2_NOEXCEPT_RETURN((void)c.iswap(o))

      template <class I>
      requires
        requires(I&& i) {
          STL2_DEDUCE_AUTO_REF_REF(static_cast<I&&>(i).get());
        }
      static constexpr auto&& cursor(I&& i)
      STL2_NOEXCEPT_RETURN(static_cast<I&&>(i).get())
    };

    template <class C>
    using reference_t = access::reference_t<C>;
    template <class C>
    using rvalue_reference_t = access::rvalue_reference_t<C>;
    template <class C>
    using value_type_t = access::value_type_t<C>;
    template <class C>
    using difference_type_t = access::difference_type_t<C>;

    template <class C>
    concept bool Arrow() {
      return requires(const C& c) { access::arrow(c); };
    }
    template <class C>
    concept bool Next() {
      return requires(C& c) { access::next(c); };
    }
    template <class C>
    concept bool Prev() {
      return requires(C& c) { access::prev(c); };
    }
    template <class C>
    concept bool IndirectMove() {
      return requires(const C& c) { access::imove(c); };
    }
    template <class C, class O>
    concept bool IndirectSwap() {
      return requires(const C& c, const O& o) { access::iswap(c, o); };
    }
    template <class C>
    concept bool Advance() {
      return requires(C& c, difference_type_t<C> n) {
        access::advance(c, n);
      };
    }
    template <class C, class T>
    concept bool Writable() {
      return requires(C& c, T&& t) {
        access::write(c, __stl2::forward<T>(t));
      };
    }
    template <class C>
    concept bool Readable() {
      return requires(const C& c) {
        typename value_type_t<C>;
        access::read(c);
      };
    }
    template <class C>
    concept bool Cursor() {
      return Semiregular<C>() && Semiregular<access::mixin_t<C>>();
    }
    template <class S, class C>
    concept bool Sentinel() {
      return Cursor<C>() && requires(const C& c, const S& s) {
        access::equal(c, s);
      };
    }
    template <class S, class C>
    concept bool SizedSentinel() {
      return Sentinel<S, C>() && requires(const C& c, const S& s) {
        // Equivalent to: { access::distance(c, s) } -> Same<difference_type_t<C>;
        STL2_EXACT_TYPE_CONSTRAINT(access::distance(c, s), difference_type_t<C>);
      };
    }
    template <class C>
    concept bool Input() {
      return Cursor<C>() && Readable<C>() && Next<C>();
    }
    template <class C>
    concept bool Forward() {
      return Input<C>() && Sentinel<C, C>() &&
        !access::single_pass<C>::value;
    }
    template <class C>
    concept bool Bidirectional() {
      return Forward<C>() && Prev<C>();
    }
    template <class C>
    concept bool RandomAccess() {
      return Bidirectional<C>() && Advance<C>() && SizedSentinel<C, C>();
    }
    template <class C>
    concept bool Contiguous() {
      return RandomAccess<C>() && access::contiguous<C>::value;
    }

    template <class>
    struct category {};
    template <Input C>
    struct category<C> {
      using type = input_iterator_tag;
    };
    template <Forward C>
    struct category<C> {
      using type = forward_iterator_tag;
    };
    template <Bidirectional C>
    struct category<C> {
      using type = bidirectional_iterator_tag;
    };
    template <RandomAccess C>
    struct category<C> {
      using type = random_access_iterator_tag;
    };
    template <Contiguous C>
    struct category<C> {
      using type = ext::contiguous_iterator_tag;
    };
    template <class C>
    using category_t = meta::_t<category<C>>;
  }

  cursor::Cursor{C}
  class basic_iterator;

  namespace detail {
    template <class I>
    struct postfix_increment_proxy {
      using value_type = value_type_t<I>;
    private:
      mutable value_type value_;
    public:
      postfix_increment_proxy() = default;
      constexpr explicit postfix_increment_proxy(I const& x)
      noexcept(noexcept(value_type(*x)))
      : value_(*x)
      {}
      constexpr value_type& operator*() const noexcept {
        return value_;
      }
    };

    template <class I>
    struct writable_postfix_increment_proxy {
      using value_type = value_type_t<I>;
    private:
      mutable value_type value_;
      I it_;
    public:
      writable_postfix_increment_proxy() = default;
      constexpr explicit writable_postfix_increment_proxy(I x)
      : value_(*x), it_(__stl2::move(x))
      {}
      constexpr const writable_postfix_increment_proxy& operator*() const noexcept
      {
        return *this;
      }
      friend constexpr value_type&& iter_move(
        const writable_postfix_increment_proxy& ref)
      {
        return __stl2::move(ref.value_);
      }
      constexpr operator value_type&() const noexcept {
        return value_;
      }
      template <class T>
      requires Writable<I, T&&>()
      constexpr void operator=(T&& x) const
      STL2_NOEXCEPT_RETURN(
        (void)(*it_ = __stl2::forward<T>(x))
      )
      constexpr operator const I&() const noexcept {
        return it_;
      }
    };

    template <class Ref, class Val>
    using is_non_proxy_reference =
      is_convertible<
        const volatile remove_reference_t<Ref>*,
        const volatile Val*>;

    template <class I, class Val, class Ref, class Cat>
    using postfix_increment_result =
      meta::if_c<
        models::DerivedFrom<Cat, forward_iterator_tag>,
        I,
        meta::if_<
          is_non_proxy_reference<Ref, Val>,
          postfix_increment_proxy<I>,
          writable_postfix_increment_proxy<I>>>;

    template <class Derived, class Head>
    struct proxy_reference_conversion {
      operator Head() const
      noexcept(noexcept(Head(Head(declval<const Derived&>().get_()))))
      {
        return Head(static_cast<const Derived*>(this)->get_());
      }
    };

    template <class>
    struct cursor_traits {
    private:
      struct private_ {};
    public:
      using value_t_ = private_;
      using reference_t_ = private_;
      using rvalue_reference_t_ = private_;
      using common_refs = meta::list<>;
    };

    cursor::Readable{C}
    struct cursor_traits<C> {
      using value_t_ = cursor::value_type_t<C>;
      using reference_t_ = cursor::reference_t<C>;
      using rvalue_reference_t_ = cursor::rvalue_reference_t<C>;
    private:
      using R1 = reference_t_;
      using R2 = common_reference_t<reference_t_&&, value_t_&>;
      using R3 = common_reference_t<reference_t_&&, rvalue_reference_t_&&>;
      using tmp1 = meta::list<value_t_, R1>;
      using tmp2 =
        meta::if_<meta::in<tmp1, __uncvref<R2>>, tmp1, meta::push_back<tmp1, R2>>;
      using tmp3 =
        meta::if_<meta::in<tmp2, __uncvref<R3>>, tmp2, meta::push_back<tmp2, R3>>;
    public:
      using common_refs = meta::unique<meta::pop_front<tmp3>>;
    };

    template <class Cur>
    struct basic_proxy_reference
    : cursor_traits<Cur>
    , meta::inherit<
        meta::transform<
          typename cursor_traits<Cur>::common_refs,
          meta::bind_front<
            meta::quote<proxy_reference_conversion>,
            basic_proxy_reference<Cur>>>>
    {
    private:
      raw_ptr<Cur> cur_;
      template <class>
      friend struct basic_proxy_reference;
      template <class, class>
      friend struct proxy_reference_conversion;
      using typename cursor_traits<Cur>::value_t_;
      using typename cursor_traits<Cur>::reference_t_;
      using typename cursor_traits<Cur>::rvalue_reference_t_;
      static_assert(models::CommonReference<value_t_&, reference_t_&&>,
                    "Your readable and writable cursor must have a value type and a reference "
                    "type that share a common reference type. See the ranges::common_reference "
                    "type trait.");

      constexpr reference_t_ get_() const
      STL2_NOEXCEPT_RETURN(
        reference_t_(cursor::access::read(*cur_))
      )
      template <class T>
      constexpr void set_(T&& t)
      STL2_NOEXCEPT_RETURN(
        cursor::access::write(*cur_, static_cast<T&&>(t))
      )

    public:
      basic_proxy_reference() = default;
      basic_proxy_reference(const basic_proxy_reference&) = default;
      template <class OtherCur>
      requires ConvertibleTo<OtherCur*, Cur*>()
      constexpr basic_proxy_reference(
        const basic_proxy_reference<OtherCur>& that) noexcept
      : cur_(that.cur_)
      {}
      explicit constexpr basic_proxy_reference(Cur& cur) noexcept
      : cur_(&cur)
      {}
      constexpr basic_proxy_reference& operator=(basic_proxy_reference&& that)
      requires cursor::Readable<Cur>()
      {
        return *this = that;
      }
      constexpr basic_proxy_reference& operator=(const basic_proxy_reference& that)
      requires cursor::Readable<Cur>()
      {
        this->set_(that.get_());
        return *this;
      }

      template <class OtherCur>
      requires
        cursor::Readable<OtherCur>() &&
        cursor::Writable<Cur, cursor::reference_t<OtherCur>>()
      constexpr basic_proxy_reference& operator=(
        basic_proxy_reference<OtherCur>&& that)
      {
        return *this = that;
      }
      template <class OtherCur>
      requires
        cursor::Readable<OtherCur>() &&
        cursor::Writable<Cur, cursor::reference_t<OtherCur>>()
      constexpr basic_proxy_reference& operator=(
        const basic_proxy_reference<OtherCur>& that)
      {
        this->set_(that.get_());
        return *this;
      }
      template <class T>
      requires cursor::Writable<Cur, T&&>()
      constexpr basic_proxy_reference& operator=(T&& t)
      {
        this->set_(static_cast<T&&>(t));
        return *this;
      }
      friend constexpr bool operator==(
        const basic_proxy_reference& x, const value_t_& y)
      requires cursor::Readable<Cur>() && EqualityComparable<value_t_>()
      {
        return x.get_() == y;
      }
      friend constexpr bool operator!=(
        const basic_proxy_reference& x, const value_t_& y)
      requires cursor::Readable<Cur>() && EqualityComparable<value_t_>()
      {
        return !(x == y);
      }
      friend constexpr bool operator==(
        const value_t_& x, const basic_proxy_reference& y)
      requires cursor::Readable<Cur>() && EqualityComparable<value_t_>()
      {
        return x == y.get_();
      }
      friend constexpr bool operator!=(
        const value_t_& x, const basic_proxy_reference& y)
      requires cursor::Readable<Cur>() && EqualityComparable<value_t_>()
      {
        return !(x == y);
      }
      friend constexpr bool operator==(
        const basic_proxy_reference& x, const basic_proxy_reference& y)
      requires cursor::Readable<Cur>() && EqualityComparable<value_t_>()
      {
        return x.get_() == y.get_();
      }
      friend constexpr bool operator!=(
        const basic_proxy_reference& x, const basic_proxy_reference& y)
      requires cursor::Readable<Cur>() && EqualityComparable<value_t_>()
      {
        return !(x == y);
      }
    };

    template <class>
    constexpr bool is_writable = true;
    template <cursor::Readable C>
    constexpr bool is_writable<C> = false;
    template <cursor::Readable C>
    requires cursor::Writable<C, cursor::value_type_t<C>&&>()
    constexpr bool is_writable<C> = true;

    template <class Cur>
    struct iterator_associated_types_base {
      using reference_t = basic_proxy_reference<Cur>;
      using const_reference_t = basic_proxy_reference<const Cur>;
      using postfix_increment_result_t = basic_iterator<Cur>;
    };

    cursor::Readable{Cur}
    struct iterator_associated_types_base<Cur> {
      using reference_t =
        meta::if_c<
          is_writable<const Cur>,
          basic_proxy_reference<const Cur>,
          meta::if_c<
            is_writable<Cur>,
            basic_proxy_reference<Cur>,
            cursor::reference_t<Cur>>>;
      using const_reference_t = reference_t;
      using postfix_increment_result_t =
        postfix_increment_result<
          basic_iterator<Cur>, cursor::value_type_t<Cur>,
          reference_t, cursor::category_t<Cur>>;
    };
  } // namespace detail

  cursor::Cursor{C}
  class basic_iterator
  : public cursor::access::mixin_t<C>,
    private detail::iterator_associated_types_base<C>
  {
    friend cursor::access;
    using mixin_t = cursor::access::mixin_t<C>;
    using mixin_t::get;

    using assoc_t = detail::iterator_associated_types_base<C>;
    using typename assoc_t::postfix_increment_result_t;
    using typename assoc_t::reference_t;
    using typename assoc_t::const_reference_t;

    using D = cursor::difference_type_t<C>;

  public:
    basic_iterator() = default;
    template <ConvertibleTo<C> O>
    constexpr basic_iterator(basic_iterator<O>&& that)
    noexcept(is_nothrow_constructible<mixin_t, O&&>::value)
      : mixin_t(cursor::access::cursor(__stl2::move(that)))
    {}
    template <ConvertibleTo<C> O>
    constexpr basic_iterator(const basic_iterator<O>& that)
    noexcept(is_nothrow_constructible<mixin_t, const O&>::value)
      : mixin_t(cursor::access::cursor(that))
    {}
    using mixin_t::mixin_t;

    template <ConvertibleTo<C> O>
    constexpr basic_iterator& operator=(basic_iterator<O>&& that) &
    noexcept(is_nothrow_assignable<C&, O&&>::value)
    {
      get() = cursor::access::cursor(__stl2::move(that));
      return *this;
    }
    template <ConvertibleTo<C> O>
    constexpr basic_iterator& operator=(const basic_iterator<O>& that) &
    noexcept(is_nothrow_assignable<C&, const O&>::value)
    {
      get() = cursor::access::cursor(that);
      return *this;
    }

    constexpr decltype(auto) operator*() const
    noexcept(noexcept(cursor::access::read(declval<const C&>())))
    requires cursor::Readable<C>() && !detail::is_writable<C>
    {
      return cursor::access::read(get());
    }
    constexpr decltype(auto) operator*()
    noexcept(noexcept(reference_t{declval<mixin_t&>().get()}))
    requires cursor::Next<C>() && detail::is_writable<C>
    {
      return reference_t{get()};
    }
    constexpr decltype(auto) operator*() const
    noexcept(noexcept(
      const_reference_t{declval<const mixin_t&>().get()}))
    requires cursor::Next<C>() && detail::is_writable<C>
    {
      return const_reference_t{get()};
    }
    constexpr basic_iterator& operator*() noexcept
    requires !cursor::Next<C>()
    {
      return *this;
    }

    // operator->: "Manual" override
    constexpr decltype(auto) operator->() const
    noexcept(noexcept(cursor::access::arrow(declval<const C&>())))
    requires
      cursor::Readable<C>() &&
      cursor::Arrow<C>()
    {
      return cursor::access::arrow(get());
    }
    // operator->: Otherwise, if reference_t is an lvalue reference,
    //   return the address of operator*()
    constexpr auto operator->() const
    noexcept(noexcept(*declval<const basic_iterator&>()))
    requires
      cursor::Readable<C>() &&
      !cursor::Arrow<C>() &&
      is_lvalue_reference<const_reference_t>::value
    {
      return __addressof::impl(**this);
    }
    // operator->: Otherwise, return a proxy
    constexpr auto operator->() const
    noexcept(is_nothrow_move_constructible<
               detail::operator_arrow_proxy<basic_iterator>>::value &&
             noexcept(detail::operator_arrow_proxy<basic_iterator>{
                        *declval<const basic_iterator&>()}))
    requires
      cursor::Readable<C>() &&
      !cursor::Arrow<C>() &&
      !is_reference<const_reference_t>::value
    {
      return detail::operator_arrow_proxy<basic_iterator>{**this};
    }

    template <class T>
    requires
      !Same<decay_t<T>, basic_iterator>() &&
      !cursor::Next<C>() &&
      cursor::Writable<C, T>()
    constexpr basic_iterator& operator=(T&& t) &
    noexcept(noexcept(
      cursor::access::write(declval<C&>(), static_cast<T&&>(t))))
    {
      cursor::access::write(get(), static_cast<T&&>(t));
      return *this;
    }

    // Must be a template to workaround
    // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69096
    template <int=42>
    requires
      cursor::Readable<C>() &&
      cursor::IndirectMove<C>()
    friend constexpr decltype(auto) iter_move(const basic_iterator& i)
    noexcept(noexcept(cursor::access::imove(i.get())))
    {
      return cursor::access::imove(i.get());
    }

    template <class O>
    requires
      cursor::Readable<C>() &&
      cursor::Readable<O>() &&
      cursor::IndirectSwap<C, O>()
    friend constexpr void iter_swap(
      const basic_iterator& x, const basic_iterator<O>& y)
    STL2_NOEXCEPT_RETURN(
      cursor::access::iswap(x, y)
    )

    constexpr basic_iterator& operator++() & noexcept {
      return *this;
    }
    constexpr basic_iterator& operator++() &
    noexcept(noexcept(cursor::access::next(declval<C&>())))
    requires cursor::Next<C>()
    {
      cursor::access::next(get());
      return *this;
    }

    constexpr postfix_increment_result_t operator++(int) &
    noexcept(is_nothrow_constructible<postfix_increment_result_t, basic_iterator&>::value &&
             is_nothrow_move_constructible<postfix_increment_result_t>::value &&
             noexcept(++declval<basic_iterator&>()))
    {
      postfix_increment_result_t tmp(*this);
      ++*this;
      return tmp;
    }

    constexpr basic_iterator& operator--() &
    noexcept(noexcept(cursor::access::prev(declval<C&>())))
    requires cursor::Bidirectional<C>()
    {
      cursor::access::prev(get());
      return *this;
    }
    constexpr basic_iterator operator--(int) &
    noexcept(is_nothrow_copy_constructible<basic_iterator>::value &&
             is_nothrow_move_constructible<basic_iterator>::value &&
             noexcept(--declval<basic_iterator&>()))
    requires cursor::Bidirectional<C>()
    {
      auto tmp = *this;
      --*this;
      return tmp;
    }

    constexpr basic_iterator& operator+=(D n) &
    noexcept(noexcept(cursor::access::advance(declval<C&>(), n)))
    requires cursor::RandomAccess<C>()
    {
      cursor::access::advance(get(), n);
      return *this;
    }
    constexpr basic_iterator& operator-=(D n) &
    noexcept(noexcept(cursor::access::advance(declval<C&>(), -n)))
    requires cursor::RandomAccess<C>()
    {
      cursor::access::advance(get(), -n);
      return *this;
    }

    constexpr decltype(auto) operator[](D n) const
    noexcept(noexcept(*(declval<basic_iterator&>() + n)))
    requires cursor::RandomAccess<C>()
    {
      return *(*this + n);
    }

    // non-template type-symmetric operators to enable
    // implicit conversions.
    friend constexpr bool operator==(
      const basic_iterator& x, const basic_iterator& y)
    noexcept(noexcept(cursor::access::equal(x.get(), y.get())))
    requires
      cursor::Sentinel<C, C>()
    {
      return cursor::access::equal(x.get(), y.get());
    }
    friend constexpr bool operator!=(
      const basic_iterator& x, const basic_iterator& y)
    noexcept(noexcept(!(x == y)))
    requires
      cursor::Sentinel<C, C>()
    {
      return !(x == y);
    }

    friend constexpr D operator-(
      const basic_iterator& x, const basic_iterator& y)
    noexcept(noexcept(cursor::access::distance(y.get(), x.get())))
    requires
      cursor::SizedSentinel<C, C>()
    {
      return cursor::access::distance(y.get(), x.get());
    }

    friend constexpr bool operator<(
      const basic_iterator& x, const basic_iterator& y)
    noexcept(noexcept(x - y))
    requires
      cursor::SizedSentinel<C, C>()
    {
      return x - y < 0;
    }
    friend constexpr bool operator>(
      const basic_iterator& x, const basic_iterator& y)
    noexcept(noexcept(x - y))
    requires
      cursor::SizedSentinel<C, C>()
    {
      return x - y > 0;
    }
    friend constexpr bool operator<=(
      const basic_iterator& x, const basic_iterator& y)
    noexcept(noexcept(x - y))
    requires
      cursor::SizedSentinel<C, C>()
    {
      return x - y <= 0;
    }
    friend constexpr bool operator>=(
      const basic_iterator& x, const basic_iterator& y)
    noexcept(noexcept(x - y))
    requires
      cursor::SizedSentinel<C, C>()
    {
      return x - y >= 0;
    }
  };

  template <class C>
  struct difference_type<basic_iterator<C>> {
    using type = cursor::difference_type_t<C>;
  };

  template <cursor::Input C>
  struct iterator_category<basic_iterator<C>> {
    using type = cursor::category_t<C>;
  };

  template <cursor::Input C>
  struct value_type<basic_iterator<C>> {
    using type = cursor::value_type_t<C>;
  };

  template <class BI>
  requires
    meta::is<__uncvref<BI>, basic_iterator>::value
  constexpr decltype(auto) get_cursor(BI&& i)
  STL2_NOEXCEPT_RETURN(
    cursor::access::cursor(__stl2::forward<BI>(i))
  )

  template <class C>
  constexpr basic_iterator<C> operator+(
    const basic_iterator<C>& i, cursor::difference_type_t<C> n)
  noexcept(is_nothrow_copy_constructible<basic_iterator<C>>::value &&
           is_nothrow_move_constructible<basic_iterator<C>>::value &&
           noexcept(cursor::access::advance(declval<C&>(), n)))
  requires cursor::RandomAccess<C>()
  {
    auto tmp = i;
    cursor::access::advance(get_cursor(tmp), n);
    return tmp;
  }
  template <class C>
  constexpr basic_iterator<C> operator+(
    cursor::difference_type_t<C> n, const basic_iterator<C>& i)
  noexcept(noexcept(i + n))
  requires cursor::RandomAccess<C>()
  {
    return i + n;
  }
  template <class C>
  constexpr basic_iterator<C> operator-(
    const basic_iterator<C>& i, cursor::difference_type_t<C> n)
  noexcept(noexcept(i + -n))
  requires cursor::RandomAccess<C>()
  {
    return i + -n;
  }

  template <class C1, class C2>
  requires cursor::Sentinel<C2, C1>()
  constexpr bool operator==(
    const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor::access::equal(
      __stl2::get_cursor(lhs), __stl2::get_cursor(rhs))
  )

  template <class C, class S>
  requires cursor::Sentinel<S, C>()
  constexpr bool operator==(
    const basic_iterator<C>& lhs, const S& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor::access::equal(__stl2::get_cursor(lhs), rhs)
  )

  template <class C, class S>
  requires cursor::Sentinel<S, C>()
  constexpr bool operator==(
    const S& lhs, const basic_iterator<C>& rhs)
  STL2_NOEXCEPT_RETURN(
    rhs == lhs
  )

  template <class C1, class C2>
  requires cursor::Sentinel<C2, C1>()
  constexpr bool operator!=(
    const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
  STL2_NOEXCEPT_RETURN(
    !(lhs == rhs)
  )

  template <class C, class S>
  requires cursor::Sentinel<S, C>()
  constexpr bool operator!=(
    const basic_iterator<C>& lhs, const S& rhs)
  STL2_NOEXCEPT_RETURN(
    !cursor::access::equal(__stl2::get_cursor(lhs), rhs)
  )

  template <class C, class S>
  requires cursor::Sentinel<S, C>()
  constexpr bool operator!=(
    const S& lhs, const basic_iterator<C>& rhs)
  STL2_NOEXCEPT_RETURN(
    !cursor::access::equal(__stl2::get_cursor(rhs), lhs)
  )

  template <class C1, class C2>
  requires cursor::SizedSentinel<C1, C2>()
  constexpr cursor::difference_type_t<C2> operator-(
    const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor::access::distance(
      __stl2::get_cursor(rhs), __stl2::get_cursor(lhs))
  )

  template <class C, class S>
  requires cursor::SizedSentinel<S, C>()
  constexpr cursor::difference_type_t<C> operator-(
    const S& lhs, const basic_iterator<C>& rhs)
  STL2_NOEXCEPT_RETURN(
    cursor::access::distance(__stl2::get_cursor(rhs), lhs)
  )

  template <class C, class S>
  requires cursor::SizedSentinel<S, C>()
  constexpr cursor::difference_type_t<C> operator-(
    const basic_iterator<C>& lhs, const S& rhs)
  STL2_NOEXCEPT_RETURN(
    -(rhs - lhs)
  )

  template <class C1, class C2>
  requires cursor::SizedSentinel<C1, C2>()
  constexpr bool operator<(
    const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
  STL2_NOEXCEPT_RETURN(
    lhs - rhs < 0
  )

  template <class C1, class C2>
  requires cursor::SizedSentinel<C1, C2>()
  constexpr bool operator>(
    const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
  STL2_NOEXCEPT_RETURN(
    lhs - rhs > 0
  )

  template <class C1, class C2>
  requires cursor::SizedSentinel<C1, C2>()
  constexpr bool operator<=(
    const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
  STL2_NOEXCEPT_RETURN(
    lhs - rhs <= 0
  )

  template <class C1, class C2>
  requires cursor::SizedSentinel<C1, C2>()
  constexpr bool operator>=(
    const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
  STL2_NOEXCEPT_RETURN(
    lhs - rhs >= 0
  )
} STL2_CLOSE_NAMESPACE

#endif
