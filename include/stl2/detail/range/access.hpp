// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_RANGE_ACCESS_HPP
#define STL2_DETAIL_RANGE_ACCESS_HPP

#include <initializer_list>
#include <stl2/memory.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>

// TODO:
// * constexpr specialization for data if iterator is a pointer.

///////////////////////////////////////////////////////////////////////////
// Range access [iterator.range]
//
STL2_OPEN_NAMESPACE {
	// begin
	namespace __begin {
		// Poison pill for std::begin. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		void begin(auto&) = delete;

		template <class R>
		constexpr bool has_member = false;
		template <class R>
			requires requires (R& r) {
				requires Iterator<__f<decltype(r.begin())>>();
			}
		constexpr bool has_member<R> = true;

		template <class R>
		constexpr bool has_non_member = false;
		template <class R>
		requires
			requires (R& r) {
				requires Iterator<__f<decltype(begin(r))>>();
			}
		constexpr bool has_non_member<R> = true;

		struct fn {
			template <class R, std::size_t N>
			constexpr R* operator()(R (&array)[N]) const noexcept {
				return array;
			}
			// Prefer member if it returns Iterator.
			template <class R>
			requires has_member<R>
			constexpr STL2_CONSTRAINED_RETURN(Iterator)
			operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.begin()
			)
			// Use ADL if it returns Iterator.
			template <class R>
			requires !has_member<R> && has_non_member<R>
			constexpr STL2_CONSTRAINED_RETURN(Iterator)
			operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				begin(r)
			)
			template <_IsNot<is_array> R>
			[[deprecated]] constexpr STL2_CONSTRAINED_RETURN(Iterator)
			operator()(const R&& r) const
			noexcept(noexcept(declval<const fn&>()(r)))
			requires has_member<const R> || has_non_member<const R>
			{
				return (*this)(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& begin = detail::static_const<__begin::fn>::value;
	}

	// end
	namespace __end {
		// Poison pill for std::end. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		void end(auto&) = delete;

		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires
			requires (R& r) {
				requires Sentinel<__f<decltype(r.end())>, decltype(__stl2::begin(r))>();
			}
		constexpr bool has_member<R> = true;

		template <class R>
		constexpr bool has_non_member = false;
		template <class R>
		requires
			requires (R& r) {
				requires Sentinel<__f<decltype(end(r))>, decltype(__stl2::begin(r))>();
			}
		constexpr bool has_non_member<R> = true;

		struct fn {
			template <class R, std::size_t N>
			constexpr R* operator()(R (&array)[N]) const noexcept {
				return array + N;
			}
			// Prefer member if it returns Sentinel.
			template <class R>
			requires has_member<R>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.end()
			)
			// Use ADL if it returns Sentinel.
			template <class R>
			requires !has_member<R> && has_non_member<R>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				end(r)
			)
			template <_IsNot<is_array> R>
			[[deprecated]] constexpr auto operator()(const R&& r) const
			noexcept(noexcept(declval<const fn&>()(r)))
			requires has_member<const R> || has_non_member<const R>
			{
				return (*this)(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& end = detail::static_const<__end::fn>::value;
	}

	// cbegin
	namespace __cbegin {
		struct fn {
			template <class R>
			requires
				requires (const R& r) { __stl2::begin(r); }
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::begin(r)
			)
			template <class R>
			[[deprecated]] constexpr auto operator()(const R&& r) const
			noexcept(noexcept(__stl2::begin(r)))
			requires
				requires (const R& r) { __stl2::begin(r); }
			{
				return __stl2::begin(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& cbegin = detail::static_const<__cbegin::fn>::value;
	}

	// cend
	namespace __cend {
		struct fn {
			template <class R>
			requires
				requires (const R& r) { __stl2::end(r); }
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::end(r)
			)
			template <class R>
			[[deprecated]] constexpr auto operator()(const R&& r) const
			noexcept(noexcept(__stl2::end(r)))
			requires
				requires (const R& r) { __stl2::end(r); }
			{
				return __stl2::end(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& cend = detail::static_const<__cend::fn>::value;
	}

	// rbegin
	namespace __rbegin {
		// Prefer member if it returns Iterator
		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires
			requires (R& r) {
				requires Iterator<__f<decltype(r.rbegin())>>();
			}
		constexpr bool has_member<R> = true;

		// Default to make_reverse_iterator(end(r)) for Bounded ranges of
		// Bidirectional iterators.
		template <class R>
		constexpr bool can_make_reverse = false;
		template <class R>
		requires
			requires (R& r) {
				requires Same<decltype(__stl2::begin(r)), decltype(__stl2::end(r))>();
				make_reverse_iterator(__stl2::end(r));
			}
		constexpr bool can_make_reverse<R> = true;

		struct fn {
			template <class R>
			requires has_member<R>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.rbegin()
			)
			template <class R>
			requires !has_member<R> && can_make_reverse<R>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				make_reverse_iterator(__stl2::end(r))
			)
			template <class R>
			[[deprecated]] constexpr auto operator()(const R&& r) const
			noexcept(noexcept(declval<const fn&>()(r)))
			requires has_member<const R> || can_make_reverse<const R>
			{
				return (*this)(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& rbegin = detail::static_const<__rbegin::fn>::value;
	}

	// rend
	namespace __rend {
		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires
			requires (R& r) {
				requires Sentinel<__f<decltype(r.rend())>, decltype(__stl2::rbegin(r))>();
			}
		constexpr bool has_member<R> = true;

		template <class R>
		constexpr bool can_make_reverse = false;
		template <class R>
		requires
			requires (R& r) {
				requires Same<decltype(__stl2::begin(r)), decltype(__stl2::end(r))>();
				make_reverse_iterator(__stl2::begin(r));
			}
		constexpr bool can_make_reverse<R> = true;

		struct fn {
			// Prefer member if it returns Sentinel
			template <class R>
			requires has_member<R>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(r.rend())

			// Default to make_reverse_iterator(begin(r)) for Bounded ranges of
			// Bidirectional iterators.
			template <class R>
			requires !has_member<R> && can_make_reverse<R>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::make_reverse_iterator(__stl2::begin(r))
			)
			template <class R>
			[[deprecated]] constexpr auto operator()(const R&& r) const
			noexcept(noexcept(declval<const fn&>()(r)))
			requires has_member<const R> || can_make_reverse<const R>
			{
				return (*this)(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& rend = detail::static_const<__rend::fn>::value;
	}

	// crbegin
	namespace __crbegin {
		struct fn {
			template <class R>
			requires
				requires (const R& r) { __stl2::rbegin(r); }
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::rbegin(r)
			)
			template <class R>
			[[deprecated]] constexpr auto operator()(const R&& r) const
			noexcept(noexcept(__stl2::rbegin(r)))
			requires
				requires (const R& r) { __stl2::rbegin(r); }
			{
				return __stl2::rbegin(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& crbegin = detail::static_const<__crbegin::fn>::value;
	}

	// crend
	namespace __crend {
		struct fn {
			template <class R>
			requires
				requires (const R& r) { __stl2::rend(r); }
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::rend(r)
			)
			template <class R>
			[[deprecated]] constexpr auto operator()(const R&& r) const
			noexcept(noexcept(__stl2::rend(r)))
			requires
				requires (const R& r) { __stl2::rend(r); }
			{
				return __stl2::rend(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& crend = detail::static_const<__crend::fn>::value;
	}

	///////////////////////////////////////////////////////////////////////////
	// Container access [iterator.container]
	//
	// size
	namespace __size {
		// Poison pill for std::size. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		void size(const auto&) = delete;

		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires
			requires (const R& r) {
				requires Integral<__f<decltype(r.size())>>();
			}
		constexpr bool has_member<R> = true;

		template <class R>
		constexpr bool has_non_member = false;
		template <class R>
		requires
			requires (const R& r) {
				requires Integral<__f<decltype(size(r))>>();
			}
		constexpr bool has_non_member<R> = true;

		template <class R>
		constexpr bool has_difference = false;
		template <class R>
		requires
			requires (const R& r) {
				requires SizedSentinel<decltype(__stl2::end(r)), decltype(__stl2::begin(r))>();
			}
		constexpr bool has_difference<R> = true;

		struct fn {
			template <class T, std::size_t N>
			constexpr std::size_t operator()(T(&)[N]) const noexcept {
				return N;
			}
			// Prefer member
			template <class R>
			requires has_member<R>
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				r.size()
			)
			// Use non-member
			template <class R>
			requires !has_member<R> && has_non_member<R>
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				size(r)
			)
			template <class R>
			requires !has_member<R> && !has_non_member<R> && has_difference<R>
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::end(r) - __stl2::begin(r)
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& size = detail::static_const<__size::fn>::value;
	}

	// empty
	namespace __empty {
		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires
			requires (const R& r) {
				STL2_CONVERSION_CONSTRAINT(r.empty(), bool);
			}
		constexpr bool has_member<R> = true;

		template <class R>
		constexpr bool has_size = false;
		template <class R>
		requires
			requires (const R& r) { __stl2::size(r); }
		constexpr bool has_size<R> = true;

		template <class R>
		constexpr bool has_begin_end = false;
		template <class R>
		requires
			requires (const R& r) {
				__stl2::end(r);
			}
		constexpr bool has_begin_end<R> = true;

		struct fn {
			// Prefer member
			template <class R>
			requires
				has_member<R>
			constexpr bool operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				static_cast<bool>(r.empty())
			)
			// Use size
			template <class R>
			requires
				!has_member<R> && has_size<R>
			constexpr bool operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::size(r) == 0
			)
			// Use begin == end
			template <class R>
			requires
				!has_member<R> && !has_size<R> && has_begin_end<R>
			constexpr bool operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				static_cast<bool>(__stl2::begin(r) == __stl2::end(r))
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& empty = detail::static_const<__empty::fn>::value;
	}

	// data
	namespace __data {
		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires
			requires (R& r) {
				requires _Is<__f<decltype(r.data())>, std::is_pointer>;
			}
		constexpr bool has_member<R> = true;

		template <class R>
		constexpr bool has_pointer_iterator = false;
		template <class R>
		requires
			requires (R& r) {
				requires is_pointer<decltype(__stl2::begin(r))>::value;
			}
		constexpr bool has_pointer_iterator<R> = true;

		template <class R>
		constexpr bool has_contiguous_iterator = false;
		template <class R>
		requires
			requires (R& r) {
				requires ext::ContiguousIterator<decltype(__stl2::begin(r))>();
			}
		constexpr bool has_contiguous_iterator<R> = true;

		struct fn {
			// Prefer member
			template <class R>
			requires
				has_member<R>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.data()
			)

			// Return begin(r) if it's a pointer
			template <class R>
			requires
				!has_member<R> && has_pointer_iterator<R>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::begin(r)
			)

			// Extension: Support contiguous ranges with non-pointer
			//            iterators.
			template <class R>
			requires
				!has_member<R> &&
				!has_pointer_iterator<R> &&
				has_contiguous_iterator<R>
			constexpr auto operator()(R& r) const
			noexcept(noexcept(__stl2::begin(r) == __stl2::end(r)
				? nullptr : __stl2::addressof(*__stl2::begin(r))))
			{
				auto i = __stl2::begin(r);
				return i == __stl2::end(r) ? nullptr : __stl2::addressof(*i);
			}

			template <class R>
			[[deprecated]] constexpr auto operator()(const R&& r) const
			noexcept(noexcept(declval<const fn&>()(r)))
			requires
				has_member<const R> ||
				has_pointer_iterator<const R> ||
				has_contiguous_iterator<const R>
			{
				return (*this)(r);
			}
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& data = detail::static_const<__data::fn>::value;
	}

	namespace ext {
		// cdata
		namespace __cdata {
			struct fn {
				template <class R>
					requires requires (const R& r) { __stl2::data(r); }
				constexpr auto operator()(const R& r) const
				STL2_NOEXCEPT_RETURN(__stl2::data(r))

				template <class R>
				[[deprecated]] constexpr auto operator()(const R&& r) const
					noexcept(noexcept(__stl2::data(r)))
					requires requires (const R& r) { __stl2::data(r); }
				{
					return __stl2::data(r);
				}
			};
		}
		// Workaround GCC PR66957 by declaring this unnamed namespace inline.
		inline namespace {
			constexpr auto& cdata = detail::static_const<__cdata::fn>::value;
		}
	}
} STL2_CLOSE_NAMESPACE

#undef STL2_TREAT_RVALUES_AS_CONST

#endif
