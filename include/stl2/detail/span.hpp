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
		template<class T>
		using data_pointer_t = decltype(__stl2::data(std::declval<T&>()));

		template<class Range>
		STL2_CONCEPT SizedContiguousRange =
			ContiguousRange<Range> && SizedRange<Range>;

		namespace __span {
			using index_t = std::ptrdiff_t;
		}

#if defined(__cpp_inline_variables)
		inline
#endif
		constexpr auto dynamic_extent = static_cast<__span::index_t>(-1);

		template<_Is<std::is_object> ElementType, __span::index_t Extent = dynamic_extent>
		requires Extent >= dynamic_extent
		struct span;

		namespace __span {
			template<index_t Extent>
			requires Extent >= dynamic_extent
			struct extent {
				constexpr extent() noexcept = default;
				constexpr extent(index_t size) noexcept
				{
					STL2_EXPECT(size == Extent);
				}
				constexpr index_t size() const noexcept { return Extent; }
			};
			template<>
			struct extent<dynamic_extent> {
				constexpr extent() noexcept = default;
				constexpr extent(index_t size) noexcept
				: size_{size}
				{
					STL2_EXPECT(size >= 0);
				}
				constexpr index_t size() const noexcept { return size_; }
			private:
				index_t size_ = 0;
			};

			template<class> struct static_extent_ {};
			template<class T, std::size_t Extent>
			struct static_extent_<T[Extent]>
			: std::integral_constant<index_t, static_cast<index_t>(Extent)>
			{};
			template<class T, std::size_t Extent>
			struct static_extent_<std::array<T, Extent>>
			: std::integral_constant<index_t, static_cast<index_t>(Extent)>
			{};
			template<class T, index_t Extent>
			struct static_extent_<span<T, Extent>>
			: std::integral_constant<index_t, Extent>
			{};
			template<class T>
			struct static_extent_<span<T>>
			{};
			template<class T>
			using static_extent = static_extent_<__uncvref<T>>;

			template<class>
			constexpr bool has_static_extent = false;
			template<class T>
			requires requires { static_extent<T>::value; }
			constexpr bool has_static_extent<T> = true;

			template<class Range>
			STL2_CONCEPT StaticSizedContiguousRange =
				SizedContiguousRange<Range> && has_static_extent<Range>;

			template<class Range, class ElementType>
			STL2_CONCEPT compatible = SizedContiguousRange<Range> &&
				ConvertibleTo<
					std::remove_pointer_t<data_pointer_t<Range>>(*)[],
					ElementType(*)[]>;

			template<Integral To, Integral From>
			constexpr To narrow_cast(From from) noexcept {
				using C = common_type_t<To, From>;
				auto to = static_cast<To>(from);
				STL2_EXPECT((from > 0) == (to > 0));
				STL2_EXPECT(static_cast<C>(from) == static_cast<C>(to));
				return to;
			}

			template<class T>
			constexpr index_t byte_extent(index_t count) noexcept
			{
				if (count < 0) {
					return dynamic_extent;
				}
				STL2_EXPECT(__span::narrow_cast<std::size_t>(count) <= PTRDIFF_MAX / sizeof(T));
				return count * __span::narrow_cast<index_t>(sizeof(T));
			}
		} // namespace __span

		// [span], class template span
		template<_Is<std::is_object> ElementType, __span::index_t Extent>
		requires Extent >= dynamic_extent
		struct span : private __span::extent<Extent> {
			// constants and types
			using element_type = ElementType;
			using value_type = std::remove_cv_t<ElementType>;
			using difference_type = __span::index_t;
			using index_type = difference_type;
			using pointer = element_type*;
			using reference = element_type&;
			using iterator = pointer;
			using reverse_iterator = __stl2::reverse_iterator<iterator>;

			static constexpr index_type extent = Extent;

			// [span.cons], span constructors
			constexpr span() noexcept = default;
			constexpr span(pointer ptr, index_type count) noexcept
			requires true // HACK: disambiguates span{ptr, 0}
			: __span::extent<Extent>{count}, data_{ptr}
			{
				STL2_EXPECT(count == 0 || ptr != nullptr);
			}
			constexpr span(pointer first, pointer last) noexcept
			: span{first, last - first}
			{}

			template<__span::compatible<ElementType> Range>
			requires Extent == __span::static_extent<Range>::value
			constexpr span(Range&& rng)
			noexcept(noexcept(__stl2::data(rng)))
			: span{__stl2::data(rng), Extent}
			{}

			template<__span::compatible<ElementType> Range>
			requires (Extent == dynamic_extent || !__span::has_static_extent<Range>)
			constexpr span(Range&& rng)
			noexcept(noexcept(__stl2::data(rng), __stl2::size(rng)))
			: span{__stl2::data(rng), __span::narrow_cast<index_type>(__stl2::size(rng))}
			{}

			// [span.sub], span subviews
			template<index_type Count>
			constexpr span<element_type, Count> first() const noexcept
			{
				static_assert(Count >= 0,
					"Count of elements to extract cannot be negative.");
				static_assert(Extent == dynamic_extent || Count <= Extent,
					"Count of elements to extract must be less than the static span extent.");
				STL2_EXPECT(size() >= Count);
				STL2_EXPECT(Count == 0 || data_ != nullptr);
				return {data_, Count};
			}
			constexpr span<element_type> first(index_type count) const noexcept
			{
				STL2_EXPECT(count >= 0);
				STL2_EXPECT(size() >= count);
				STL2_EXPECT(count == 0 || data_ != nullptr);
				return {data_, count};
			}

			template<index_type Count>
			constexpr span<element_type, Count> last() const noexcept
			{
				static_assert(Count >= 0,
					"Count of elements to extract cannot be negative.");
				static_assert(Extent == dynamic_extent || Count <= Extent,
					"Count of elements to extract must be less than the static span extent.");
				STL2_EXPECT(size() >= Count);
				STL2_EXPECT((Count == 0 && size() == 0) || data_ != nullptr);
				return {data_ + size() - Count, Count};
			}
			constexpr span<element_type> last(index_type count) const noexcept
			{
				STL2_EXPECT(count >= 0);
				STL2_EXPECT(size() >= count);
				STL2_EXPECT((count == 0 && size() == 0) || data_ != nullptr);
				return {data_ + size() - count, count};
			}

			template<index_type Offset, index_type Count>
			constexpr span<element_type, Count> subspan() const noexcept
			{
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
			constexpr span<element_type, Extent >= Offset ? Extent - Offset : dynamic_extent> subspan() const noexcept
			{
				static_assert(Offset >= 0,
					"Offset of first element to extract cannot be negative.");
				static_assert(Extent == dynamic_extent || Offset <= Extent,
					"Offset of first element to extract must be less than the static span extent.");
				STL2_EXPECT(size() >= Offset);
				STL2_EXPECT((Offset == 0 && size() == 0) || data_ != nullptr);
				return {data_ + Offset, size() - Offset};
			}
			constexpr span<element_type, dynamic_extent> subspan(index_type offset) const noexcept
			{
				STL2_EXPECT(offset >= 0);
				STL2_EXPECT(size() >= offset);
				STL2_EXPECT((offset == 0 && size() == 0) || data_ != nullptr);
				return {data_ + offset, size() - offset};
			}
			constexpr span<element_type, dynamic_extent> subspan(index_type offset, index_type count) const noexcept
			{
				STL2_EXPECT(offset >= 0);
				STL2_EXPECT(count >= 0);
				STL2_EXPECT(size() >= offset + count);
				STL2_EXPECT((offset == 0 && count == 0) || data_ != nullptr);
				return {data_ + offset, count};
			}

			// [span.obs], span observers
			using __span::extent<Extent>::size;
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

			// Extension for P0970
			friend constexpr iterator begin(span s) noexcept { return s.begin(); }
			friend constexpr iterator end(span s) noexcept { return s.end(); }

			// [span.comparison], span comparison operators
			template<EqualityComparableWith<ElementType> U, index_type UExtent>
			bool operator==(span<U, UExtent> that) const
			{
				STL2_EXPECT(!size() || data());
				STL2_EXPECT(!that.size() || that.data());
				return __stl2::equal(*this, that);
			}
			template<EqualityComparableWith<ElementType> U, index_type UExtent>
			bool operator!=(span<U, UExtent> that) const
			{
				return !(*this == that);
			}
			template<StrictTotallyOrderedWith<ElementType> U, index_type UExtent>
			bool operator<(span<U, UExtent> that) const
			{
				STL2_EXPECT(!size() || data());
				STL2_EXPECT(!that.size() || that.data());
				return __stl2::lexicographical_compare(*this, that);
			}
			template<StrictTotallyOrderedWith<ElementType> U, index_type UExtent>
			bool operator>(span<U, UExtent> that)
			{
				return that < *this;
			}
			template<StrictTotallyOrderedWith<ElementType> U, index_type UExtent>
			bool operator<=(span<U, UExtent> that)
			{
				return !(that < *this);
			}
			template<StrictTotallyOrderedWith<ElementType> U, index_type UExtent>
			bool operator>=(span<U, UExtent> that)
			{
				return !(*this < that);
			}

		private:
			ElementType* data_ = nullptr;
		};

#ifdef __cpp_deduction_guides
		template<SizedContiguousRange Rng>
		span(Rng&& rng) -> span<std::remove_pointer_t<data_pointer_t<Rng>>>;

		template<__span::StaticSizedContiguousRange Rng>
		span(Rng&& rng) -> span<std::remove_pointer_t<data_pointer_t<Rng>>,
			__span::static_extent<Rng>::value>;
#endif

		// [span.objectrep], views of object representation
		template<class ElementType, __span::index_t Extent>
		span<const unsigned char, __span::byte_extent<ElementType>(Extent)>
		as_bytes(span<ElementType, Extent> s) noexcept
		{
			return {reinterpret_cast<const unsigned char*>(s.data()), s.size_bytes()};
		}
		template<class ElementType, __span::index_t Extent>
		span<unsigned char, __span::byte_extent<ElementType>(Extent)>
		as_writeable_bytes(span<ElementType, Extent> s) noexcept
		{
			return {reinterpret_cast<unsigned char*>(s.data()), s.size_bytes()};
		}

		template<class ElementType>
		constexpr auto make_span(ElementType* ptr, __span::index_t count) noexcept
		{
			return span<ElementType>{ptr, count};
		}
		template<class ElementType>
		constexpr auto make_span(ElementType* first, ElementType* last) noexcept
		{
			return span<ElementType>{first, last};
		}
		template<SizedContiguousRange Rng>
		constexpr auto make_span(Rng&& rng)
		noexcept(noexcept(__stl2::data(rng), __stl2::size(rng)))
		{
			using S = span<std::remove_pointer_t<data_pointer_t<Rng>>>;
			return S{__stl2::data(rng), __span::narrow_cast<__span::index_t>(__stl2::size(rng))};
		}
		template<__span::StaticSizedContiguousRange Rng>
		constexpr auto make_span(Rng&& rng)
		noexcept(noexcept(__stl2::data(rng)))
		{
			using S = span<std::remove_pointer_t<data_pointer_t<Rng>>,
				__span::static_extent<Rng>::value>;
			return S{__stl2::data(rng), __span::static_extent<Rng>::value};
		}
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
