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
#ifndef STL2_DETAIL_ITERATOR_OSTREAMBUF_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_OSTREAMBUF_ITERATOR_HPP

#include <iosfwd>
#include <string>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>

STL2_OPEN_NAMESPACE {
	namespace __ostreambuf_iterator {
		template <MoveConstructible charT, class traits>
		class cursor {
			using ostream_type = std::basic_ostream<charT, traits>;
			using streambuf_type = std::basic_streambuf<charT, traits>;
			detail::raw_ptr<streambuf_type> sbuf_ = nullptr;

		public:
			class mixin : protected detail::ebo_box<cursor> {
				using box_t = detail::ebo_box<cursor>;
			public:
				using difference_type = std::ptrdiff_t;
				using char_type = charT;
				using traits_type = traits;
				using ostream_type = cursor::ostream_type;
				using streambuf_type = cursor::streambuf_type;

				constexpr mixin() noexcept = default;
				using box_t::box_t;

				STL2_CONSTEXPR_EXT bool failed() const noexcept {
					return box_t::get().sbuf_ == nullptr;
				}
			};

			constexpr cursor() noexcept = default;
			STL2_CONSTEXPR_EXT cursor(streambuf_type* s) noexcept
			: sbuf_{s}
			{}
			cursor(ostream_type& s) noexcept
			: cursor{s.rdbuf()}
			{}
			// Extension
			constexpr cursor(default_sentinel) noexcept
			: cursor{}
			{}

			void write(charT c) {
				if (sbuf_) {
					if (traits::eq_int_type(sbuf_->sputc(__stl2::move(c)), traits::eof())) {
						sbuf_ = nullptr;
					}
				}
			}

			// Extension
			bool equal(const cursor& that) const noexcept {
				return sbuf_ == that.sbuf_;
			}
			// Extension
			bool equal(default_sentinel) const noexcept {
				return sbuf_ == nullptr;
			}
		};
	}

	// Not to spec:
	// * MoveConstructible requirement is implicit
	//   See https://github.com/ericniebler/stl2/issues/246)
	// * Extension: satisfies EqualityComparable and Sentinel<default_sentinel>
	template <MoveConstructible charT, class traits = std::char_traits<charT>>
	using ostreambuf_iterator =
		basic_iterator<__ostreambuf_iterator::cursor<charT, traits>>;
} STL2_CLOSE_NAMESPACE

#endif
