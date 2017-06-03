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
#include <string>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/memory/addressof.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// ostream_iterator [ostream.iterator]
	// Extension: ostream_iterator<void> accepts any streamable type.
	// Not to spec:
	// * StreamInsertable constraint is implicit
	//   See https://github.com/ericniebler/stl2/issues/246
	//
	template <class T = void, class charT = char, class traits = std::char_traits<charT>>
	requires
		Same<T, void> ||
		ext::StreamInsertable<T, std::basic_ostream<charT, traits>>
	class ostream_iterator {
	public:
		using difference_type = ptrdiff_t;
		using char_type = charT;
		using traits_type = traits;
		using ostream_type = std::basic_ostream<charT, traits>;

		constexpr ostream_iterator() noexcept = default;

		STL2_CONSTEXPR_EXT ostream_iterator(
			ostream_type& os, const charT* delimiter = nullptr) noexcept
		: out_stream_(__stl2::addressof(os)), delim_(delimiter) {}

		template <class U, class V = meta::if_<std::is_void<T>, U, T>>
		requires
			ConvertibleTo<U, V const&> &&
			ext::StreamInsertable<V, ostream_type>
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
		// Not to spec:
		// https://github.com/ericniebler/stl2/issues/232
		ostream_iterator& operator++(int) noexcept {
			return *this;
		}
	private:
		detail::raw_ptr<basic_ostream<charT, traits>> out_stream_{nullptr};
		const charT* delim_{nullptr};
	};

} STL2_CLOSE_NAMESPACE

#endif
