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
#ifndef STL2_MEMORY_HPP
#define STL2_MEMORY_HPP

#include <memory>
#include <new>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/uninitialized_algorithms.hpp>
#include <stl2/detail/tagged.hpp>

STL2_OPEN_NAMESPACE {
	// pointer traits
	using std::pointer_traits;

	// pointer safety
	using std::pointer_safety;
	using std::declare_reachable;
	using std::undeclare_reachable;
	using std::declare_no_pointers;
	using std::undeclare_no_pointers;
	using std::get_pointer_safety;

	// pointer alignment function
	using std::align;

	// allocator argument tag
	using std::allocator_arg;

	// uses_allocator
	using std::uses_allocator;

	// allocator traits
	using std::allocator_traits;

	// the default allocator
	using std::allocator;

	// raw storage iterator
	using std::raw_storage_iterator;

	// temporary buffers
	using std::get_temporary_buffer;
	using std::return_temporary_buffer;

	// specialized algorithms
	// addressof defined below
	using std::uninitialized_copy;
	using std::uninitialized_copy_n;
	using std::uninitialized_fill;
	using std::uninitialized_fill_n;

	// template class unique_ptr
	using std::default_delete;
	using std::unique_ptr;
	using std::make_unique;

	// class bad_weak_ptr
	using std::bad_weak_ptr;

	// class template shared_ptr
	using std::shared_ptr;

	// shared_ptr creation
	using std::make_shared;
	using std::allocate_shared;

	// shared_ptr casts
	using std::static_pointer_cast;
	using std::dynamic_pointer_cast;
	using std::const_pointer_cast;

	// shared_ptr get_deleter
	using std::get_deleter;

	// class template weak_ptr
	using std::weak_ptr;

	// class template owner_less
	using std::owner_less;

	// shared_ptr atomic access
	using std::atomic_is_lock_free;
	using std::atomic_load;
	using std::atomic_load_explicit;
	using std::atomic_store;
	using std::atomic_store_explicit;
	using std::atomic_exchange;
	using std::atomic_exchange_explicit;
	using std::atomic_compare_exchange_weak;
	using std::atomic_compare_exchange_strong;
	using std::atomic_compare_exchange_weak_explicit;
	using std::atomic_compare_exchange_strong_explicit;

	// hash support
	using std::hash;

	// uses_allocator
	//using std::uses_allocator_v;
	template <class T, class A>
	constexpr bool uses_allocator_v = uses_allocator<T, A>::value;

	// addressof
	namespace __addressof {
		template <class>
		constexpr bool __user_defined_addressof = false;
		template <class T>
		requires
			requires(T& t) { t.operator&(); } ||
			requires(T& t) { operator&(t); }
		constexpr bool __user_defined_addressof<T> = true;

		template <class T>
		requires __user_defined_addressof<T>
		T* impl(T& t) noexcept {
			return std::addressof(t);
		}

		constexpr auto impl(auto& t) noexcept {
			return &t;
		}
	}
	template <class T>
	constexpr T* addressof(T& t) noexcept {
		return __addressof::impl(t);
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_copy [Extension]
	//
	template <InputIterator I, Sentinel<I> S, ForwardIterator O>
	requires
		models::Constructible<value_type_t<O>, reference_t<I>> &&
		models::Same<value_type_t<O>&, reference_t<O>>
	tagged_pair<tag::in(I), tag::out(O)>
	uninitialized_copy(I first, S last, O result)
	{
		using T = value_type_t<O>;
		auto saved = result;
		try {
			for (; first != last; ++first, void(++result)) {
				::new(static_cast<void*>(&*result)) T(*first);
			}
		} catch(...) {
			for (; saved != result; ++saved) {
				(*saved).~T();
			}
			throw;
		}
		return {__stl2::move(first), __stl2::move(result)};
	}

	template <InputRange Rng, class O>
	requires
		models::ForwardIterator<__f<O>> &&
		models::Constructible<value_type_t<__f<O>>, reference_t<iterator_t<Rng>>> &&
		models::Same<value_type_t<__f<O>>&, reference_t<__f<O>>>
	tagged_pair<tag::in(safe_iterator_t<Rng>), tag::out(__f<O>)>
	uninitialized_copy(Rng&& rng, O&& result)
	{
		return __stl2::uninitialized_copy(
			__stl2::begin(rng), __stl2::end(rng), __stl2::forward<O>(result));
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_copy_n [Extension]
	//
	template <class I, class O>
	requires
		models::InputIterator<__f<I>> &&
		models::ForwardIterator<__f<O>> &&
		models::Constructible<value_type_t<__f<O>>, reference_t<__f<I>>> &&
		models::Same<value_type_t<__f<O>>&, reference_t<__f<O>>>
	tagged_pair<tag::in(__f<I>), tag::out(__f<O>)>
	uninitialized_copy_n(I&& first, difference_type_t<__f<I>> n, O&& out)
	{
		auto counted = __stl2::make_counted_iterator(__stl2::forward<I>(first), n);
		auto result = uninitialized_copy(__stl2::move(counted),
			default_sentinel{}, __stl2::forward<O>(out));
		return {result.in().base(), result.out()};
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_fill [Extension]
	//
	template <ForwardIterator I, Sentinel<I> S, CopyConstructible T>
	requires
		models::Constructible<value_type_t<T>, reference_t<I>>() &&
		models::Same<value_type_t<T>&, reference_t<T>>
	void uninitialized_fill(I&& first, S&& last, const T& value)
	{
		detail::uninitialized_fill(__stl2::forward<I>(first), __stl2::forward<S>(last), value);
	}

	template <ForwardRange Rng, CopyConstructible T>
	requires
		models::Constructible<value_type_t<__f<T>>, reference_t<iterator_t<Rng>>>() &&
		models::Same<value_type_t<T>&, reference_t<T>>
	void uninitialized_fill(Rng&& rng, const T& value)
	{
		__stl2::uninitialized_fill(
			__stl2::begin(rng), __stl2::end(rng), result);
	}

	///////////////////////////////////////////////////////////////////////////
	// uninitialized_fill_n [Extension]
	//
	template <ForwardIterator I, CopyConstructible T>
	requires
		models::Constructible<value_type_t<T>, reference_t<I>>() &&
		models::Same<value_type_t<T>&, reference_t<T>>
	tag::in(__f<I>)
	uninitialized_fill_n(I&& first, const Integral size, const T& value)
	{
		auto counted = __stl2::make_counted_iterator(__stl2::forward<I>(first), n);
		return __uninitialized_fill(__stl2::move(counted), default_sentinel{}, value);
	}
} STL2_CLOSE_NAMESPACE

#endif
