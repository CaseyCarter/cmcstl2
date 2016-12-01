// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2016
//  Copyright Christopher Di Bella 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef RAW_BUFFER_HPP
#define RAW_BUFFER_HPP

#include <stl2/memory.hpp>
#include <stl2/detail/swap.hpp>

template <typename T>
class raw_buffer {
public:
	~raw_buffer() {
		if (data_) {
			std::allocator<T>{}.deallocate(data_, size_);
		}
	}

	explicit raw_buffer(const std::ptrdiff_t size)
		: data_{std::allocator<T>{}.allocate(size)}
		, size_{size}
	{}

	raw_buffer(raw_buffer&& that) noexcept
	: data_{__stl2::exchange(that.data_, nullptr)}
	, size_{__stl2::exchange(that.size_, 0)}
	{}

	raw_buffer& operator=(raw_buffer&& that) & noexcept {
		data_ = __stl2::exchange(that.data_, nullptr);
		size_ = __stl2::exchange(that.size_, 0);
		return *this;
	}

	T* begin() noexcept {
		return data_;
	}

	const T* begin() const noexcept {
		return data_;
	}

	auto cbegin() const noexcept {
		return begin();
	}

	T* end() noexcept {
		return data_ + size_;
	}

	const T* end() const noexcept {
		return data_ + size_;
	}

	auto cend() const noexcept {
		return end();
	}

	T* data() noexcept {
		return data_;
	}
	const T* data() const noexcept {
		return data_;
	}

	std::ptrdiff_t size() const noexcept {
		return size_;
	}
private:
	T* data_;
	std::ptrdiff_t size_;
};

template <typename T>
using Array = std::array<T, 8>;

template <typename T>
auto make_buffer(const std::size_t size) {
	// STL2_EXPECT(size <= static_cast<std::size_t>(PTRDIFF_MAX)); // FIXME: post-merge
	return raw_buffer<T>{static_cast<std::ptrdiff_t>(size)};
}

#endif // RAW_BUFFER_HPP
