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
#include <string>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/memory/addressof.hpp>

///////////////////////////////////////////////////////////////////////////
// Range access [iterator.range]
//
STL2_OPEN_NAMESPACE {
	// begin
	namespace __begin {
		// Poison pill for std::begin. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		// Not to spec
		template <class R>
		void begin(R&&) = delete;

		template <class R>
		constexpr bool has_member = false;
		template <class R>
			requires requires(R& r) {
				//{ r.begin() } -> Iterator;
				requires Iterator<__f<decltype(r.begin())>>;
			}
		constexpr bool has_member<R&> = true;

		template <class R>
		constexpr bool has_non_member = false;
		template <class R>
		requires requires(R&& r) {
				// { begin((R&&) r) } -> Iterator;
				requires Iterator<__f<decltype(begin((R&&) r))>>;
			}
		constexpr bool has_non_member<R> = true;

		struct fn {
			template <class R, std::size_t N>
			void operator()(R (&&)[N]) const = delete;
			template <class T>
			void operator()(std::initializer_list<T>&&) const = delete;
			template <class R, std::size_t N>
			constexpr R* operator()(R (&array)[N]) const noexcept {
				return array;
			}
			// Prefer member if it returns Iterator.
			template <class R>
			requires has_member<R&>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.begin()
			)
			// Use ADL if it returns Iterator.
			template <class R>
			requires !has_member<R> && has_non_member<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				begin((R&&) r)
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& begin = detail::static_const<__begin::fn>::value;
	}

	template <class R>
	using __begin_t = decltype(__stl2::begin(std::declval<R>()));

	// end
	namespace __end {
		// Poison pill for std::end. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		// Not to spec
		template <class R>
		void end(R&&) = delete;

		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires requires(R& r) {
				typename __begin_t<R&>;
				// { r.end() } -> Sentinel<__begin_t<R&>>;
				requires Sentinel<__f<decltype(r.end())>, __begin_t<R&>>;
			}
		constexpr bool has_member<R&> = true;

		template <class R>
		constexpr bool has_non_member = false;
		template <class R>
		requires requires(R&& r) {
				typename __begin_t<R>;
				// { end((R&&) r) } -> Sentinel<__begin_t<R>>;
				requires Sentinel<__f<decltype(end((R&&) r))>, __begin_t<R&>>;
			}
		constexpr bool has_non_member<R> = true;

		struct fn {
			template <class R, std::size_t N>
			void operator()(R (&&)[N]) const = delete;
			template <class T>
			void operator()(std::initializer_list<T>&&) const = delete;
			template <class R, std::size_t N>
			constexpr R* operator()(R (&array)[N]) const noexcept {
				return array + N;
			}
			// Prefer member if it returns Sentinel.
			template <class R>
			requires has_member<R&>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.end()
			)
			// Use ADL if it returns Sentinel.
			template <class R>
			requires !has_member<R> && has_non_member<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				end((R&&) r)
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& end = detail::static_const<__end::fn>::value;
	}

	template <class R>
	using __end_t = decltype(__stl2::end(std::declval<R>()));

	// cbegin
	namespace __cbegin {
		struct fn {
			template <class R>
			requires requires { typename __begin_t<const R&>; }
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::begin(r)
			)
			template <class R>
			requires requires { typename __begin_t<const R>; }
			constexpr auto operator()(const R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::begin((const R&&) r)
			)
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
			requires requires { typename __end_t<const R&>; }
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::end(r)
			)
			template <class R>
			requires requires { typename __end_t<const R>; }
			constexpr auto operator()(const R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::end((const R&&) r)
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& cend = detail::static_const<__cend::fn>::value;
	}

	// rbegin
	namespace __rbegin {
		// Poison pill, not to spec
		template <class R>
		void rbegin(R&&) = delete;

		// Prefer member if it returns Iterator
		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires requires(R& r) {
				// { r.rbegin() } -> Iterator;
				requires Iterator<__f<decltype(r.rbegin())>>;
			}
		constexpr bool has_member<R&> = true;

		template <class R>
		constexpr bool has_non_member = false;
		template <class R>
		requires requires(R&& r) {
				// { rbegin((R&&) r) } -> Iterator;
				requires Iterator<__f<decltype(rbegin((R&&) r))>>;
			}
		constexpr bool has_non_member<R> = true;

		// Default to make_reverse_iterator(end(r)) for Bounded ranges of
		// Bidirectional iterators.
		template <class R>
		constexpr bool can_make_reverse = false;
		template <class R>
		requires Same<__begin_t<R>, __end_t<R>> &&
			requires(R&& r) {
				__stl2::make_reverse_iterator(__stl2::end((R&&) r));
			}
		constexpr bool can_make_reverse<R> = true;

		struct fn {
			// Consider the case of a hypothetical reverse_subrange whose
			// begin/end members pass its stored iterators through
			// make_reverse_iterator. Like subrange, it is only a pair of
			// iterators, and so rbegin/rend make sense on an rvalue
			// reverse_subrange.
			template <class R>
			requires has_member<R&>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.rbegin()
			)
			template <class R>
			requires !has_member<R> && has_non_member<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				rbegin((R&&) r)
			)
			template <class R>
			requires !has_member<R> && !has_non_member<R> && can_make_reverse<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::make_reverse_iterator(__stl2::end((R&&) r))
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& rbegin = detail::static_const<__rbegin::fn>::value;
	}

	template <class T>
	using __rbegin_t = decltype(__stl2::rbegin(std::declval<T>()));

	// rend
	namespace __rend {
		// poison-pill overload, not to spec
		template <class R>
		void rend(R&&) = delete;

		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires requires(R& r) {
				typename __rbegin_t<R&>;
				// { r.rend() } -> Sentinel<__rbegin_t<R&>>;
				requires Sentinel<__f<decltype(r.rend())>, __rbegin_t<R&>>;
			}
		constexpr bool has_member<R&> = true;

		template <class R>
		constexpr bool has_non_member = false;
		template <class R>
		requires requires(R&& r) {
				typename __rbegin_t<R>;
				// { rend((R&&) r) } -> Sentinel<__rbegin_t<R>>;
				requires Sentinel<__f<decltype(rend((R&&) r))>, __rbegin_t<R>>;
			}
		constexpr bool has_non_member<R> = true;

		template <class R>
		constexpr bool can_make_reverse = false;
		template <class R>
		requires Same<__begin_t<R>, __end_t<R>> &&
			requires(R&& r) {
				__stl2::make_reverse_iterator(__stl2::begin((R&&) r));
			}
		constexpr bool can_make_reverse<R> = true;

		struct fn {
			// Prefer member if it returns Sentinel
			template <class R>
			requires has_member<R&>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.rend()
			)
			template <class R>
			requires !has_member<R> && has_non_member<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				rend((R&&) r)
			)
			// Default to make_reverse_iterator(begin(r)) for Bounded ranges of
			// Bidirectional iterators.
			template <class R>
			requires !has_member<R> && !has_non_member<R> && can_make_reverse<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::make_reverse_iterator(__stl2::begin((R&&) r))
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& rend = detail::static_const<__rend::fn>::value;
	}

	template <class T>
	using __rend_t = decltype(__stl2::rend(std::declval<T>()));

	// crbegin
	namespace __crbegin {
		struct fn {
			template <class R>
			requires requires { typename __rbegin_t<const R&>; }
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::rbegin(r)
			)
			template <class R>
			requires requires { typename __rbegin_t<const R>; }
			constexpr auto operator()(const R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::rbegin((const R&&) r)
			)
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
			requires requires { typename __rend_t<const R&>; }
			constexpr auto operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::rend(r)
			)
			template <class R>
			requires requires { typename __rend_t<const R>; }
			constexpr auto operator()(const R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::rend((const R&&) r)
			)
		};
	}
	// Workaround GCC PR66957 by declaring this unnamed namespace inline.
	inline namespace {
		constexpr auto& crend = detail::static_const<__crend::fn>::value;
	}

	///////////////////////////////////////////////////////////////////////////
	// disable_sized_range [ranges.sized]
	//
	template <class R>
	constexpr bool disable_sized_range = false;

	///////////////////////////////////////////////////////////////////////////
	// Container access [iterator.container]
	//
	// size
	namespace __size {
		// Poison pill for std::size. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template <class R>
		void size(const R&) = delete;

		template <class R>
		constexpr bool has_member = false;
		template <class R>
		requires !disable_sized_range<R> &&
			requires(const R& r) {
				// { r.size() } -> Integral;
				requires Integral<__f<decltype(r.size())>>;
			}
		constexpr bool has_member<R> = true;

		template <class R>
		constexpr bool has_non_member = false;
		template <class R>
		requires !disable_sized_range<R> &&
			requires(const R& r) {
				// { size(r) } -> Integral;
				requires Integral<__f<decltype(size(r))>>;
			}
		constexpr bool has_non_member<R> = true;

		template <class R>
		constexpr bool has_difference = false;
		template <class R>
		requires SizedSentinel<__end_t<const R&>, __begin_t<const R&>>
		constexpr bool has_difference<R> = true;

		struct fn {
			template <class T, std::size_t N>
			constexpr std::size_t operator()(const T(&)[N]) const noexcept {
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
		requires requires(const R& r) {
				// { r.empty() } -> bool;
				requires ConvertibleTo<__f<decltype(r.empty())>, bool>;
			}
		constexpr bool has_member<R> = true;

		template <class R>
		constexpr bool has_size = false;
		template <class R>
		requires requires(const R& r) { __stl2::size(r); }
		constexpr bool has_size<R> = true;

		template <class R>
		constexpr bool has_begin_end = false;
		template <class R>
		requires requires { typename __end_t<R>; }
		constexpr bool has_begin_end<R> = true;

		struct fn {
			// Prefer member
			template <class R>
			requires has_member<R>
			constexpr bool operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				static_cast<bool>(r.empty())
			)
			// Use size
			template <class R>
			requires !has_member<R> && has_size<R>
			constexpr bool operator()(const R& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::size(r) == 0
			)
			// Use begin == end
			template <class R>
			requires !has_member<R> && !has_size<R> && has_begin_end<R>
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
		requires requires(R& r) {
				r.data();
				requires std::is_pointer<__f<decltype(r.data())>>::value;
			}
		constexpr bool has_member<R&> = true;

		template <class R>
		constexpr bool has_pointer_iterator = false;
		template <class R>
		requires std::is_pointer<__begin_t<R>>::value
		constexpr bool has_pointer_iterator<R> = true;

		template <class R>
		constexpr bool has_contiguous_iterator = false;
		template <class R>
		requires ext::ContiguousIterator<__begin_t<R>>
		constexpr bool has_contiguous_iterator<R> = true;

		struct fn {
			// Prefer member
			template <class R>
			requires has_member<R&>
			constexpr auto operator()(R& r) const
			STL2_NOEXCEPT_RETURN(
				r.data()
			)
			// Return begin(r) if it's a pointer
			template <class R>
			requires !has_member<R> && has_pointer_iterator<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::begin((R&&) r)
			)
			// Extension: Support contiguous ranges with non-pointer
			//            iterators.
			template <class R>
			requires !has_member<R> && !has_pointer_iterator<R> &&
				has_contiguous_iterator<R>
			constexpr auto operator()(R&& r) const
			noexcept(noexcept(*std::declval<__begin_t<R>&>()) &&
				noexcept(std::declval<__begin_t<R>&>() == std::declval<__end_t<R>>()))
			{
				auto i = __stl2::begin((R&&) r);
				return i == __stl2::end((R&&) r) ? nullptr : detail::addressof(*i);
			}
			// Extension: return pointer-to-mutable for string even before C++17.
			template <class CharT, class Traits>
			CharT* operator()(std::basic_string<CharT, Traits>& str) const
			noexcept(noexcept(str.data()))
			{
				return const_cast<CharT*>(str.data());
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
				requires requires(const R& r) { __stl2::data(r); }
				constexpr auto operator()(const R& r) const
				STL2_NOEXCEPT_RETURN(
					__stl2::data(r)
				)

				template <class R>
				constexpr auto operator()(const R&& r) const
				STL2_NOEXCEPT_RETURN(
					__stl2::data((const R&&) r)
				)
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
