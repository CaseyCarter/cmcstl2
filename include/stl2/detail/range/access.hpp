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
#include <memory>
#include <string>
#include <string_view>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// range access [range.access]
//
STL2_OPEN_NAMESPACE {
	inline constexpr struct __as_const_fn {
		template<class T>
		constexpr const T& operator()(T& t) const noexcept {
			return t;
		}
		template<class T>
		constexpr const T&& operator()(T&& t) const noexcept {
			return (T&&)t;
		}
	} __as_const {};

	// begin
	namespace __begin {
		// Poison pill for std::begin. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template<class T> void begin(T&&) = delete;

		template<class T>
		void begin(std::initializer_list<T>) = delete; // TODO: file LWG issue

		template<class R>
		META_CONCEPT has_member = std::is_lvalue_reference_v<R> &&
			requires(R& r) {
				r.begin();
				{ __decay_copy(r.begin()) } -> input_or_output_iterator;
			};

		template<class R>
		META_CONCEPT has_non_member = requires(R&& r) {
			begin(static_cast<R&&>(r));
			{ __decay_copy(begin(static_cast<R&&>(r))) } -> input_or_output_iterator;
		};

		template<class>
		inline constexpr bool nothrow = false;
		template<has_member R>
		inline constexpr bool nothrow<R> = noexcept(std::declval<R&>().begin());
		template<class R>
		requires (!has_member<R> && has_non_member<R>)
		inline constexpr bool nothrow<R> = noexcept(begin(std::declval<R>()));

		struct __fn {
			// Handle builtin arrays directly
			template<class R, std::size_t N>
			void operator()(R (&&)[N]) const = delete;

			template<class R, std::size_t N>
			constexpr R* operator()(R (&array)[N]) const noexcept {
				return array;
			}

			// Handle basic_string_view directly to implement P0970 non-intrusively
			template<class CharT, class Traits>
			constexpr auto operator()(
				std::basic_string_view<CharT, Traits> sv) const noexcept {
				return sv.begin();
			}

			template<class R>
			requires has_member<R> || has_non_member<R>
			constexpr auto operator()(R&& r) const noexcept(nothrow<R>) {
				if constexpr (has_member<R>) {
					return r.begin();
				} else {
					return begin(static_cast<R&&>(r));
				}
			}
		};
	}
	inline namespace __cpos {
		inline constexpr __begin::__fn begin{};
	}

	template<class R>
	using __begin_t = decltype(begin(std::declval<R>()));

	// end
	namespace __end {
		// Poison pill for std::end. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template<class T> void end(T&&) = delete;

		template<class T>
		void end(std::initializer_list<T>) = delete; // TODO: file LWG issue

		template<class R>
		META_CONCEPT has_member = std::is_lvalue_reference_v<R> &&
			requires(R& r) {
				r.end();
				begin(r);
				{ __decay_copy(r.end()) } -> sentinel_for<__begin_t<R>>;
			};

		template<class R>
		META_CONCEPT has_non_member = requires(R&& r) {
			end(static_cast<R&&>(r));
			begin(static_cast<R&&>(r));
			{ __decay_copy(end(static_cast<R&&>(r))) } -> sentinel_for<__begin_t<R>>;
		};

		template<class>
		inline constexpr bool nothrow = false;
		template<has_member R>
		inline constexpr bool nothrow<R> = noexcept(std::declval<R&>().end());
		template<class R>
		requires (!has_member<R> && has_non_member<R>)
		inline constexpr bool nothrow<R> = noexcept(end(std::declval<R>()));

		struct __fn {
			// Handle builtin arrays directly
			template<class R, std::size_t N>
			void operator()(R (&&)[N]) const = delete;

			template<class R, std::size_t N>
			constexpr R* operator()(R (&array)[N]) const noexcept {
				return array + N;
			}

			// Handle basic_string_view directly to implement P0970 non-intrusively
			template<class CharT, class Traits>
			constexpr auto operator()(
				std::basic_string_view<CharT, Traits> sv) const noexcept {
				return sv.end();
			}

			template<class R>
			requires has_member<R> || has_non_member<R>
			constexpr auto operator()(R&& r) const noexcept(nothrow<R>) {
				if constexpr (has_member<R>)
					return r.end();
				else
					return end(static_cast<R&&>(r));
			}
		};
	}
	inline namespace __cpos {
		inline constexpr __end::__fn end{};
	}

	template<class R>
	using __end_t = decltype(end(std::declval<R>()));

	// cbegin
	struct __cbegin_fn {
		template<class R>
		constexpr auto operator()(R&& r) const
#if STL2_WORKAROUND_CLANGC_50
		noexcept(noexcept(begin(__as_const(static_cast<R&&>(r)))))
		requires requires(R&& r) { begin(__as_const(static_cast<R&&>(r))); } {
			return begin(__as_const(static_cast<R&&>(r)));
		}
#else // ^^^ workaround / no workaround vvv
		STL2_NOEXCEPT_REQUIRES_RETURN(
			begin(__as_const(static_cast<R&&>(r)))
		)
#endif // STL2_WORKAROUND_CLANGC_50
	};
	inline namespace __cpos {
		inline constexpr __cbegin_fn cbegin{};
	}

	// cend
	struct __cend_fn {
		template<class R>
		constexpr auto operator()(R&& r) const
#if STL2_WORKAROUND_CLANGC_50
		noexcept(noexcept(end(__as_const(static_cast<R&&>(r)))))
		requires requires(R&& r) { end(__as_const(static_cast<R&&>(r))); } {
			return end(__as_const(static_cast<R&&>(r)));
		}
#else // ^^^ workaround / no workaround vvv
		STL2_NOEXCEPT_REQUIRES_RETURN(
			end(__as_const(static_cast<R&&>(r)))
		)
#endif // STL2_WORKAROUND_CLANGC_50
	};
	inline namespace __cpos {
		inline constexpr __cend_fn cend{};
	}

	// rbegin
	namespace __rbegin {
		// Poison pill
		template<class T> void rbegin(T&&) = delete;

		// Prefer member if it returns input_or_output_iterator
		template<class R>
		META_CONCEPT has_member = std::is_lvalue_reference_v<R> &&
			requires(R& r) {
				r.rbegin();
				{ __decay_copy(r.rbegin()) } -> input_or_output_iterator;
			};

		template<class R>
		META_CONCEPT has_non_member = requires(R&& r) {
			rbegin(static_cast<R&&>(r));
			{ __decay_copy(rbegin(static_cast<R&&>(r))) } -> input_or_output_iterator;
		};

		// Default to make_reverse_iterator(end(r)) for common_with ranges of
		// Bidirectional iterators.
		template<class R>
		META_CONCEPT can_make_reverse = requires(R&& r) {
			{ begin(static_cast<R&&>(r)) } -> bidirectional_iterator;
			{ end(static_cast<R&&>(r)) } -> same_as<__begin_t<R>>;
		};

		template<class>
		inline constexpr bool nothrow = false;
		template<has_member R>
		inline constexpr bool nothrow<R> = noexcept(std::declval<R&>().rbegin());
		template<class R>
		requires (!has_member<R> && has_non_member<R>)
		inline constexpr bool nothrow<R> = noexcept(rbegin(std::declval<R>()));
		template<class R>
		requires (!has_member<R> && !has_non_member<R> && can_make_reverse<R>)
		inline constexpr bool nothrow<R> =
			noexcept(__stl2::make_reverse_iterator(end(std::declval<R>())));

		struct __fn {
			// Consider the case of a hypothetical reverse_subrange whose
			// begin/end members pass its stored iterators through
			// make_reverse_iterator. Like subrange, it is only a pair of
			// iterators, and so rbegin/rend make sense on an rvalue
			// reverse_subrange.
			template<class R>
			requires has_member<R> || has_non_member<R> || can_make_reverse<R>
			constexpr auto operator()(R&& r) const noexcept(nothrow<R>) {
				if constexpr (has_member<R>)
					return r.rbegin();
				else if constexpr (has_non_member<R>)
					return rbegin(static_cast<R&&>(r));
				else
					return __stl2::make_reverse_iterator(end(static_cast<R&&>(r)));
			}
		};
	}
	inline namespace __cpos {
		inline constexpr __rbegin::__fn rbegin{};
	}

	template<class T>
	using __rbegin_t = decltype(rbegin(std::declval<T>()));

	// rend
	namespace __rend {
		// Poison pill
		template<class R> void rend(R&&) = delete;

		template<class R>
		META_CONCEPT has_member = std::is_lvalue_reference_v<R> &&
			requires(R& r) {
				rbegin(r);
				r.rend();
				{ __decay_copy(r.rend()) } -> sentinel_for<__rbegin_t<R>>;
			};

		template<class R>
		META_CONCEPT has_non_member = requires(R&& r) {
			rbegin(static_cast<R&&>(r));
			rend(static_cast<R&&>(r));
			{ __decay_copy(rend(static_cast<R&&>(r))) } ->
				sentinel_for<__rbegin_t<R>>;
		};

		using __stl2::__rbegin::can_make_reverse;

		template<class>
		inline constexpr bool nothrow = false;
		template<has_member R>
		inline constexpr bool nothrow<R> = noexcept(std::declval<R&>().rend());
		template<class R>
		requires (!has_member<R> && has_non_member<R>)
		inline constexpr bool nothrow<R> = noexcept(rend(std::declval<R>()));
		template<class R>
		requires (!has_member<R> && !has_non_member<R> && can_make_reverse<R>)
		inline constexpr bool nothrow<R> =
			noexcept(__stl2::make_reverse_iterator(begin(std::declval<R>())));

		struct __fn {
			template<class R>
			requires has_member<R> || has_non_member<R> || can_make_reverse<R>
			constexpr auto operator()(R&& r) const noexcept(nothrow<R>) {
				if constexpr (has_member<R>)
					return r.rend();
				else if constexpr (has_non_member<R>)
					return rend(static_cast<R&&>(r));
				else
					return __stl2::make_reverse_iterator(begin(static_cast<R&&>(r)));
			}
		};
	}
	inline namespace __cpos {
		inline constexpr __rend::__fn rend{};
	}

	template<class T>
	using __rend_t = decltype(rend(std::declval<T>()));

	// crbegin
	struct __crbegin_fn {
		template<class R>
		constexpr auto operator()(R&& r) const
#if STL2_WORKAROUND_CLANGC_50
		noexcept(noexcept(rbegin(__as_const(static_cast<R&&>(r)))))
		requires requires(R&& r) { rbegin(__as_const(static_cast<R&&>(r))); } {
			return rbegin(__as_const(static_cast<R&&>(r)));
		}
#else // ^^^ workaround / no workaround vvv
		STL2_NOEXCEPT_REQUIRES_RETURN(
			rbegin(__as_const(static_cast<R&&>(r)))
		)
#endif // STL2_WORKAROUND_CLANGC_50
	};
	inline namespace __cpos {
		inline constexpr __crbegin_fn crbegin{};
	}

	// crend
	struct __crend_fn {
		template<class R>
		constexpr auto operator()(R&& r) const
#if STL2_WORKAROUND_CLANGC_50
		noexcept(noexcept(rend(__as_const(static_cast<R&&>(r)))))
		requires requires(R&& r) { rend(__as_const(static_cast<R&&>(r))); } {
			return rend(__as_const(static_cast<R&&>(r)));
		}
#else // ^^^ workaround / no workaround vvv
		STL2_NOEXCEPT_REQUIRES_RETURN(
			rend(__as_const(static_cast<R&&>(r)))
		)
#endif // STL2_WORKAROUND_CLANGC_50
	};
	inline namespace __cpos {
		inline constexpr __crend_fn crend{};
	}

	// disable_sized_range [range.sized]
	template<class>
	inline constexpr bool disable_sized_range = false;

	// size [range.prim.size]
	// Not to spec: lvalue treatment
	// TODO: LWG issue
	namespace __size {
		// Poison pill for std::size. (See the detailed discussion at
		// https://github.com/ericniebler/stl2/issues/139)
		template<class T> void size(T&&) = delete;

		template<class R>
		META_CONCEPT has_member = requires(R& r) {
			r.size();
			{ __decay_copy(r.size()) } -> integral;
		};

		template<class R>
		META_CONCEPT has_non_member = requires(R& r) {
			size(r);
			{ __decay_copy(size(r)) } -> integral;
		};

		template<class R>
		META_CONCEPT has_difference = requires(R& r) {
			{ begin(r) } -> forward_iterator;
			{ end(r) } -> sized_sentinel_for<__begin_t<R&>>;
		};

		template<class T>
		inline constexpr bool nothrow = std::is_array_v<T>;
		template<class T>
		requires (!disable_sized_range<std::remove_cv_t<T>> && has_member<T>)
		inline constexpr bool nothrow<T> = noexcept(std::declval<T&>().size());
		template<class T>
		requires (!disable_sized_range<std::remove_cv_t<T>>
			&& !has_member<T> && has_non_member<T>)
		inline constexpr bool nothrow<T> = noexcept(size(std::declval<T&>()));
		template<class T>
		requires (disable_sized_range<std::remove_cv_t<T>>
			|| (!has_member<T> && !has_non_member<T>)) && has_difference<T>
		inline constexpr bool nothrow<T> =
			noexcept(end(std::declval<T&>()) - begin(std::declval<T&>()));

		struct __fn {
			template<class R, class T = std::remove_reference_t<R>,
				class U = std::remove_cv_t<T>>
			requires std::is_array_v<T>
				|| (!disable_sized_range<U>
					&& (has_member<T> || has_non_member<T>))
				|| has_difference<T>
			constexpr auto operator()(R&& r) const noexcept(nothrow<T>) {
				if constexpr (std::is_array_v<T>)
					return std::extent_v<T>;
				else if constexpr (!disable_sized_range<U> && has_member<T>)
					return r.size();
				else if constexpr (!disable_sized_range<U> && has_non_member<T>)
					return size(r);
				else // has_difference<T>
					return end(r) - begin(r);
			}
		};
	}
	inline namespace __cpos {
		inline constexpr __size::__fn size{};
	}

	// empty
	// Not to spec: lvalue treatment
	// TODO: LWG issue
	namespace __empty {
		template<class R>
		META_CONCEPT has_member = requires(R& r) {
			r.empty();
			bool(r.empty());
		};

		template<class R>
		META_CONCEPT has_size = requires(R& r) {
			size(r);
		};

		template<class R>
		META_CONCEPT has_begin_end = requires(R& r) {
			{ begin(r) } -> forward_iterator;
			end(r);
		};

		template<class R>
		inline constexpr bool nothrow = false;
		template<has_member R>
		inline constexpr bool nothrow<R> =
			noexcept(bool(std::declval<R&>().empty()));
		template<class R>
		requires (!has_member<R> && has_size<R>)
		inline constexpr bool nothrow<R> =
			noexcept(size(std::declval<R&>()) == 0);
		template<class R>
		requires (!has_member<R> && !has_size<R> && has_begin_end<R>)
		inline constexpr bool nothrow<R> =
			noexcept(begin(std::declval<R&>()) == end(std::declval<R&>()));

		struct __fn {
			template<class R>
			requires has_member<R&> || has_size<R&> || has_begin_end<R&>
			constexpr bool operator()(R&& r) const noexcept(nothrow<R&>) {
				if constexpr (has_member<R&>)
					return bool(r.empty());
				else if constexpr (has_size<R&>)
					return size(r) == 0;
				else // has_begin_end<R&>
					return begin(r) == end(r);
			}
		};
	}
	inline namespace __cpos {
		inline constexpr __empty::__fn empty{};
	}

	// data
	// Not to spec:
	// * lvalue treatment
	// * calls member data() for rvalue forwarding-ranges
	// TODO: Needs LWG issue
	namespace __data {
		template<class>
		inline constexpr bool is_object_ptr = false;
		template<class T>
		requires std::is_object_v<T>
		inline constexpr bool is_object_ptr<T*> = true;

		template<class R>
		META_CONCEPT forwarding_range_hack = requires(R&& r) {
			end(static_cast<R&&>(r));
		};

		template<class R>
		META_CONCEPT has_member =
			requires(R& r) {
				r.data();
				__decay_copy(r.data());
				requires is_object_ptr<decltype(__decay_copy(r.data()))>;
			} &&
			(std::is_lvalue_reference_v<R> || forwarding_range_hack<R>);

		template<class R>
		META_CONCEPT has_contiguous_iterator = requires(R&& r) {
			{ begin(static_cast<R&&>(r)) } -> contiguous_iterator;
			end(static_cast<R&&>(r));
		};

		template<class I>
		I& __idref(I) noexcept; // not defined

		template<class>
		inline constexpr bool nothrow = false;
		template<has_member R>
		inline constexpr bool nothrow<R> = noexcept(std::declval<R&>().data());
		template<class R>
		requires (!has_member<R> && has_contiguous_iterator<R>)
		inline constexpr bool nothrow<R> = noexcept(
			*((__idref)(begin(std::declval<R&>())) == end(std::declval<R&>())
				? nullptr : (__idref)(begin(std::declval<R&>()))));

		struct __fn {
			template<class R>
			requires has_member<R> || has_contiguous_iterator<R>
			constexpr auto operator()(R&& r) const noexcept(nothrow<R>) {
				if constexpr (has_member<R>)
					return r.data();
				else { // has_contiguous_iterator<R>
					auto first = begin(r);
					return first == end(r) ? nullptr : std::addressof(*first);
				}
			}

			// Extension: return pointer-to-mutable for string even before C++17.
			template<class CharT, class Traits>
			CharT* operator()(std::basic_string<CharT, Traits>& str) const noexcept {
				return const_cast<CharT*>(str.data());
			}
		};
	}
	inline namespace __cpos {
		inline constexpr __data::__fn data{};
	}

	// cdata
	struct __cdata_fn {
		template<class R>
		constexpr auto operator()(R&& r) const
#if STL2_WORKAROUND_CLANGC_50
		noexcept(noexcept(data(__as_const(static_cast<R&&>(r)))))
		requires requires(R&& r) { data(__as_const(static_cast<R&&>(r))); } {
			return data(__as_const(static_cast<R&&>(r)));
		}
#else // ^^^ workaround / no workaround vvv
		STL2_NOEXCEPT_REQUIRES_RETURN(
			data(__as_const(static_cast<R&&>(r)))
		)
#endif // STL2_WORKAROUND_CLANGC_50
	};
	inline namespace __cpos {
		inline constexpr __cdata_fn cdata{};
	}
} STL2_CLOSE_NAMESPACE

#endif
