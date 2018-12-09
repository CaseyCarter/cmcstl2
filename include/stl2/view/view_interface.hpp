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
	namespace detail {
		template<Range Rng>
		struct __range_common_iterator_impl {
			using type = common_iterator<iterator_t<Rng>, sentinel_t<Rng>>;
		};
		template<CommonRange Rng>
		struct __range_common_iterator_impl<Rng> {
			using type = iterator_t<Rng>;
		};
		template<Range Rng>
		using __range_common_iterator =
			typename __range_common_iterator_impl<Rng>::type;

		template<class R>
		STL2_CONCEPT CanEmpty = Range<R> && requires(R& r) { __stl2::empty(r); };
		template<class R>
		STL2_CONCEPT SizedSentinelForwardRange = ForwardRange<R> && SizedSentinel<sentinel_t<R>, iterator_t<R>>;
		template<class C, class R> // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		STL2_CONCEPT ContainerConvertibleGCCBugs = InputRange<R> &&
			ConvertibleTo<iter_reference_t<iterator_t<R>>, iter_value_t<iterator_t<C>>> &&
			Constructible<C, __range_common_iterator<R>, __range_common_iterator<R>>;
		template<class C, class R>
		STL2_CONCEPT ContainerConvertible = ForwardRange<C> && !View<C> &&
			ContainerConvertibleGCCBugs<C, R>;

		template<Range R>
		constexpr bool is_in_range(R& r, iter_difference_t<iterator_t<R>> n) noexcept {
			return 0 <= n;
		}

		template<SizedRange R>
		constexpr bool is_in_range(R& r, iter_difference_t<iterator_t<R>> n)
			noexcept(noexcept(__stl2::size(r)))
		{
			auto sz = __stl2::size(r);
			using T = std::make_unsigned_t<common_type_t<
				iter_difference_t<iterator_t<R>>,
				decltype(sz)>>;
			return static_cast<T>(n) < static_cast<T>(sz);
		}
	}

	template<class D>
	requires std::is_class_v<D>
	class view_interface : public view_base {
	private:
		constexpr D& derived() noexcept {
			static_assert(DerivedFrom<D, view_interface>);
			return static_cast<D&>(*this);
		}
		constexpr const D& derived() const noexcept {
			static_assert(DerivedFrom<D, view_interface>);
			return static_cast<const D&>(*this);
		}
	public:
		constexpr bool empty() const requires ForwardRange<const D> {
			auto& d = derived();
			return __stl2::begin(d) == __stl2::end(d);
		}
		// Distinct named concept to workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82740
		template<detail::CanEmpty = const D> // gcc_bugs_bugs_bugs
		constexpr explicit operator bool() const {
			return !__stl2::empty(derived());
		}
		template<Range R = D>
		requires ContiguousIterator<iterator_t<R>>
		constexpr auto data() {
			auto& d = derived();
			return __stl2::empty(d) ? nullptr
				: detail::addressof(*__stl2::begin(d));
		}
		template<Range R = const D>
		requires ContiguousIterator<iterator_t<R>>
		constexpr auto data() const {
			auto& d = derived();
			return __stl2::empty(d) ? nullptr
				: detail::addressof(*__stl2::begin(d));
		}
		template<class R = const D> // gcc_bugs_bugs_bugs
		constexpr auto size() const
		// Distinct named concept to workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		requires detail::SizedSentinelForwardRange<R> {
			auto& d = derived();
			return __stl2::end(d) - __stl2::begin(d);
		}
		constexpr decltype(auto) front() requires ForwardRange<D> {
			auto& d = derived();
			const auto first = __stl2::begin(d);
			STL2_EXPECT(first != __stl2::end(d));
			return *first;
		}
		constexpr decltype(auto) front() const requires ForwardRange<const D> {
			auto& d = derived();
			const auto first = __stl2::begin(d);
			STL2_EXPECT(first != __stl2::end(d));
			return *first;
		}
		constexpr decltype(auto) back()
		requires BidirectionalRange<D> && CommonRange<D> {
			auto& d = derived();
			auto last = __stl2::end(d);
			STL2_EXPECT(__stl2::begin(d) != last);
			return *--last;
		}
		constexpr decltype(auto) back() const
		requires BidirectionalRange<const D> && CommonRange<const D> {
			auto& d = derived();
			auto last = __stl2::end(d);
			STL2_EXPECT(__stl2::begin(d) != last);
			return *--last;
		}
		template<RandomAccessRange R = D>
		constexpr decltype(auto) operator[](iter_difference_t<iterator_t<R>> n) {
			auto& d = derived();
			STL2_EXPECT(detail::is_in_range(d, n));
			return __stl2::begin(d)[n];
		}
		template<RandomAccessRange R = const D>
		constexpr decltype(auto) operator[](iter_difference_t<iterator_t<R>> n) const {
			auto& d = derived();
			STL2_EXPECT(detail::is_in_range(d, n));
			return __stl2::begin(d)[n];
		}
		// Distinct named concept to workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82507
		template<detail::ContainerConvertible<const D&> C>
		operator C() const {
			auto& d = derived();
			using I = detail::__range_common_iterator<const D>;
			return C(I{__stl2::begin(d)}, I{__stl2::end(d)});
		}
	};
} STL2_CLOSE_NAMESPACE

#endif
