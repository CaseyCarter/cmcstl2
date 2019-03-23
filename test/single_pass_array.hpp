#include <cstddef>
#include <iterator>
#include <type_traits>
#include <stl2/detail/concepts/object.hpp>

template<class> struct single_pass_pointer_iterator;

template<class T>
struct single_pass_pointer_sentinel {
	friend single_pass_pointer_iterator<T>;
	friend single_pass_pointer_iterator<const T>;

	single_pass_pointer_sentinel() = default;
	explicit constexpr single_pass_pointer_sentinel(const T* end) : end_{end} {}
private:
	const T* end_ = nullptr;
};

template<class T>
struct single_pass_pointer_iterator {
	using iterator_category = std::input_iterator_tag;
	using difference_type   = std::ptrdiff_t;
	using value_type        = std::remove_cv_t<T>;
	using pointer           = T*;
	using reference         = T&;

	single_pass_pointer_iterator() = default;
	explicit constexpr single_pass_pointer_iterator(T* ptr) noexcept : p_{ptr} {}

	single_pass_pointer_iterator(single_pass_pointer_iterator &&) = default;
	single_pass_pointer_iterator & operator=(single_pass_pointer_iterator && other) = default;

	T& operator*() const noexcept { return *p_; }
	T* operator->() const noexcept { return p_; }

	single_pass_pointer_iterator& operator++() noexcept {
		++p_;
		return *this;
	}
	void operator++(int) noexcept {
		++*this;
	}

	friend constexpr bool operator==(
		const single_pass_pointer_iterator& x, single_pass_pointer_sentinel<value_type> y) noexcept {
		return x.p_ == y.end_;
	}
	friend constexpr bool operator==(
		single_pass_pointer_sentinel<value_type> y, const single_pass_pointer_iterator& x) noexcept {
		return x == y;
	}
	friend constexpr bool operator!=(
		const single_pass_pointer_iterator& x, single_pass_pointer_sentinel<value_type> y) noexcept {
		return !(x == y);
	}
	friend constexpr bool operator!=(
		single_pass_pointer_sentinel<value_type> y, const single_pass_pointer_iterator& x) noexcept {
		return !(x == y);
	}

private:
	T* p_ = nullptr;
};

template<__stl2::ext::destructible_object T, std::size_t N>
struct single_pass_array {
	using value_type = T;
	using iterator = single_pass_pointer_iterator<T>;
	using const_iterator = single_pass_pointer_iterator<const T>;

	constexpr auto begin() noexcept { return single_pass_pointer_iterator<T>{elements_ + 0}; }
	constexpr auto begin() const noexcept { return single_pass_pointer_iterator<const T>{elements_ + 0}; }
	constexpr auto end() const noexcept { return single_pass_pointer_sentinel<T>{elements_ + N}; }

	constexpr const T* data() const noexcept { return elements_; }
	static constexpr std::size_t size() noexcept { return N; }

	T elements_[N];
};

template<class T, class... Args>
single_pass_array(T, Args...) -> single_pass_array<T, sizeof...(Args) + 1>;
