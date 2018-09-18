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
#ifndef STL2_DETAIL_ITERATOR_ISTREAM_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_ISTREAM_ITERATOR_HPP

#include <iosfwd>
#include <string>
#include <stl2/detail/memory/addressof.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		///////////////////////////////////////////////////////////////////////////
		// istream_cursor [Implementation detail]
		//
		template<Semiregular T, class charT = char,
			class traits = std::char_traits<charT>,
			SignedIntegral Distance = std::ptrdiff_t>
		requires
			StreamExtractable<T, charT, traits>
		class istream_cursor : semiregular_box<T> {
			using box_t = semiregular_box<T>;
		public:
			using difference_type = Distance;
			using value_type = T;
			using istream_type = std::basic_istream<charT, traits>;
			using single_pass = true_type;

			class mixin : protected basic_mixin<istream_cursor> {
				using base_t = basic_mixin<istream_cursor>;
			public:
				using iterator_category = input_iterator_tag;
				using difference_type = istream_cursor::difference_type;
				using value_type = istream_cursor::value_type;
				using reference = const T&;
				using pointer = const T*;
				using char_type = charT;
				using traits_type = traits;
				using istream_type = istream_cursor::istream_type;

				mixin() = default;
				STL2_CONSTEXPR_EXT mixin(istream_type& s)
				: base_t{istream_cursor{s}}
				{}
				constexpr mixin(default_sentinel)
				: base_t{}
				{}
				using base_t::base_t;
			};

			constexpr istream_cursor()
			noexcept(is_nothrow_default_constructible<T>::value) = default;

			STL2_CONSTEXPR_EXT istream_cursor(istream_type& s)
			: stream_{detail::addressof(s)}
			{ next(); }

			constexpr istream_cursor(default_sentinel)
			noexcept(is_nothrow_default_constructible<T>::value)
			: istream_cursor{}
			{}

			STL2_CONSTEXPR_EXT const T& read() const noexcept {
				return box_t::get();
			}

			STL2_CONSTEXPR_EXT void next() {
				*stream_ >> box_t::get();
				if (!*stream_) {
					stream_ = nullptr;
				}
			}
			STL2_CONSTEXPR_EXT istream_cursor post_increment() {
				auto tmp = *this;
				next();
				return tmp;
			}

			STL2_CONSTEXPR_EXT bool equal(const istream_cursor& that) const noexcept {
				return stream_ == that.stream_;
			}

			STL2_CONSTEXPR_EXT bool equal(default_sentinel) const noexcept {
				return stream_ == nullptr;
			}

		private:
			raw_ptr<istream_type> stream_ = nullptr;
		};
	}

	///////////////////////////////////////////////////////////////////////////
	// istream_iterator [iterator.istream]
	//
	template<Semiregular T, class charT = char, class traits = std::char_traits<charT>,
		SignedIntegral Distance = std::ptrdiff_t>
	requires
		StreamExtractable<T, charT, traits>
	using istream_iterator =
		basic_iterator<detail::istream_cursor<T, charT, traits, Distance>>;
} STL2_CLOSE_NAMESPACE

#endif
