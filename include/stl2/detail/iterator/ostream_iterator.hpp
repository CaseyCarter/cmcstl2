// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_OSTREAM_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_OSTREAM_ITERATOR_HPP

#include <iosfwd>
#include <memory>
#include <string>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// ostream_iterator [ostream.iterator]
	// Extension: ostream_iterator<void> accepts any streamable type.
	//
	template<class T = void, class charT = char, class traits = std::char_traits<charT>>
	requires
		Same<T, void> ||
		StreamInsertable<T, charT, traits>
	class ostream_iterator {
	public:
		using difference_type = std::ptrdiff_t;
		using char_type = charT;
		using traits_type = traits;
		using ostream_type = std::basic_ostream<charT, traits>;

		constexpr ostream_iterator() noexcept = default;

		ostream_iterator(
			ostream_type& os, const charT* delimiter = nullptr) noexcept
		: out_stream_(std::addressof(os)), delim_(delimiter) {}

		template<class U, class V = meta::if_<std::is_void<T>, U, T>>
		requires
			ConvertibleTo<U, V const&> &&
			StreamInsertable<V, charT, traits>
		ostream_iterator& operator=(U&& u) {
			*out_stream_ << static_cast<V const &>(std::forward<U>(u));
			if (delim_ != nullptr) {
				*out_stream_ << delim_;
			}
			return *this;
		}

		ostream_iterator& operator*() noexcept {
			return *this;
		}
		ostream_iterator& operator++() noexcept {
			return *this;
		}
		ostream_iterator& operator++(int) noexcept {
			return *this;
		}
	private:
		detail::raw_ptr<std::basic_ostream<charT, traits>> out_stream_{nullptr};
		const charT* delim_{nullptr};
	};

} STL2_CLOSE_NAMESPACE

#endif
