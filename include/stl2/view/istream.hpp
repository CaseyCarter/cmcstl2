// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2013-present
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_ISTREAM_HPP
#define STL2_VIEW_ISTREAM_HPP

#include <istream>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/memory/addressof.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<Semiregular Val>
		requires StreamExtractable<Val>
		struct istream_view : view_interface<istream_view<Val>> {
		private:
			struct __iterator;

			detail::raw_ptr<std::istream> sin_ = nullptr;
			Val obj_ {};

			void next_()
			{ *sin_ >> obj_; }
		public:
			istream_view() = default;
			explicit constexpr istream_view(std::istream& sin)
			noexcept(std::is_nothrow_default_constructible_v<Val>)
			: sin_{detail::addressof(sin)} {}

			__iterator begin() {
				next_(); // prime the pump
				return __iterator{*this};
			}

			constexpr default_sentinel end() const noexcept
			{ return {}; }
		};

		template<Semiregular Val>
		requires StreamExtractable<Val>
		struct istream_view<Val>::__iterator {
		private:
			detail::raw_ptr<istream_view<Val>> parent_ = nullptr;
		public:
			using iterator_category = input_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = Val;

			__iterator() = default;
			explicit constexpr __iterator(istream_view<Val>& parent) noexcept
			: parent_{detail::addressof(parent)} {}

			__iterator& operator++()
			{ parent_->next_(); return *this; }
			void operator++(int)
			{ ++*this; }

			Val& operator*() const
			{ return parent_->obj_; }

			friend bool operator==(__iterator x, default_sentinel)
			{ return !*x.parent_->sin_; }
			friend bool operator==(default_sentinel y, __iterator x)
			{ return x == y; }
			friend bool operator!=(__iterator x, default_sentinel y)
			{ return !(x == y); }
			friend bool operator!=(default_sentinel y, __iterator x)
			{ return !(y == x); }
		};
	} // namespace ext

	namespace view {
		template<Semiregular Val>
		requires StreamExtractable<Val>
		struct __istream_fn {
			constexpr auto operator()(std::istream& sin) const noexcept
			{ return __stl2::ext::istream_view<Val>{sin}; }
		};

		template<Semiregular Val>
		requires StreamExtractable<Val>
		inline constexpr __istream_fn<Val> istream{};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif
