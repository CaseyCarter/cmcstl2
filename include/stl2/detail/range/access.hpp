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
#include <string_view>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/memory/addressof.hpp>

///////////////////////////////////////////////////////////////////////////
// Range access [range.access]
//
STL2_OPEN_NAMESPACE {
	template<class T>
	requires requires(T&& t) { static_cast<std::decay_t<T>>(static_cast<T&&>(t)); }
	constexpr std::decay_t<T> __decay_copy(T&& t)
	STL2_NOEXCEPT_RETURN(
		static_cast<std::decay_t<T>>(static_cast<T&&>(t))
	)

	// begin
	namespace __begin {
		// Poison pill for std::begin. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template <class T>
		void begin(T&&) = delete;

		template <class T>
		void begin(std::initializer_list<T>&&) = delete;

		template <class R>
		concept bool has_member = _Is<R, std::is_lvalue_reference> && requires(R r) {
			r.begin();
			{ __decay_copy(r.begin()) } -> Iterator;
		};

		template <class R>
		concept bool has_non_member = requires(R&& r) {
			begin(static_cast<R&&>(r));
			{ __decay_copy(begin(static_cast<R&&>(r))) } -> Iterator;
		};

		struct __fn {
			// Handle builtin arrays directly
			template <class R, std::size_t N>
			void operator()(R (&&)[N]) const = delete;

			template <class R, std::size_t N>
			constexpr R* operator()(R (&array)[N]) const noexcept {
				return array;
			}

			// Handle basic_string_view directly to implement P0970 non-intrusively
			template <class CharT, class Traits>
			constexpr auto operator()(const std::basic_string_view<CharT, Traits>& sv) const noexcept {
				return sv.begin();
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
				begin(static_cast<R&&>(r))
			)
		};
	}
	inline constexpr __begin::__fn begin{};

	template <class R>
	using __begin_t = decltype(__stl2::begin(std::declval<R>()));

	// end
	namespace __end {
		// Poison pill for std::end. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template <class T>
		void end(T&&) = delete;

		template <class T>
		void end(std::initializer_list<T>&&) = delete;

		template <class R>
		concept bool has_member = _Is<R, std::is_lvalue_reference> && requires(R r) {
			typename __begin_t<R>;
			r.end();
			{ __decay_copy(r.end()) } -> Sentinel<__begin_t<R>>;
		};

		template <class R>
		concept bool has_non_member = requires(R&& r) {
			typename __begin_t<R>;
			end(static_cast<R&&>(r));
			{ __decay_copy(end(static_cast<R&&>(r))) } -> Sentinel<__begin_t<R>>;
		};

		struct __fn {
			// Handle builtin arrays directly
			template <class R, std::size_t N>
			void operator()(R (&&)[N]) const = delete;

			template <class R, std::size_t N>
			constexpr R* operator()(R (&array)[N]) const noexcept {
				return array + N;
			}

			// Handle basic_string_view directly to implement P0970 non-intrusively
			template <class CharT, class Traits>
			constexpr auto operator()(const std::basic_string_view<CharT, Traits>& sv) const noexcept {
				return sv.end();
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
				end(static_cast<R&&>(r))
			)
		};
	}
	inline constexpr __end::__fn end{};

	template <class R>
	using __end_t = decltype(__stl2::end(std::declval<R>()));

	// cbegin
	struct __cbegin_fn {
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
			__stl2::begin(static_cast<const R&&>(r))
		)
	};
	inline constexpr __cbegin_fn cbegin{};

	// cend
	struct __cend_fn {
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
			__stl2::end(static_cast<const R&&>(r))
		)
	};
	inline constexpr __cend_fn cend{};

	// rbegin
	namespace __rbegin {
		// Poison pill
		template <class T>
		void rbegin(T&&) = delete;

		// Prefer member if it returns Iterator
		template <class R>
		concept bool has_member = _Is<R, std::is_lvalue_reference> && requires(R r) {
			r.begin();
			{ __decay_copy(r.rbegin()) } -> Iterator;
		};

		template <class R>
		concept bool has_non_member = requires(R&& r) {
			rbegin(static_cast<R&&>(r));
			{ __decay_copy(rbegin(static_cast<R&&>(r))) } -> Iterator;
		};

		// Default to make_reverse_iterator(end(r)) for Common ranges of
		// Bidirectional iterators.
		template <class R>
		concept bool can_make_reverse = requires(R&& r) {
			typename __begin_t<R>;
			typename __end_t<R>;
			requires Same<__begin_t<R>, __end_t<R>>;
			requires BidirectionalIterator<__end_t<R>>;
		};

		struct __fn {
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
				rbegin(static_cast<R&&>(r))
			)
			template <class R>
			requires !has_member<R> && !has_non_member<R> && can_make_reverse<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::make_reverse_iterator(__stl2::end(static_cast<R&&>(r)))
			)
		};
	}
	inline constexpr __rbegin::__fn rbegin{};

	template <class T>
	using __rbegin_t = decltype(__stl2::rbegin(std::declval<T>()));

	// rend
	namespace __rend {
		// poison-pill overload, not to spec
		template <class R>
		void rend(R&&) = delete;

		template <class R>
		concept bool has_member = _Is<R, std::is_lvalue_reference> && requires(R r) {
			typename __rbegin_t<R>;
			r.rend();
			{ __decay_copy(r.rend()) } -> Sentinel<__rbegin_t<R>>;
		};

		template <class R>
		concept bool has_non_member = requires(R&& r) {
			typename __rbegin_t<R>;
			rend(static_cast<R&&>(r));
			{ __decay_copy(rend(static_cast<R&&>(r))) } -> Sentinel<__rbegin_t<R>>;
		};

		template <class R>
		concept bool can_make_reverse = requires(R&& r) {
			typename __begin_t<R>;
			typename __end_t<R>;
			requires Same<__begin_t<R>, __end_t<R>>;
			__stl2::make_reverse_iterator(__stl2::end(static_cast<R&&>(r)));
		};

		struct __fn {
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
				rend(static_cast<R&&>(r))
			)
			// Default to make_reverse_iterator(begin(r)) for Common ranges of
			// Bidirectional iterators.
			template <class R>
			requires !has_member<R> && !has_non_member<R> && can_make_reverse<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::make_reverse_iterator(__stl2::begin(static_cast<R&&>(r)))
			)
		};
	}
	inline constexpr __rend::__fn rend{};

	template <class T>
	using __rend_t = decltype(__stl2::rend(std::declval<T>()));

	// crbegin
	struct __crbegin_fn {
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
			__stl2::rbegin(static_cast<const R&&>(r))
		)
	};
	inline constexpr __crbegin_fn crbegin{};

	// crend
	struct __crend_fn {
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
			__stl2::rend(static_cast<const R&&>(r))
		)
	};
	inline constexpr __crend_fn crend{};

	///////////////////////////////////////////////////////////////////////////
	// disable_sized_range [range.sized]
	//
	template <class R>
	inline constexpr bool disable_sized_range = false;

	///////////////////////////////////////////////////////////////////////////
	// Range primitives [range.primitives]
	//
	// size
	namespace __size {
		// Poison pill for std::size. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template <class T>
		void size(T&&) = delete;

		template <class R>
		concept bool has_member = !disable_sized_range<__uncvref<R>> &&
			requires(R&& r) {
				static_cast<R&&>(r).size();
				{ __decay_copy(static_cast<R&&>(r).size()) } -> Integral;
			};

		template <class R>
		concept bool has_non_member = !disable_sized_range<__uncvref<R>> &&
			requires(R&& r) {
				size(static_cast<R&&>(r));
				{ __decay_copy(size(static_cast<R&&>(r))) } -> Integral;
			};

		template <class R>
		concept bool has_difference = requires {
			typename __begin_t<R>;
			typename __end_t<R>;
		} &&
		ForwardIterator<__begin_t<R>> &&
		SizedSentinel<__end_t<R>, __begin_t<R>>;

		struct __fn {
			template <class T, std::size_t N>
			constexpr std::size_t operator()(T(&)[N]) const noexcept {
				return N;
			}
			template <class T, std::size_t N>
			constexpr std::size_t operator()(T(&&)[N]) const noexcept {
				return N;
			}
			// Prefer member
			template <class R>
			requires has_member<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				static_cast<R&&>(r).size()
			)
			// Use non-member
			template <class R>
			requires !has_member<R> && has_non_member<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				size(static_cast<R&&>(r))
			)
			template <class R>
			requires !has_member<R> && !has_non_member<R> && has_difference<R>
			constexpr auto operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::end(static_cast<R&&>(r)) - __stl2::begin(static_cast<R&&>(r))
			)
		};
	}
	inline constexpr __size::__fn size{};

	// empty
	namespace __empty {
		template <class R>
		concept bool has_member = requires(R&& r) {
			static_cast<R&&>(r).empty();
			static_cast<bool>(static_cast<R&&>(r).empty());
		};

		template <class R>
		concept bool has_size = requires(R&& r) {
			__stl2::size(static_cast<R&&>(r));
		};

		template <class R>
		concept bool has_begin_end = requires {
			typename __begin_t<R>;
			typename __end_t<R>;
			requires ForwardIterator<__begin_t<R>>;
		};

		struct __fn {
			// Prefer member
			template <class R>
			requires has_member<R>
			constexpr bool operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				static_cast<bool>(static_cast<R&&>(r).empty())
			)
			// Use size
			template <class R>
			requires !has_member<R> && has_size<R>
			constexpr bool operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::size(static_cast<R&&>(r)) == 0
			)
			// Use begin == end
			template <class R>
			requires !has_member<R> && !has_size<R> && has_begin_end<R>
			constexpr bool operator()(R&& r) const
			STL2_NOEXCEPT_RETURN(
				__stl2::begin(static_cast<R&&>(r)) == __stl2::end(static_cast<R&&>(r))
			)
		};
	}
	inline constexpr __empty::__fn empty{};

	// data
	namespace __data {
		template <class P>
		inline constexpr bool is_object_ptr = false;
		template <_Is<std::is_object> T>
		inline constexpr bool is_object_ptr<T *> = true;

		template <class R>
		concept bool has_member = _Is<R, std::is_lvalue_reference> && requires(R r) {
			r.data();
			__decay_copy(r.data());
			requires is_object_ptr<decltype(__decay_copy(r.data()))>;
		};

		template <class R>
		concept bool has_pointer_iterator = requires {
			typename __begin_t<R>;
			requires is_object_ptr<__begin_t<R>>;
		};

		template <class R>
		concept bool has_contiguous_iterator = requires {
			typename __begin_t<R>;
			typename __end_t<R>;
			requires ext::ContiguousIterator<__begin_t<R>>;
		};

		struct __fn {
			template <class I>
			static I& __idref(I) noexcept;
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
				__stl2::begin(static_cast<R&&>(r))
			)
			// If the range is contiguous, convert begin() to a pointer
			template <class R>
			requires !has_member<R> && !has_pointer_iterator<R> &&
				has_contiguous_iterator<R>
			constexpr auto operator()(R&& r) const
			noexcept(noexcept((__idref)(__stl2::begin(static_cast<R&&>(r))) == __stl2::end(static_cast<R&&>(r))
				? nullptr : *(__idref)(__stl2::begin(static_cast<R&&>(r)))))
			{
				auto i = __stl2::begin(static_cast<R&&>(r));
				return i == __stl2::end(static_cast<R&&>(r)) ? nullptr : detail::addressof(*i);
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
	inline constexpr __data::__fn data{};

	// cdata
	struct __cdata_fn {
		template <class R>
		requires requires(const R& r) { __stl2::data(r); }
		constexpr auto operator()(const R& r) const
		STL2_NOEXCEPT_RETURN(
			__stl2::data(r)
		)

		template <class R>
		requires requires(const R&& r) { __stl2::data(static_cast<const R&&>(r)); }
		constexpr auto operator()(const R&& r) const
		STL2_NOEXCEPT_RETURN(
			__stl2::data(static_cast<const R&&>(r))
		)
	};
	inline constexpr __cdata_fn cdata{};
} STL2_CLOSE_NAMESPACE

#endif
