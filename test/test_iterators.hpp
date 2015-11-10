//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef RANGES_TEST_ITERATORS_HPP
#define RANGES_TEST_ITERATORS_HPP

#include <stl2/iterator.hpp>

template <class It, bool Sized = false>
class sentinel;

template <class It>
class output_iterator;

template <class It>
class input_iterator;

template <class It>
class forward_iterator;

template <class It>
class bidirectional_iterator;

template <class It>
class random_access_iterator;


template <class Iter, bool Sized>
constexpr Iter base(sentinel<Iter, Sized> i) { return i.base(); }

template <class Iter>
constexpr Iter base(output_iterator<Iter> i) { return i.base(); }

template <class Iter>
constexpr Iter base(input_iterator<Iter> i) { return i.base(); }

template <class Iter>
constexpr Iter base(forward_iterator<Iter> i) { return i.base(); }

template <class Iter>
constexpr Iter base(bidirectional_iterator<Iter> i) { return i.base(); }

template <class Iter>
constexpr Iter base(random_access_iterator<Iter> i) { return i.base(); }

template <class Iter>    // everything else
constexpr Iter base(Iter i) { return i; }


template <class It, bool Sized>
class sentinel
{
    It it_;
public:
    constexpr sentinel() : it_() {}
    constexpr explicit sentinel(It it) : it_(it) {}
    constexpr It base() const { return it_; }
    constexpr friend bool operator==(const sentinel& x, const sentinel& y)
    {
        STL2_ASSUME(x.it_ == y.it_); (void)x; (void)y;
        return true;
    }
    constexpr friend bool operator!=(const sentinel& x, const sentinel& y)
    {
        STL2_ASSUME(x.it_ == y.it_); (void)x; (void)y;
        return false;
    }
    template<typename I>
    constexpr friend bool operator==(const I& x, const sentinel& y)
    {
        using ::base;
        return base(x) == y.it_;
    }
    template<typename I>
    constexpr friend bool operator!=(const I& x, const sentinel& y)
    {
        return !(x == y);
    }
    template<typename I>
    constexpr friend bool operator==(const sentinel& x, const I& y)
    {
        using ::base;
        return x.it_ == base(y);
    }
    template<typename I>
    constexpr friend bool operator!=(const sentinel& x, const I& y)
    {
        return !(x == y);
    }
};

// For making sized iterator ranges:
template<template<typename> class I, typename It>
constexpr
std::ptrdiff_t operator-(sentinel<It, true> end, I<It> begin)
{
    return base(end) - base(begin);
}

template<typename It, template<typename> class I>
constexpr
std::ptrdiff_t operator-(I<It> end, sentinel<It, true> begin)
{
    return -(begin - end);
}

template<typename It>
inline constexpr
std::ptrdiff_t operator-(sentinel<It, true> x, sentinel<It, true> y)
{
    STL2_ASSUME(x == y);
    return 0;
}

template <class It>
class output_iterator
{
    It it_;

    template <class U> friend class output_iterator;
public:
    using difference_type = __stl2::difference_type_t<It>;
    using pointer = It;
    using reference = __stl2::reference_t<It>;

    constexpr It base() const {return it_;}

    constexpr output_iterator () {}
    constexpr explicit output_iterator(It it) : it_(it) {}
    template <class U>
      requires __stl2::ConvertibleTo<U, It>()
    constexpr
    output_iterator(const output_iterator<U>& u) :it_(u.it_) {}

    constexpr reference operator*() const {return *it_;}

    constexpr output_iterator& operator++() {++it_; return *this;}
    constexpr output_iterator operator++(int)
    {output_iterator tmp(*this); ++(*this); return tmp;}
};

template <class It>
class input_iterator
{
    It it_;

    template <class U> friend class input_iterator;
public:
    typedef __stl2::input_iterator_tag iterator_category;
    typedef __stl2::value_type_t<It>      value_type;
    typedef __stl2::difference_type_t<It> difference_type;
    typedef It                       pointer;
    typedef __stl2::reference_t<It>  reference;

    constexpr It base() const {return it_;}

    constexpr input_iterator() : it_() {}
    constexpr explicit input_iterator(It it) : it_(it) {}
    template <class U>
      requires __stl2::ConvertibleTo<U, It>()
    constexpr input_iterator(const input_iterator<U>& u) :it_(u.it_) {}

