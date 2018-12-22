// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_TAKE_EXACTLY_HPP
#define STL2_VIEW_TAKE_EXACTLY_HPP

#include <type_traits>
#include <utility>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/view/all.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<View Base>
		class take_exactly_view
		: public view_interface<take_exactly_view<Base>>
		, private detail::ebo_box<Base, take_exactly_view<Base>>
		{
			using base_t = detail::ebo_box<Base, take_exactly_view<Base>>;
			using base_t::get;

			iter_difference_t<iterator_t<Base>> n_;
		public:
			take_exactly_view() = default;

			constexpr take_exactly_view(Base view, iter_difference_t<iterator_t<Base>> n)
			noexcept(std::is_nothrow_move_constructible<Base>::value)
			: base_t{std::move(view)}, n_{n}
			{ STL2_EXPECT(n >= 0); }

			constexpr auto begin()
			noexcept(noexcept(counted_iterator{__stl2::begin(std::declval<Base&>()), n_}))
			requires !Range<Base const>
			{ return counted_iterator{__stl2::begin(get()), n_}; }

	#if 0 // FIXME: Untagged bug workaround
			constexpr auto data()
			noexcept(noexcept(__stl2::data(std::declval<Base&>())))
			requires
				!Range<Base const> &&
				requires(Base& b) { __stl2::data(b); }
			{ return __stl2::data(get()); }
	#else
			template<class B = Base>
			requires
				Same<B, Base> &&
				!Range<B const> &&
				requires(B& b) { __stl2::data(b); }
			constexpr auto data()
			noexcept(noexcept(__stl2::data(std::declval<B&>())))
			{ return __stl2::data(get()); }
	#endif

			constexpr auto begin() const
			noexcept(noexcept(counted_iterator{__stl2::begin(std::declval<Base const&>()), n_}))
			requires Range<Base const>
			{ return counted_iterator{__stl2::begin(get()), n_}; }

	#if 0 // FIXME: Untagged bug workaround
			constexpr auto data() const
			noexcept(noexcept(__stl2::data(std::declval<Base const&>())))
			requires
				!Range<Base const> &&
				requires(Base const& b) { __stl2::data(b); }
			{ return __stl2::data(get()); }
	#else
			template<class B = Base>
			requires
				Same<B, Base> &&
				Range<B const> &&
				requires(B const& b) { __stl2::data(b); }
			constexpr auto data() const
			noexcept(noexcept(__stl2::data(std::declval<B const&>())))
			{ return __stl2::data(get()); }
	#endif

			constexpr default_sentinel end() const noexcept { return {}; }
			constexpr iter_difference_t<iterator_t<Base>> size() const noexcept { return n_; }
			constexpr bool empty() const noexcept { return n_ == 0; }
		};

		template<Range R>
		take_exactly_view(R&& base, iter_difference_t<iterator_t<R>> n)
			-> take_exactly_view<all_view<R>>;
	} // namespace ext

	template<class V>
	inline constexpr bool enable_view<ext::take_exactly_view<V>> = true;

	namespace view::ext {
		struct __take_exactly_fn : detail::__pipeable<__take_exactly_fn> {
			template<class V>
			constexpr auto operator()(V&& view, iter_difference_t<iterator_t<V>> const n) const
			STL2_REQUIRES_RETURN(
				__stl2::ext::take_exactly_view(all(std::forward<V>(view)), n)
			)

			template<Integral D>
			constexpr auto operator()(D count) const {
				return detail::view_closure(*this, static_cast<D>(count));
			}
		};

		inline constexpr __take_exactly_fn take_exactly {};
	} // namespace view::ext
} STL2_CLOSE_NAMESPACE

#endif
