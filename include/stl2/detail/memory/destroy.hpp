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

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/counted_iterator.hpp>
#include <stl2/detail/iterator/dangling.hpp>
#include <stl2/detail/memory/addressof.hpp>
#include <stl2/detail/memory/concepts.hpp>
#include <stl2/detail/range/access.hpp>
#include <stl2/detail/range/concepts.hpp>

STL2_OPEN_NAMESPACE {
	///////////////////////////////////////////////////////////////////////////
	// destroy_at [Extension]
	//
	template<Destructible T>
	void destroy_at(T* p) noexcept
	{
		p->~T();
	}

	///////////////////////////////////////////////////////////////////////////
	// destroy [Extension]
	//
	template<__NoThrowInputIterator I, __NoThrowSentinel<I> S>
	requires
		Destructible<iter_value_t<I>>
	I destroy(I first, S last) noexcept
	{
		for (; first != last; ++first) {
			__stl2::destroy_at(detail::addressof(*first));
		}

		return first;
	}

	template<__NoThrowInputRange Rng>
	requires
		Destructible<iter_value_t<iterator_t<Rng>>>
	safe_iterator_t<Rng> destroy(Rng&& rng) noexcept
	{
		return __stl2::destroy(begin(rng), end(rng));
	}

	///////////////////////////////////////////////////////////////////////////
	// destroy_n [Extension]
	//
	template<__NoThrowInputIterator I>
	requires
		Destructible<iter_value_t<I>>
	I destroy_n(I first, iter_difference_t<I> n) noexcept
	{
		return __stl2::destroy(counted_iterator{std::move(first), n},
			default_sentinel{}).base();
	}

	namespace detail {
		template<__NoThrowForwardIterator I>
		class destroy_guard {
			I first_;
			detail::raw_ptr<I> last_;
		public:
			~destroy_guard() {
				if (last_) {
					__stl2::destroy(std::move(first_), *last_);
				}
			}

			explicit destroy_guard(I& it)
			: first_{it}, last_{detail::addressof(it)} {}

			destroy_guard(destroy_guard&& that)
			: first_{std::move(that.first_)}
			, last_{__stl2::exchange(that.last_, nullptr)} {}

			void release() noexcept { last_ = nullptr; }
		};
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_MEMORY_DESTROY_HPP
