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
	// Not to spec:
	// * Extension: satisfies EqualityComparable and Sentinel<default_sentinel>
	template <class charT, class traits = std::char_traits<charT>>
	class ostreambuf_iterator {
	public:
		using difference_type = ptrdiff_t;
		using char_type = charT;
		using traits_type = traits;
		using ostream_type = std::basic_ostream<charT, traits>;
		using streambuf_type = std::basic_streambuf<charT, traits>;

		constexpr ostreambuf_iterator() noexcept = default;
		constexpr ostreambuf_iterator(default_sentinel) noexcept
		: ostreambuf_iterator{}
		{}
		STL2_CONSTEXPR_EXT ostreambuf_iterator(ostream_type& s) noexcept
		: sbuf_(s.rdbuf()) {}
		STL2_CONSTEXPR_EXT ostreambuf_iterator(streambuf_type* s) noexcept
		: sbuf_(s) {}
		ostreambuf_iterator& operator=(charT c) {
			if (sbuf_) {
				if (traits::eq_int_type(sbuf_->sputc(std::move(c)), traits::eof())) {
					sbuf_ = nullptr;
				}
			}
			return *this;
		}
		ostreambuf_iterator& operator*() noexcept {
			return *this;
		}
		ostreambuf_iterator& operator++() noexcept {
			return *this;
		}
		ostreambuf_iterator& operator++(int) noexcept {
			return *this;
		}
		bool failed() const noexcept {
			return sbuf_ != nullptr;
		}
		friend bool operator==(ostreambuf_iterator a, ostreambuf_iterator b) noexcept {
			return a.sbuf_ == b.sbuf_;
		}
		friend bool operator!=(ostreambuf_iterator a, ostreambuf_iterator b) noexcept {
			return !(a == b);
		}
		friend bool operator==(ostreambuf_iterator a, default_sentinel) noexcept {
			return a.sbuf_ == nullptr;
		}
		friend bool operator!=(ostreambuf_iterator a, default_sentinel b) noexcept {
			return !(a == b);
		}
		friend bool operator==(default_sentinel, ostreambuf_iterator b) noexcept {
			return b.sbuf_ == nullptr;
		}
		friend bool operator!=(default_sentinel a, ostreambuf_iterator b) noexcept {
			return !(a == b);
		}
	private:
		detail::raw_ptr<streambuf_type> sbuf_{nullptr};
	};
} STL2_CLOSE_NAMESPACE

#endif
