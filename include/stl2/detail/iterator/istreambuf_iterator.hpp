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
#ifndef STL2_DETAIL_ITERATOR_ISTREAMBUF_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_ISTREAMBUF_ITERATOR_HPP

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
	namespace __istreambuf_iterator {
		template <class charT, class traits = std::char_traits<charT>>
		requires
			MoveConstructible<charT>() &&
			DefaultConstructible<charT>() &&
			SignedIntegral<typename traits::off_type>()
		class cursor;
	}

	// Not to spec:
	// * requirements are implicit.
	//   See https://github.com/ericniebler/stl2/issues/246)
	//
	template <class charT, class traits = std::char_traits<charT>>
	requires
		MoveConstructible<charT>() &&
		DefaultConstructible<charT>() &&
		SignedIntegral<typename traits::off_type>()
	using istreambuf_iterator =
		basic_iterator<__istreambuf_iterator::cursor<charT, traits>>;

	namespace __istreambuf_iterator {
		template <class charT, class traits>
		requires
			MoveConstructible<charT>() &&
			DefaultConstructible<charT>() &&
			SignedIntegral<typename traits::off_type>()
		class cursor {
		public:
			using value_type = charT;
			using difference_type = typename traits::off_type;
			using streambuf_type = std::basic_streambuf<charT, traits>;
			using istream_type = std::basic_istream<charT, traits>;
			using int_type = typename traits::int_type;
			using single_pass = true_type;

			class pointer {
			public:
				constexpr charT* operator->() noexcept {
					return &keep_;
				}

			private:
				friend cursor;

				explicit constexpr pointer(const cursor& i)
				noexcept(is_nothrow_move_constructible<charT>::value)
				: keep_{i.current()}
				{}

				charT keep_;
			};

			class __proxy {
			public:
				using value_type = charT;

				constexpr charT operator*() const
				noexcept(is_nothrow_copy_constructible<charT>::value)
				{
					return keep_;
				}

			private:
				friend cursor;

				constexpr __proxy() noexcept = default;
				constexpr __proxy(charT c, streambuf_type* sbuf)
				noexcept(is_nothrow_move_constructible<charT>::value)
				: keep_{__stl2::move(c)}, sbuf_{sbuf}
				{}

				charT keep_;
				detail::raw_ptr<streambuf_type> sbuf_;
			};

			class mixin : protected detail::ebo_box<cursor> {
				using box_t = detail::ebo_box<cursor>;
			public:
				using iterator_category = input_iterator_tag;
				using value_type = cursor::value_type;
				using difference_type = cursor::difference_type;
				using reference = charT;
				using pointer = cursor::pointer;
				using char_type = charT;
				using traits_type = traits;
				using int_type = cursor::int_type;
				using streambuf_type = cursor::streambuf_type;
				using istream_type = cursor::istream_type;

				using box_t::box_t;

				// Yuck. This can't be simply "basic_iterator<cursor>".
				// Since basic_iterator<cursor> derives from mixin, mixin must be
				// instantiable before basic_iterator<cursor> is complete.
				STL2_CONSTEXPR_EXT bool
				equal(const basic_iterator<Same<cursor> >& that) const noexcept {
					return box_t::get().equal(__stl2::get_cursor(that));
				}
			};

			constexpr cursor() noexcept = default;
			constexpr cursor(default_sentinel) noexcept
			: cursor{}
			{}
			STL2_CONSTEXPR_EXT cursor(streambuf_type* s) noexcept
			: sbuf_{s}
			{}
			STL2_CONSTEXPR_EXT cursor(const __proxy& p) noexcept
			: cursor{p.sbuf_}
			{}
			cursor(istream_type& s) noexcept
			: cursor{s.rdbuf()}
			{}

			charT read() const {
				return current();
			}
			pointer arrow() const {
				return pointer{*this};
			}

			void next() {
				advance();
			}
			__proxy post_increment() {
				return {traits::to_char_type(advance()), sbuf_};
			}

			STL2_CONSTEXPR_EXT bool equal(const cursor& that) const noexcept {
				return (sbuf_ == nullptr) == (that.sbuf_ == nullptr);
			}
			STL2_CONSTEXPR_EXT bool equal(default_sentinel) const noexcept {
				return sbuf_ == nullptr;
			}

		private:
			detail::raw_ptr<streambuf_type> sbuf_ = nullptr;

			charT current() const {
				auto c = sbuf_->sgetc();
				STL2_ASSERT(!traits::eq_int_type(c, traits::eof()));
				return traits::to_char_type(__stl2::move(c));
			}

			int_type advance() {
				auto old_c = sbuf_->sbumpc();
				STL2_ASSERT(!traits::eq_int_type(old_c, traits::eof()));
				if (traits::eq_int_type(sbuf_->sgetc(), traits::eof())) {
					sbuf_ = nullptr;
				}
				return old_c;
			}
		};
	}
} STL2_CLOSE_NAMESPACE

#endif