    constexpr reference operator*() const {return *it_;}
    constexpr pointer operator->() const {return it_;}

    constexpr input_iterator& operator++() {++it_; return *this;}
    constexpr input_iterator operator++(int)
        {input_iterator tmp(*this); ++(*this); return tmp;}

    constexpr
    friend bool operator==(const input_iterator& x, const input_iterator& y)
        {return x.it_ == y.it_;}
    constexpr
    friend bool operator!=(const input_iterator& x, const input_iterator& y)
        {return !(x == y);}
};

template <class T, class U>
constexpr
bool
operator==(const input_iterator<T>& x, const input_iterator<U>& y)
{
    return x.base() == y.base();
}

template <class T, class U>
constexpr
bool
operator!=(const input_iterator<T>& x, const input_iterator<U>& y)
{
    return !(x == y);
}

template <class It>
class forward_iterator
{
    It it_;

    template <class U> friend class forward_iterator;
public:
    typedef __stl2::forward_iterator_tag iterator_category;
    typedef __stl2::value_type_t<It>        value_type;
    typedef __stl2::difference_type_t<It>   difference_type;
    typedef It                         pointer;
    typedef __stl2::reference_t<It>    reference;

    constexpr It base() const {return it_;}

    constexpr forward_iterator() : it_() {}
    constexpr explicit forward_iterator(It it) : it_(it) {}
    template <class U>
      requires __stl2::ConvertibleTo<U, It>()
    constexpr forward_iterator(const forward_iterator<U>& u) :it_(u.it_) {}

    constexpr reference operator*() const {return *it_;}
    constexpr pointer operator->() const {return it_;}

    constexpr forward_iterator& operator++() {++it_; return *this;}
    constexpr forward_iterator operator++(int)
    {forward_iterator tmp(*this); ++(*this); return tmp;}

    constexpr
    friend bool operator==(const forward_iterator& x, const forward_iterator& y)
    {return x.it_ == y.it_;}
    constexpr
    friend bool operator!=(const forward_iterator& x, const forward_iterator& y)
    {return !(x == y);}
};

template <class T, class U>
constexpr
bool
operator==(const forward_iterator<T>& x, const forward_iterator<U>& y)
{
    return x.base() == y.base();
}

template <class T, class U>
constexpr
bool
operator!=(const forward_iterator<T>& x, const forward_iterator<U>& y)
{
    return !(x == y);
}

template <class It>
class bidirectional_iterator
{
    It it_;

    template <class U> friend class bidirectional_iterator;
public:
    typedef __stl2::bidirectional_iterator_tag iterator_category;
    typedef __stl2::value_type_t<It>              value_type;
    typedef __stl2::difference_type_t<It>         difference_type;
    typedef It                               pointer;
    typedef __stl2::reference_t<It>          reference;

    constexpr It base() const {return it_;}

    constexpr bidirectional_iterator() : it_() {}
    constexpr explicit bidirectional_iterator(It it) : it_(it) {}
    template <class U>
      requires __stl2::ConvertibleTo<U, It>()
    constexpr bidirectional_iterator(const bidirectional_iterator<U>& u) :it_(u.it_) {}

    constexpr reference operator*() const {return *it_;}
    constexpr pointer operator->() const {return it_;}

    constexpr bidirectional_iterator& operator++() {++it_; return *this;}
    constexpr bidirectional_iterator operator++(int)
    {bidirectional_iterator tmp(*this); ++(*this); return tmp;}

    constexpr bidirectional_iterator& operator--() {--it_; return *this;}
    constexpr bidirectional_iterator operator--(int)
    {bidirectional_iterator tmp(*this); --(*this); return tmp;}
};

template <class T, class U>
constexpr
bool
operator==(const bidirectional_iterator<T>& x, const bidirectional_iterator<U>& y)
{
    return x.base() == y.base();
}

template <class T, class U>
constexpr
bool
operator!=(const bidirectional_iterator<T>& x, const bidirectional_iterator<U>& y)
{
    return !(x == y);
}

template <class It>
class random_access_iterator
{
    It it_;

