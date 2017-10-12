// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_INTERFACE_HPP
#define STL2_VIEW_INTERFACE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/concepts/callable.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <Range Rng>
		struct __range_common_iterator_impl {
			using type = common_iterator<iterator_t<Rng>, sentinel_t<Rng>>;
		};
		template <BoundedRange Rng>
		struct __range_common_iterator_impl<Rng> {
			using type = iterator_t<Rng>;
		};
		template <Range Rng>
		using __range_common_iterator =
			typename __range_common_iterator_impl<Rng>::type;

		// \expos
		template <class Rng, class Cont>
		concept bool _ConvertibleToContainer =
			Range<Cont> &&
			!View<Cont> &&
			MoveConstructible<Cont> &&
			ConvertibleTo<value_type_t<iterator_t<Rng>>, value_type_t<iterator_t<Cont>>> &&
			Constructible<Cont, __range_common_iterator<Rng>, __range_common_iterator<Rng>>;

		template <class D>
		class view_interface : __stl2::view_base {
		private:
			constexpr D& derived() noexcept {
				return static_cast<D&>(*this);
			}
			constexpr const D& derived() const noexcept { // exposition only
				return static_cast<const D&>(*this);
			}
		public:
			constexpr bool empty() const requires ForwardRange<const D> {
				return __stl2::begin(derived()) == __stl2::end(derived());
			}
			constexpr explicit operator bool() const requires ForwardRange<const D> {
				return !empty();
			}
			constexpr bool operator!() const requires ForwardRange<const D> {
				return empty();
			}
			// Template to work around https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <ForwardRange R = const D>
			constexpr auto size() const
			requires SizedSentinel<sentinel_t<R>, iterator_t<R>> {
				return __stl2::end(derived()) - __stl2::begin(derived());
			}
			constexpr decltype(auto) front() requires ForwardRange<D> {
				return *__stl2::begin(derived());
			}
			constexpr decltype(auto) front() const requires ForwardRange<const D> {
				return *__stl2::begin(derived());
			}
			constexpr decltype(auto) back()
			requires BidirectionalRange<D> && BoundedRange<D> {
				return *__stl2::prev(__stl2::end(derived()));
			}
			constexpr decltype(auto) back() const
			requires BidirectionalRange<const D>  && BoundedRange<const D>{
				return *__stl2::prev(__stl2::end(derived()));
			}
			template <RandomAccessRange R = D>
			constexpr decltype(auto) operator[](difference_type_t<iterator_t<R>> n) {
				return __stl2::begin(derived())[n];
			}
			template <RandomAccessRange R = const D>
			constexpr decltype(auto) operator[](difference_type_t<iterator_t<R>> n) const {
				return __stl2::begin(derived())[n];
			}
			template <RandomAccessRange R = D>
			requires SizedRange<R>
			constexpr decltype(auto) at(difference_type_t<iterator_t<R>> n) {
				return n < 0 || n >= __stl2::size(derived())
					? throw std::out_of_range{}
					: (*this)[n];
			}
			template <RandomAccessRange R = const D>
			requires SizedRange<R>
			constexpr decltype(auto) at(difference_type_t<iterator_t<R>> n) const {
				return n < 0 || n >= __stl2::size(derived())
					? throw std::out_of_range{}
					: (*this)[n];
			}
			// Not to spec: `InputRange R = const D` to work around
			// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <ForwardRange C, InputRange R = const D>
			requires !View<C> && MoveConstructible<C> &&
				ConvertibleTo<value_type_t<iterator_t<R>>, value_type_t<iterator_t<C>>> &&
				Constructible<C, __range_common_iterator<R>, __range_common_iterator<R>>
			operator C () const {
				using I = __range_common_iterator<R>;
				return C{I{__stl2::begin(derived())}, I{__stl2::end(derived())}};
			}
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
