// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_MEMORY_DESTROY_HPP
#define STL2_DETAIL_MEMORY_DESTROY_HPP

#include <memory>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/range/dangling.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// destroy_at [specialized.destroy]
	//
	struct __destroy_at_fn : private __niebloid {
		template<destructible T>
		void operator()(T* p) const noexcept;
	};

	inline constexpr __destroy_at_fn destroy_at {};

	///////////////////////////////////////////////////////////////////////////
	// destroy [specialized.destroy]
	//
	struct __destroy_fn : private __niebloid {
		template<_NoThrowInputIterator I, _NoThrowSentinel<I> S>
		requires destructible<iter_value_t<I>>
		I operator()(I first, S last) const noexcept {
			if constexpr (std::is_trivially_destructible_v<iter_value_t<I>>) {
				advance(first, last);
			} else {
				for (; first != last; ++first) {
					destroy_at(std::addressof(*first));
				}
			}

			return first;
		}

		template<_NoThrowInputRange R>
		requires destructible<iter_value_t<iterator_t<R>>>
		safe_iterator_t<R> operator()(R&& r) const noexcept {
			if constexpr (std::is_trivially_destructible_v<iter_value_t<iterator_t<R>>> &&
			              same_as<dangling, safe_iterator_t<R>>) {
				return {};
			} else {
				return (*this)(begin(r), end(r));
			}
		}
	};

	inline constexpr __destroy_fn destroy {};

	template<destructible T>
	inline void __destroy_at_fn::operator()(T* p) const noexcept {
		if constexpr (!std::is_trivially_destructible_v<T>) {
			if constexpr (std::is_array_v<T>) {
				destroy(begin(*p), end(*p));
			} else {
				p->~T();
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// destroy_n [specialized.destroy]
	//
	struct __destroy_n_fn : private __niebloid {
		template<_NoThrowInputIterator I>
		requires destructible<iter_value_t<I>>
		I operator()(I first, iter_difference_t<I> n) const noexcept {
			if constexpr (std::is_trivially_destructible_v<iter_value_t<I>>) {
				return next(std::move(first), n);
			} else {
				return destroy(counted_iterator{std::move(first), n},
					default_sentinel).base();
			}
		}
	};

	inline constexpr __destroy_n_fn destroy_n {};

	namespace detail {
		template<_NoThrowForwardIterator I>
		requires destructible<iter_value_t<I>>
		struct destroy_guard {
			~destroy_guard() {
				if (last_) {
					destroy(std::move(first_), *last_);
				}
			}

			explicit destroy_guard(I& it)
			: first_{it}, last_{std::addressof(it)} {}

			destroy_guard(destroy_guard&& that)
			: first_{std::move(that.first_)}
			, last_{__stl2::exchange(that.last_, nullptr)} {}

			void release() noexcept { last_ = nullptr; }
		private:
			I first_;
			detail::raw_ptr<I> last_;
		};

		template<_NoThrowForwardIterator I>
		requires destructible<iter_value_t<I>> &&
			std::is_trivially_destructible_v<iter_value_t<I>>
		struct destroy_guard<I> {
			constexpr explicit destroy_guard(I&) noexcept {}
			constexpr void release() const noexcept {}
		};
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_DESTROY_HPP
