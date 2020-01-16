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
#include <memory>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/iostream/concepts.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<movable Val>
		requires default_initializable<Val> && StreamExtractable<Val>
		struct STL2_EMPTY_BASES istream_view
		: view_interface<istream_view<Val>>
		, detail::semiregular_box<Val> {
		private:
			struct __iterator;

			detail::raw_ptr<std::istream> sin_ = nullptr;

			void next_() { *sin_ >> this->get(); }
		public:
			istream_view() = default;
			explicit constexpr istream_view(std::istream& sin)
			noexcept(std::is_nothrow_default_constructible_v<Val>)
			: sin_{std::addressof(sin)} {}

			__iterator begin() {
				next_(); // prime the pump
				return __iterator{*this};
			}

			constexpr default_sentinel_t end() const noexcept { return {}; }
		};

		template<movable Val>
		requires default_initializable<Val> && StreamExtractable<Val>
		struct istream_view<Val>::__iterator {
			using iterator_category = input_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = Val;

			__iterator() = default;
			explicit constexpr __iterator(istream_view<Val>& parent) noexcept
			: parent_{std::addressof(parent)} {}

			__iterator& operator++() {
				parent_->next_();
				return *this;
			}
			void operator++(int) { ++*this; }

			Val& operator*() const { return parent_->get(); }
			Val* operator->() const { return std::addressof(parent_->get()); }

			friend bool operator==(__iterator x, default_sentinel_t) {
				return x.at_end();
			}
			friend bool operator==(default_sentinel_t, __iterator x) {
				return x.at_end();
			}
			friend bool operator!=(__iterator x, default_sentinel_t) {
				return !x.at_end();
			}
			friend bool operator!=(default_sentinel_t, __iterator x) {
				return !x.at_end();
			}
		private:
			bool at_end() const { return !*parent_->sin_; }
			detail::raw_ptr<istream_view<Val>> parent_ = nullptr;
		};
	} // namespace ext

	namespace views {
		template<class Val>
		requires requires { typename __stl2::ext::istream_view<Val>; }
		struct __istream_fn {
			constexpr auto operator()(std::istream& sin) const noexcept
			{ return __stl2::ext::istream_view<Val>{sin}; }
		};

		template<class Val>
		inline constexpr __istream_fn<Val> istream{};
	} // namespace views
} STL2_CLOSE_NAMESPACE

#endif
