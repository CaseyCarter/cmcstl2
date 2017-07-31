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
#include <stl2/detail/algorithm/equal.hpp>
#include <stl2/detail/algorithm/lexicographical_compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/reverse_iterator.hpp>

///////////////////////////////////////////////////////////////////////////
// Implementation of P0122 span
//
STL2_OPEN_NAMESPACE {
	namespace ext {
#if defined(__cpp_inline_variables)
		inline
#endif
		constexpr std::ptrdiff_t dynamic_extent = static_cast<std::ptrdiff_t>(-1);

		template <_Is<std::is_object> ElementType, std::ptrdiff_t Extent = dynamic_extent>
		requires Extent >= dynamic_extent
		struct span;

		namespace __span {
			template <std::ptrdiff_t Extent>
			requires Extent >= dynamic_extent
			struct base {
				constexpr base() noexcept
				requires Extent == 0
				{}
				constexpr base(std::ptrdiff_t size) noexcept
				{
					STL2_EXPECT(size == Extent);
				}
				constexpr std::ptrdiff_t size() const noexcept { return Extent; }
			};
			template <>
			struct base<dynamic_extent> {
				constexpr base(std::ptrdiff_t size = 0) noexcept
				: size_{size}
				{
					STL2_EXPECT(size >= 0);
				}
				constexpr std::ptrdiff_t size() const noexcept { return size_; }
			private:
				std::ptrdiff_t size_ = 0;
			};

			template <class> struct static_extent_ {};
			template <class T, std::size_t N>
			struct static_extent_<T[N]>
			: std::integral_constant<std::ptrdiff_t, static_cast<std::ptrdiff_t>(N)>
			{};
			template <class T, std::size_t N>
			struct static_extent_<std::array<T, N>>
			: std::integral_constant<std::ptrdiff_t, static_cast<std::ptrdiff_t>(N)>
			{};
			template <class T, std::ptrdiff_t N>
			struct static_extent_<span<T, N>>
			: std::integral_constant<std::ptrdiff_t, N>
			{};
			template <class T>
			struct static_extent_<span<T>>
			{};
			template <class T>
			using static_extent = static_extent_<__uncvref<T>>;

			template <class>
			constexpr bool has_static_extent = false;
			template <class T>
			requires requires { static_extent<T>::value; }
			constexpr bool has_static_extent<T> = true;

			template <class Range, class ElementType>
			concept bool compatible = ContiguousLikeRange<Range> &&
				std::is_convertible<
					std::remove_pointer_t<
						decltype(__stl2::data(std::declval<Range&>()))>(*)[],
					ElementType(*)[]>::value;

			template <class T>
			constexpr std::ptrdiff_t byte_extent(std::ptrdiff_t count) noexcept
			{
				if (count < 0) {
					return dynamic_extent;
				}
				STL2_EXPECT(static_cast<std::size_t>(count) <= PTRDIFF_MAX / sizeof(T));
				return count * sizeof(T);
			}

			template <class To, Common<To> From>
			constexpr To narrow_cast(From from) noexcept {
				using C = common_type_t<To, From>;
				auto to = static_cast<To>(from);
				STL2_EXPECT((from > 0) == (to > 0));
				STL2_EXPECT(C(from) == C(to));
				return to;
			}
		} // namespace __span

		// [span], class template span
		template <_Is<std::is_object> ElementType, std::ptrdiff_t Extent>
		requires Extent >= dynamic_extent
		struct span : __span::base<Extent> {
			// constants and types
			using element_type = ElementType;
			using value_type = std::remove_cv_t<ElementType>;
			using index_type = std::ptrdiff_t;
			using difference_type = std::ptrdiff_t;
			using pointer = element_type*;
			using reference = element_type&;
			using iterator = pointer;
			using const_iterator = iterator;
			using reverse_iterator = __stl2::reverse_iterator<iterator>;
			using const_reverse_iterator = __stl2::reverse_iterator<const_iterator>;
			static constexpr index_type extent = Extent;

			// [span.cons], span constructors
			using __span::base<Extent>::base;
			span() = default;
			constexpr span(pointer ptr, index_type count)
			: __span::base<Extent>{count}, data_{ptr}
			{
				STL2_EXPECT(count >= 0);
				STL2_EXPECT(Extent == dynamic_extent || count == Extent);
				STL2_EXPECT(count == 0 || ptr != nullptr);
			}
			constexpr span(pointer first, pointer last)
			: span{first, last - first}
			{}

			template <__span::compatible<ElementType> Range>
			requires Extent == __span::static_extent<Range>::value &&
				(std::is_lvalue_reference<Range>::value || View<Range>)
			constexpr span(Range&& rng)
			noexcept(noexcept(__stl2::data(rng)))
			: span(__stl2::data(rng), Extent)
			{}
			template <__span::compatible<ElementType> Range>
			requires Extent == __span::static_extent<Range>::value &&
				!(std::is_lvalue_reference<Range>::value || View<Range>)
			explicit constexpr span(Range&& rng)
			noexcept(noexcept(__stl2::data(rng)))
			: span(__stl2::data(rng), Extent)
			{}

			template <__span::compatible<ElementType> Range>
			requires (Extent == dynamic_extent || !__span::has_static_extent<Range>) &&
				(std::is_lvalue_reference<Range>::value || View<Range>)
			constexpr span(Range&& rng)
			: span(__stl2::data(rng), __span::narrow_cast<index_type>(__stl2::size(rng)))
			{}
			template <__span::compatible<ElementType> Range>
			requires (Extent == dynamic_extent || !__span::has_static_extent<Range>) &&
				!(std::is_lvalue_reference<Range>::value || View<Range>)
			explicit constexpr span(Range&& rng)
			: span(__stl2::data(rng), __span::narrow_cast<index_type>(__stl2::size(rng)))
			{}

			// [span.sub], span subviews
			template <ptrdiff_t Count>
			requires Count >= 0 && (Extent == dynamic_extent || Extent >= Count)
			constexpr span<element_type, Count> first() const noexcept
			{
				STL2_EXPECT(size() >= Count);
				return {data(), Count};
			}
			constexpr span<element_type> first(index_type count) const noexcept
			{
				STL2_EXPECT(count >= 0);
				STL2_EXPECT(size() >= count);
				return {data(), count};
			}

			template <ptrdiff_t Count>
			requires Count >= 0 && (Extent == dynamic_extent || Extent >= Count)
			constexpr span<element_type, Count> last() const noexcept
			{
				STL2_EXPECT(size() >= Count);
				return {data() + size() - Count, Count};
			}
			constexpr span<element_type> last(index_type count) const noexcept
			{
				STL2_EXPECT(count >= 0);
				STL2_EXPECT(size() >= count);
				return {data() + size() - count, count};
			}

			template <ptrdiff_t Offset>
			requires Offset >= 0 && (Extent == dynamic_extent || Extent >= Offset)
			constexpr span<element_type, Extent >= Offset ? Extent - Offset : dynamic_extent> subspan() const noexcept
			{
				STL2_EXPECT(size() >= Offset);
				return {data() + Offset, size() - Offset};
			}
			template <ptrdiff_t Offset, ptrdiff_t Count>
			requires Offset >= 0 && Count >= 0 && (Extent == dynamic_extent || Extent >= Offset + Count)
			constexpr span<element_type, Count> subspan() const noexcept
			{
				STL2_EXPECT(size() >= Offset + Count);
				return {data() + Offset, Count};
			}
			constexpr span<element_type, dynamic_extent> subspan(index_type offset) const noexcept
			{
				STL2_EXPECT(offset >= 0);
				STL2_EXPECT(size() >= offset);
				return {data() + offset, size() - offset};
			}
			constexpr span<element_type, dynamic_extent> subspan(index_type offset, index_type count) const noexcept
			{
				STL2_EXPECT(offset >= 0);
				STL2_EXPECT(count >= 0);
				STL2_EXPECT(size() >= offset + count);
				return {data() + offset, count};
			}

			// [span.obs], span observers
			using __span::base<Extent>::size;
			constexpr index_type size_bytes() const noexcept
			{
				return __span::byte_extent<ElementType>(size());
			}
			constexpr bool empty() const noexcept { return size() == 0; }

			// [span.elem], span element access
			constexpr reference operator[](index_type idx) const noexcept
			{
				STL2_EXPECT(idx >= 0);
				STL2_EXPECT(idx < size());
				return data_[idx];
			}
			constexpr pointer data() const noexcept { return data_; }

			// [span.iter], span iterator support
			iterator begin() const noexcept { return data_; }
			iterator end() const noexcept { return data_ + size(); }
			iterator cbegin() const noexcept { return data_; }
			iterator cend() const noexcept { return data_ + size(); }
			reverse_iterator rbegin() const noexcept { return reverse_iterator{data_ + size()}; }
			reverse_iterator rend() const noexcept { return reverse_iterator{data_}; }
			reverse_iterator crbegin() const noexcept { return reverse_iterator{data_ + size()}; }
			reverse_iterator crend() const noexcept { return reverse_iterator{data_}; }

			// [span.comparison], span comparison operators
			template <class U, std::ptrdiff_t UExtent>
			requires EqualityComparableWith<ElementType, U>
			friend constexpr bool operator==(const span& x, const span<U, UExtent>& y)
			{
				return __stl2::equal(x, y);
			}
			template <class U, std::ptrdiff_t UExtent>
			requires EqualityComparableWith<ElementType, U>
			friend constexpr bool operator!=(const span& x, const span<U, UExtent>& y)
			{
				return !(x == y);
			}
			template <class U, std::ptrdiff_t UExtent>
			requires StrictTotallyOrderedWith<ElementType, U>
			friend constexpr bool operator<(const span& x, const span<U, UExtent>& y)
			{
				return __stl2::lexicographical_compare(x, y);
			}
			template <class U, std::ptrdiff_t UExtent>
			requires StrictTotallyOrderedWith<ElementType, U>
			friend constexpr bool operator>(const span& x, const span<U, UExtent>& y)
			{
				return y < x;
			}
			template <class U, std::ptrdiff_t UExtent>
			requires StrictTotallyOrderedWith<ElementType, U>
			friend constexpr bool operator<=(const span& x, const span<U, UExtent>& y)
			{
				return !(y < x);
			}
			template <class U, std::ptrdiff_t UExtent>
			requires StrictTotallyOrderedWith<ElementType, U>
			friend constexpr bool operator>=(const span& x, const span<U, UExtent>& y)
			{
				return !(x < y);
			}

		private:
			ElementType* data_ = nullptr;
		};

#ifdef __cpp_deduction_guides
		template <ContiguousLikeRange Rng>
		span(Rng&& rng) -> span<std::remove_pointer_t<decltype(__stl2::data(rng))>>;
#endif

		// [span.objectrep], views of object representation
		template <class ElementType, std::ptrdiff_t Extent>
		span<const unsigned char, __span::byte_extent<ElementType>(Extent)>
		as_bytes(span<ElementType, Extent> s) noexcept
		{
			return {reinterpret_cast<const unsigned char*>(s.data()), s.size_bytes()};
		}
		template <class ElementType, std::ptrdiff_t Extent>
		span<unsigned char, __span::byte_extent<ElementType>(Extent)>
		as_writeable_bytes(span<ElementType, Extent> s) noexcept
		{
			return {reinterpret_cast<unsigned char*>(s.data()), s.size_bytes()};
		}

		template <class ElementType>
		constexpr auto make_span(ElementType* ptr, std::ptrdiff_t count) noexcept
		{
			return span<ElementType>{ptr, count};
		}
		template <class ElementType>
		constexpr auto make_span(ElementType* first, ElementType* last) noexcept
		{
			return span<ElementType>{first, last};
		}
		template <ContiguousLikeRange Rng>
		constexpr auto make_span(Rng&& rng)
		noexcept(noexcept(__stl2::data(rng), __stl2::size(rng)))
		{
			using S = span<std::remove_pointer_t<decltype(__stl2::data(rng))>>;
			return S{__stl2::data(rng), __span::narrow_cast<std::ptrdiff_t>(__stl2::size(rng))};
		}
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
