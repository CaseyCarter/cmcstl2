// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//

#ifndef STL2_DETAIL_CACHED_POSITION_HPP
#define STL2_DETAIL_CACHED_POSITION_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		/// \addtogroup group-utility
		/// @{

		/// \brief Caches a position in a range.
		template<Range R, class Tag = void, bool Enable = true>
		struct cached_position {
			/// \brief Returns `true` if and only if a position is cached.
			constexpr explicit operator bool() const noexcept {
				return false;
			}
			/// \brief Retrieves an `iterator_t<R>` denoting the cached position.
			///
			/// \pre `*this` caches a position within the \c Range `r`.
			[[noreturn]] iterator_t<R> get(R& r) const {
				(void) r;
				STL2_EXPECT(false);
			}
			/// \brief Caches the position of the `iterator_t<R>` argument.
			///
			/// \pre The \c Iterator `it` denotes a position in \c Range r.
			constexpr void set(R& r, const iterator_t<R>& it) {
				(void) r;
				(void) it;
			}
		};
		/// @}

		/// \cond
		template<Range R, class Tag>
		struct cached_position<R, Tag, true> {
			constexpr explicit operator bool() const noexcept {
				return static_cast<bool>(cache_);
			}
			constexpr iterator_t<R> get(R&) const {
				STL2_EXPECT(*this);
				return *cache_;
			}
			constexpr void set(R&, const iterator_t<R>& it) {
				cache_ = it;
			}
		private:
			meta::if_c<_ForwardingRange<R>,
				std::optional<iterator_t<R>>,
				non_propagating_cache<iterator_t<R>>> cache_;
		};
		template<RandomAccessRange R, class Tag>
		struct cached_position<R, Tag, true> {
			cached_position() = default;
			cached_position(const cached_position&) = default;
			cached_position(cached_position&& that)
			:	offset_{__stl2::exchange(that.offset_, -1)} {}
			cached_position& operator=(const cached_position&) = default;
			cached_position& operator=(cached_position&& that) {
				offset_ = __stl2::exchange(that.offset_, -1);
				return *this;
			}

			constexpr explicit operator bool() const noexcept {
				return offset_ >= 0;
			}
			constexpr iterator_t<R> get(R& r) const {
				STL2_EXPECT(*this);
				return begin(r) + offset_;
			}
			constexpr void set(R& r, const iterator_t<R>& it) {
				offset_ = it - begin(r);
			}
		private:
			iter_difference_t<iterator_t<R>> offset_ = -1;
		};
		/// \endcond
	}
} STL2_CLOSE_NAMESPACE

#endif