    template <class U> friend class random_access_iterator;
public:
    typedef __stl2::random_access_iterator_tag iterator_category;
    typedef __stl2::value_type_t<It>              value_type;
    typedef __stl2::difference_type_t<It>         difference_type;
    typedef It                               pointer;
    typedef __stl2::reference_t<It>          reference;

    constexpr It base() const {return it_;}

    constexpr random_access_iterator() : it_() {}
    constexpr explicit random_access_iterator(It it) : it_(it) {}
    template <class U>
      requires __stl2::ConvertibleTo<U, It>()
    constexpr random_access_iterator(const random_access_iterator<U>& u) :it_(u.it_) {}

    constexpr reference operator*() const {return *it_;}
    constexpr pointer operator->() const {return it_;}

    constexpr random_access_iterator& operator++() {++it_; return *this;}
    constexpr random_access_iterator operator++(int)
    {random_access_iterator tmp(*this); ++(*this); return tmp;}

    constexpr random_access_iterator& operator--() {--it_; return *this;}
    constexpr random_access_iterator operator--(int)
    {random_access_iterator tmp(*this); --(*this); return tmp;}

    constexpr
    random_access_iterator& operator+=(difference_type n) {it_ += n; return *this;}
    constexpr
    random_access_iterator operator+(difference_type n) const
    {random_access_iterator tmp(*this); tmp += n; return tmp;}
    constexpr
    friend random_access_iterator operator+(difference_type n, random_access_iterator x)
    {x += n; return x;}
    constexpr
    random_access_iterator& operator-=(difference_type n) {return *this += -n;}
    constexpr
    random_access_iterator operator-(difference_type n) const
    {random_access_iterator tmp(*this); tmp -= n; return tmp;}

    constexpr
    reference operator[](difference_type n) const {return it_[n];}
};

template <class T, class U>
constexpr
bool
operator==(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return x.base() == y.base();
}

template <class T, class U>
constexpr
bool
operator!=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(x == y);
}

template <class T, class U>
constexpr
bool
operator<(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return x.base() < y.base();
}

template <class T, class U>
constexpr
bool
operator<=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(y < x);
}

template <class T, class U>
constexpr
bool
operator>(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return y < x;
}

template <class T, class U>
constexpr
bool
operator>=(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return !(x < y);
}

template <class T, class U>
constexpr
__stl2::difference_type_t<T>
operator-(const random_access_iterator<T>& x, const random_access_iterator<U>& y)
{
    return x.base() - y.base();
}

template<typename It, bool Sized = false>
struct sentinel_type
{
    using type = It;
};

template<typename T, bool Sized>
struct sentinel_type<T*, Sized>
{
    using type = sentinel<T*, Sized>;
};

template<template<typename> class I, typename It, bool Sized>
struct sentinel_type<I<It>, Sized>
{
    using type = sentinel<It, Sized>;
};

STL2_OPEN_NAMESPACE {
    template<typename I0, bool S, typename I1>
    struct common_type<sentinel<I0, S>, I1>
    {
        using type = common_iterator<I1, sentinel<I0, S>>;
    };
    template<typename I0, typename I1, bool S>
    struct common_type<I0, sentinel<I1, S>>
    {
        using type = common_iterator<I0, sentinel<I1, S>>;
    };
    template<typename I, bool B>
    struct common_type<sentinel<I, B>, sentinel<I, B>>
    {
        using type = sentinel<I>;
    };
    template<typename I0, bool S, typename I1, template <class> class TQual,
             template <class> class UQual>
    struct basic_common_reference<sentinel<I0, S>, I1, TQual, UQual>
    {
        using type = common_iterator<I1, sentinel<I0, S>>;
    };
    template<typename I0, typename I1, bool S, template <class> class TQual,
             template <class> class UQual>
    struct basic_common_reference<I0, sentinel<I1, S>, TQual, UQual>
    {
        using type = common_iterator<I0, sentinel<I1, S>>;
    };
    template<typename I, bool B, template <class> class TQual,
             template <class> class UQual>
    struct basic_common_reference<sentinel<I, B>, sentinel<I, B>, TQual, UQual>
    {
        using type = sentinel<I>;
    };
} STL2_CLOSE_NAMESPACE

#endif  // ITERATORS_H
