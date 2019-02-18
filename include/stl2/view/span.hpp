// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2017
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_SPAN_HPP
#define STL2_DETAIL_SPAN_HPP

#include <cstddef>
#include <array>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>
#include <stl2/detail/range/concepts.hpp>

///////////////////////////////////////////////////////////////////////////
// Implementation of P0122 span
// Not to spec: many things (hence the extension namespace). This is a
// "How should span better fit into the Ranges design" experiment.
STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class T>
		using data_pointer_t = decltype(data(std::declval<T&>()));

		template<class R>
		META_CONCEPT SizedContiguousRange =
			ContiguousRange<R> && SizedRange<R>;
	}

	inline constexpr auto dynamic_extent = std::ptrdiff_t{-1};

	template<ext::Object ElementType, std::ptrdiff_t Extent = dynamic_extent>
	requires Extent >= dynamic_extent
	struct span;

	namespace __span {
		template<std::ptrdiff_t Extent>
		requires Extent >= dynamic_extent
		struct extent {
			constexpr extent() noexcept = default;
			constexpr extent(std::ptrdiff_t size) noexcept {
				STL2_EXPECT(size == Extent);
			}
			constexpr std::ptrdiff_t size() const noexcept { return Extent; }
		};
		template<>
		struct extent<dynamic_extent> {
			constexpr extent() noexcept = default;
			constexpr extent(std::ptrdiff_t size) noexcept
			: size_{size} {
				STL2_EXPECT(size >= 0);
			}
			constexpr std::ptrdiff_t size() const noexcept { return size_; }
		private:
			std::ptrdiff_t size_ = 0;
		};

		template<class R>
		META_CONCEPT StaticSizedContiguousRange =
			ext::SizedContiguousRange<R> && ext::__has_static_extent<R>;

		template<Integral To, Integral From>
		constexpr To narrow_cast(From from) noexcept {
			using C = common_type_t<To, From>;
			auto to = static_cast<To>(from);
			STL2_EXPECT((from > 0) == (to > 0));
			STL2_EXPECT(static_cast<C>(from) == static_cast<C>(to));
			return to;
		}

		template<class T>
		constexpr std::ptrdiff_t byte_extent(std::ptrdiff_t count) noexcept {
			if (count < 0) return dynamic_extent;
			STL2_EXPECT(__span::narrow_cast<std::size_t>(count) <= PTRDIFF_MAX / sizeof(T));
			return count * __span::narrow_cast<std::ptrdiff_t>(sizeof(T));
		}

#if STL2_WORKAROUND_CLANG_37556
		template<class T, std::ptrdiff_t E>
		constexpr auto begin(span<T, E> s) noexcept { return s.begin(); }
		template<class T, std::ptrdiff_t E>
		constexpr auto end(span<T, E> s) noexcept { return s.end(); }
#endif // STL2_WORKAROUND_CLANG_37556
	} // namespace __span

	// [span], class template span
	template<ext::Object ElementType, std::ptrdiff_t Extent>
	requires Extent >= dynamic_extent
	struct span : private __span::extent<Extent> {
		// constants and types
		using element_type = ElementType;
		using value_type = std::remove_cv_t<ElementType>;
		using difference_type = std::ptrdiff_t;
		using index_type = difference_type;
		using pointer = element_type*;
		using reference = element_type&;
		using iterator = pointer;
		using reverse_iterator = __stl2::reverse_iterator<iterator>;

		static constexpr index_type extent = Extent;

		// [span.cons], span constructors
		constexpr span() noexcept = default;
		constexpr span(pointer ptr, index_type count) noexcept
		: __span::extent<Extent>{count}
		, data_{ptr} {
			STL2_EXPECT(count == 0 || ptr != nullptr);
		}
		constexpr span(pointer first, pointer last) noexcept
		: span{first, last - first} {}

		template<_NotSameAs<span> R>
		requires ContiguousRange<R> && SizedRange<R> && _ForwardingRange<R> &&
			(Extent == dynamic_extent || Extent == ext::static_extent_of<R>) &&
			ConvertibleTo<
				std::remove_pointer_t<std::remove_reference_t<
					iter_reference_t<iterator_t<R>>>>(*)[],
				ElementType(*)[]>
		constexpr span(R&& rng)
		noexcept(noexcept(__stl2::data(rng), __stl2::size(rng)))
		: span{__stl2::data(rng), __span::narrow_cast<index_type>(__stl2::size(rng))} {}

		// [span.sub], span subviews
		template<index_type Count>
		constexpr span<element_type, Count> first() const noexcept {
			static_assert(Count >= 0,
				"Count of elements to extract cannot be negative.");
			static_assert(Extent == dynamic_extent || Count <= Extent,
				"Count of elements to extract must be less than the static span extent.");
			STL2_EXPECT(size() >= Count);
			STL2_EXPECT(Count == 0 || data_ != nullptr);
			return {data_, Count};
		}
		constexpr span<element_type> first(index_type count) const noexcept {
			STL2_EXPECT(count >= 0);
			STL2_EXPECT(size() >= count);
			STL2_EXPECT(count == 0 || data_ != nullptr);
			return {data_, count};
		}

		template<index_type Count>
		constexpr span<element_type, Count> last() const noexcept {
			static_assert(Count >= 0,
				"Count of elements to extract cannot be negative.");
			static_assert(Extent == dynamic_extent || Count <= Extent,
				"Count of elements to extract must be less than the static span extent.");
			STL2_EXPECT(size() >= Count);
			STL2_EXPECT((Count == 0 && size() == 0) || data_ != nullptr);
			return {data_ + size() - Count, Count};
		}
		constexpr span<element_type> last(index_type count) const noexcept {
			STL2_EXPECT(count >= 0);
			STL2_EXPECT(size() >= count);
			STL2_EXPECT((count == 0 && size() == 0) || data_ != nullptr);
			return {data_ + size() - count, count};
		}

		template<index_type Offset, index_type Count>
		constexpr span<element_type, Count> subspan() const noexcept {
			static_assert(Offset >= 0,
				"Offset of first element to extract cannot be negative.");
			static_assert(Count >= 0,
				"Count of elements to extract cannot be negative.");
			static_assert(Extent == dynamic_extent || Extent >= Offset + Count,
				"Sequence of elements to extract must be within the static span extent.");
			STL2_EXPECT(size() >= Offset + Count);
			STL2_EXPECT((Offset == 0 && Count == 0) || data_ != nullptr);
			return {data_ + Offset, Count};
		}
		template<index_type Offset>
		requires Offset >= 0 && (Extent == dynamic_extent || Extent >= Offset)
		constexpr span<element_type, Extent >= Offset ? Extent - Offset : dynamic_extent>
		subspan() const noexcept {
			static_assert(Offset >= 0,
				"Offset of first element to extract cannot be negative.");
			static_assert(Extent == dynamic_extent || Offset <= Extent,
				"Offset of first element to extract must be less than the static span extent.");
			STL2_EXPECT(size() >= Offset);
			STL2_EXPECT((Offset == 0 && size() == 0) || data_ != nullptr);
			return {data_ + Offset, size() - Offset};
		}
		constexpr span<element_type, dynamic_extent>
		subspan(index_type offset) const noexcept {
			STL2_EXPECT(offset >= 0);
			STL2_EXPECT(size() >= offset);
			STL2_EXPECT((offset == 0 && size() == 0) || data_ != nullptr);
			return {data_ + offset, size() - offset};
		}
		constexpr span<element_type, dynamic_extent>
		subspan(index_type offset, index_type count) const noexcept {
			STL2_EXPECT(offset >= 0);
			STL2_EXPECT(count >= 0);
			STL2_EXPECT(size() >= offset + count);
			STL2_EXPECT((offset == 0 && count == 0) || data_ != nullptr);
			return {data_ + offset, count};
		}

		// [span.obs], span observers
		using __span::extent<Extent>::size;
		constexpr index_type size_bytes() const noexcept {
			return __span::byte_extent<ElementType>(size());
		}
		constexpr bool empty() const noexcept { return size() == 0; }

		// [span.elem], span element access
		constexpr reference operator[](index_type idx) const noexcept {
			STL2_EXPECT(idx >= 0);
			STL2_EXPECT(idx < size());
			STL2_EXPECT(data_);
			return data_[idx];
		}
		constexpr pointer data() const noexcept { return data_; }

		// [span.iter], span iterator support
		constexpr iterator begin() const noexcept { return data_; }
		constexpr iterator end() const noexcept {
			STL2_EXPECT(!size() || data_);
			return data_ + size();
		}
		constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator{end()}; }
		constexpr reverse_iterator rend() const noexcept { return reverse_iterator{begin()}; }

