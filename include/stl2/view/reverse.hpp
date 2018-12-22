// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_REVERSE_HPP
#define STL2_VIEW_REVERSE_HPP

#include <stl2/detail/cached_position.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template<View V>
	requires BidirectionalRange<V>
	class reverse_view
	: public view_interface<reverse_view<V>>
	, private detail::cached_position<V, reverse_view<V>, !CommonRange<V>> {
		using __iterator = reverse_iterator<iterator_t<V>>;

		V base_ = V();

		constexpr auto& end_() noexcept {
			return static_cast<typename reverse_view::cached_position&>(*this);
		}
	public:
		reverse_view() = default;

		constexpr explicit reverse_view(V base)
		: base_{std::move(base)} {}

		constexpr V base() const { return base_; }

		constexpr __iterator begin() {
			if constexpr (CommonRange<V>) {
				return __iterator{__stl2::end(base_)};
			} else {
				const auto cached = static_cast<bool>(end_());
				iterator_t<V> first = cached
					? end_().get(base_)
					: next(__stl2::begin(base_), __stl2::end(base_));
				if (!cached)
					end_().set(base_, first);
				return __iterator{std::move(first)};
			}
		}

		constexpr __iterator begin() const requires CommonRange<const V> {
			return __iterator{__stl2::end(base_)};
		}

		constexpr __iterator end() {
			return __iterator{__stl2::begin(base_)};
		}
		constexpr __iterator end() const requires CommonRange<const V> {
			return __iterator{__stl2::begin(base_)};
		}

		constexpr auto size() requires SizedRange<V> {
			return __stl2::size(base_);
		}
		constexpr auto size() const requires SizedRange<const V> {
			return __stl2::size(base_);
		}
	};

	template<class R>
	reverse_view(R&&) -> reverse_view<all_view<R>>;

	namespace view {
		template<class>
		inline constexpr bool __is_reversible_subrange = false;
		template<class I, subrange_kind K>
		inline constexpr bool __is_reversible_subrange<
			subrange<reverse_iterator<I>, reverse_iterator<I>, K>> = true;

		struct __reverse_fn : detail::__pipeable<__reverse_fn> {
			template<BidirectionalRange R>
			requires ViewableRange<R>
			constexpr auto operator()(R&& r) const {
				if constexpr (_SpecializationOf<R, reverse_view>) {
					return std::forward<R>(r).base();
				} else if constexpr (__is_reversible_subrange<__uncvref<R>>) {
					using I = decltype(begin(r).base());
					if constexpr (SizedRange<R>) {
						return subrange<I, I, subrange_kind::sized>(
							r.end().base(), r.begin().base(), r.size());
					} else {
						return subrange<I, I>(
							r.end().base(), r.begin().base());
					}
				} else {
					return reverse_view{std::forward<R>(r)};
				}
			}
		};

		inline constexpr __reverse_fn reverse {};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif
