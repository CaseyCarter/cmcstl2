// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015-2016
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
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/memory/addressof.hpp>

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
} STL2_CLOSE_NAMESPACE

#endif