#if !STL2_WORKAROUND_CLANG_37556
		friend constexpr iterator begin(span s) noexcept { return s.begin(); }
		friend constexpr iterator end(span s) noexcept { return s.end(); }
#endif // !STL2_WORKAROUND_CLANG_37556

	private:
		ElementType* data_ = nullptr;
	};

	template<ext::SizedContiguousRange Rng>
	span(Rng&& rng) -> span<std::remove_pointer_t<ext::data_pointer_t<Rng>>>;

	template<__span::StaticSizedContiguousRange Rng>
	span(Rng&& rng) -> span<std::remove_pointer_t<ext::data_pointer_t<Rng>>,
		ext::static_extent_of<Rng>>;

	template<class T, std::ptrdiff_t Extent>
	requires Extent > dynamic_extent
	inline constexpr std::ptrdiff_t ext::static_extent<span<T, Extent>> = Extent;

	// [span.objectrep], views of object representation
	template<class ElementType, std::ptrdiff_t Extent>
	span<const unsigned char, __span::byte_extent<ElementType>(Extent)>
	as_bytes(span<ElementType, Extent> s) noexcept {
		return {reinterpret_cast<const unsigned char*>(s.data()), s.size_bytes()};
	}
	template<class ElementType, std::ptrdiff_t Extent>
	span<unsigned char, __span::byte_extent<ElementType>(Extent)>
	as_writeable_bytes(span<ElementType, Extent> s) noexcept {
		return {reinterpret_cast<unsigned char*>(s.data()), s.size_bytes()};
	}
} STL2_CLOSE_NAMESPACE

#endif
