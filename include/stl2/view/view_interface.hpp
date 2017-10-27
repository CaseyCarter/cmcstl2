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
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

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
	}

	namespace detail {
		template <class R>
		concept bool CanEmpty = Range<R> && requires(R& r) { __stl2::empty(r); };
		template <class R>
		concept bool SizedSentinelForwardRange = ForwardRange<R> && SizedSentinel<sentinel_t<R>, iterator_t<R>>;
		template <class C, class R>
		concept bool ContainerConvertible = InputRange<R> && ForwardRange<C> && !View<C> &&
			ConvertibleTo<reference_t<iterator_t<R>>, value_type_t<iterator_t<C>>> &&
			Constructible<C, ext::__range_common_iterator<R>, ext::__range_common_iterator<R>>;
	}

	namespace ext {
		// \expos
		template <class Rng, class Cont>
		concept bool _ConvertibleToContainer =
			Range<Cont> &&
			!View<Cont> &&
			MoveConstructible<Cont> &&
			ConvertibleTo<value_type_t<iterator_t<Rng>>, value_type_t<iterator_t<Cont>>> &&
			Constructible<Cont, __range_common_iterator<Rng>, __range_common_iterator<Rng>>;

		template <class D>
		requires std::is_class_v<D>
		class view_interface : public view_base {
		private:
			constexpr D& derived() noexcept {
				static_assert(models::DerivedFrom<D, view_interface>);
				return static_cast<D&>(*this);
			}
			constexpr const D& derived() const noexcept {
				static_assert(models::DerivedFrom<D, view_interface>);
				return static_cast<const D&>(*this);
			}
		public:
			constexpr bool empty() const requires ForwardRange<const D> {
				return __stl2::begin(derived()) == __stl2::end(derived());
			}
			constexpr explicit operator bool() const
			// Distinct named concept to workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82740
			requires detail::CanEmpty<const D> {
				return !__stl2::empty(derived());
			}
			constexpr auto size() const
			// Distinct named concept to workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			requires detail::SizedSentinelForwardRange<const D> {
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
			requires BidirectionalRange<const D> && BoundedRange<const D> {
				return *__stl2::prev(__stl2::end(derived()));
			}
			template <RandomAccessRange R = D>
			constexpr decltype(auto) operator[](difference_type_t<iterator_t<R>> n) {
				auto& d = derived();
				STL2_EXPECT(!models::SizedRange<R> || n < __stl2::distance(d));
				return __stl2::begin(d)[n];
			}
			template <RandomAccessRange R = const D>
			constexpr decltype(auto) operator[](difference_type_t<iterator_t<R>> n) const {
				auto& d = derived();
				STL2_EXPECT(!models::SizedRange<R> || n < __stl2::distance(d));
				return __stl2::begin(d)[n];
			}
			template <RandomAccessRange R = D>
			requires SizedRange<R>
			constexpr decltype(auto) at(difference_type_t<iterator_t<R>> n) {
				return n < 0 || n >= __stl2::size(derived())
					? throw std::out_of_range{}
					: derived()[n];
			}
			template <RandomAccessRange R = const D>
			requires SizedRange<R>
			constexpr decltype(auto) at(difference_type_t<iterator_t<R>> n) const {
				return n < 0 || n >= __stl2::size(derived())
					? throw std::out_of_range{}
					: derived()[n];
			}
			// Distinct named concept to workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
			template <detail::ContainerConvertible<const D> C>
			operator C () const {
				using I = __range_common_iterator<const D>;
				return C(I{__stl2::begin(derived())}, I{__stl2::end(derived())});
			}
		};
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
