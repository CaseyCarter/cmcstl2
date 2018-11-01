// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Eric Niebler
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_ZIP_WITH_HPP
#define STL2_VIEW_ZIP_WITH_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/common_tuple.hpp>
#include <stl2/detail/algorithm/max.hpp>
#include <stl2/detail/algorithm/min.hpp>
#include <stl2/detail/iterator/increment.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/all.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<Range R>
		using range_value_t = iter_value_t<iterator_t<R>>;

		template<Range R>
		using range_difference_t = iter_difference_t<iterator_t<R>>;

		template<Range R>
		using range_reference_t = iter_reference_t<iterator_t<R>>;

		template<class T, bool is_const = false>
		using __semiregular_ref_or_val_t = detail::semiregular_box<T>;

		template<class F, class... Rs>
		concept bool __zippable =
			(InputRange<Rs> && ...) &&
			Invocable<F&, iterator_t<Rs>...>;

		template<class F, class... Rs>
		requires (ViewableRange<Rs> && ...) && Invocable<F&, range_reference_t<Rs>&&...>
		struct __iter_zip_with_view
		: private detail::semiregular_box<F>
		, private tuple<Rs...> {
		private:
			using difference_type = common_type_t<range_difference_t<Rs>...>;
			using size_type = make_unsigned_t<difference_type>;

			template<bool is_const>
			struct __cursor;

			template<bool is_const>
			using __iterator = basic_iterator<__cursor<is_const>>;

			template<bool is_const>
			struct __sentinel;

			template<bool is_const>
			using __end_cursor_t = conditional_t<
				(CommonRange<__maybe_const<is_const, Rs>> && ...) &&
					(ForwardRange<__maybe_const<is_const, Rs>> && ...),
				__iterator<is_const>,
				__sentinel<is_const>
			>;

			using detail::semiregular_box<F>::get;

			template<class Self>
			[[nodiscard]] static constexpr auto&& __as_tuple(Self&& self)
			{
				using __tuple_base = tuple<Rs...>;
				if constexpr (is_lvalue_reference_v<Self>) {
					if constexpr (is_const_v<remove_reference_t<Self>>) {
						return static_cast<__tuple_base const&>(self);
					}
					else {
						return static_cast<__tuple_base&>(self);
					}
				}
				else if (is_lvalue_reference_v<Self>) {
					return static_cast<__tuple_base&&>(self);
				}
			}

			template<bool is_const, class Self>
			[[nodiscard]] static constexpr __iterator<is_const> begin_impl([[maybe_unused]] Self&& self)
			{
				return __iterator<is_const>(
					__cursor<is_const>(
						self.get(),
						detail::tuple_transform(__as_tuple(self), __stl2::begin)
					)
				);
			}

			template<bool is_const, class Self>
			[[nodiscard]] static constexpr __end_cursor_t<is_const> end_impl([[maybe_unused]] Self&& self)
			{
				if constexpr (Same<__end_cursor_t<is_const>, __iterator<is_const>>) {
					return __iterator<is_const>(
						__cursor<is_const>(
							self.get(),
							detail::tuple_transform(__as_tuple(self), __stl2::end)
						)
					);
				}
				else {
					return __sentinel<is_const>(
						self.get(),
						detail::tuple_transform(__as_tuple(self), __stl2::end)
					);
				}
			}
		public:
			void foo() {
				__iterator<false>{};
			}
			__iter_zip_with_view() = default;

			constexpr explicit __iter_zip_with_view(Rs... rs)
			: __iter_zip_with_view::semiregular_box(F{})
			, __iter_zip_with_view::tuple(std::move(rs)...)
			{}

			constexpr explicit __iter_zip_with_view(F f, Rs... rs)
			: __iter_zip_with_view::semiregular_box(std::move(f))
			, __iter_zip_with_view::tuple(std::move(rs)...)
			{}

			constexpr size_type size() const
			requires (SizedRange<Rs const> && ...)
			{
				return detail::tuple_foldl(
					detail::tuple_transform(__as_tuple(*this), __stl2::size),
					numeric_limits<size_type>::max(),
					__stl2::min
				);
			}

			constexpr __iterator<false> begin()
			{ return __iter_zip_with_view::begin_impl<false>(*this); }

			constexpr __iterator<true> begin() const
			requires (Range<Rs const> && ...) && (__zippable<F, Rs const> && ...)
			{ return __iter_zip_with_view::begin_impl<true>(*this); }

			constexpr auto cbegin() const
			requires (Range<Rs const> && ...) && (__zippable<F, Rs const> && ...)
			{ return begin(); }

			constexpr __end_cursor_t<false> end()
			{ return __iter_zip_with_view::end_impl<false>(*this); }

			constexpr __end_cursor_t<true> end() const
			requires (Range<Rs const> && ...) && (__zippable<F, Rs const> && ...)
			{ return __iter_zip_with_view::end_impl<true>(*this); }

			constexpr auto cend() const
			requires (Range<Rs const> && ...) && (__zippable<F, Rs const> && ...)
			{ return end(); }
		};

		template<class F, class... Rs>
		template<bool is_const>
		struct __iter_zip_with_view<F, Rs...>::__sentinel
		: private tuple<sentinel_t<__maybe_const<is_const, Rs>>...> {
		private:
			friend struct __cursor<is_const>;
			friend struct __sentinel<!is_const>;
			using __tuple_base = tuple<sentinel_t<__maybe_const<is_const, Rs>>...>;

			template<class Self>
			[[nodiscard]] static constexpr auto&& __as_tuple(Self&& self)
			{
				if constexpr (is_lvalue_reference_v<Self>) {
					if constexpr (is_const_v<remove_reference_t<Self>>) {
						return static_cast<__tuple_base const&>(self);
					}
					else {
						return static_cast<__tuple_base&>(self);
					}
				}
				else if (is_lvalue_reference_v<Self>) {
					return static_cast<__tuple_base&&>(self);
				}
			}
		public:
			constexpr __sentinel() = default;
			constexpr __sentinel(__tuple_base ends)
				: __sentinel::tuple(std::move(ends))
			{}

			template<bool other_const>
			requires is_const && !other_const
			constexpr __sentinel(__sentinel<other_const> that)
				: __sentinel::tuple(std::move(__as_tuple(that)))
			{}
		};

		template<class F, class... Rs>
		template<bool is_const>
		struct __iter_zip_with_view<F, Rs...>::__cursor
		: private __semiregular_ref_or_val_t<F, is_const>
		, private tuple<iterator_t<__maybe_const<is_const, Rs>>...> {
		private:
			using __function_base = __semiregular_ref_or_val_t<F, is_const>;
			using __tuple_base = tuple<iterator_t<__maybe_const<is_const, Rs>>...>;

			using __function_base::get;

			friend struct __cursor<!is_const>;

			template<class Self>
			[[nodiscard]] static constexpr auto&& __as_tuple(Self&& self)
			{
				if constexpr (is_lvalue_reference_v<Self>) {
					if constexpr (is_const_v<remove_reference_t<Self>>) {
						return static_cast<__tuple_base const&>(self);
					}
					else {
						return static_cast<__tuple_base&>(self);
					}
				}
				else if (is_lvalue_reference_v<Self>) {
					return static_cast<__tuple_base&&>(self);
				}
			}
		public:
			using difference_type = common_type_t<range_difference_t<__maybe_const<is_const, Rs>>...>;
			using value_type =
				decay_t<indirect_result_t<F&, iterator_t<__maybe_const<is_const, Rs>>...>>;

			__cursor() = default;

			constexpr __cursor(__function_base f, __tuple_base is)
			: __function_base(std::move(f))
			, __tuple_base(std::move(is))
			{}

			template<bool other_const = false>
			requires is_const && !other_const
			constexpr __cursor(__cursor<other_const> that)
			: __function_base(std::move(that.get()))
			, __tuple_base(std::move(__as_tuple(that)))
			{}

			constexpr decltype(auto) read() const
			{
				return std::apply([this](auto&&... xs) {
					return __stl2::invoke(get(), *std::forward<decltype(xs)>(xs)...);
				}, __as_tuple(*this));
			}

			constexpr void next()
			{
				detail::tuple_for_each(__as_tuple(*this), [](auto& x){ ++x; });
			}

			constexpr bool equal(__cursor const& that) const
			requires (Sentinel<
					iterator_t<__maybe_const<is_const, Rs>>,
					iterator_t<__maybe_const<is_const, Rs>>> && ...)
			{
				// By returning true if *any* of the iterators are equal, we allow
				// zipped ranges to be of different lengths, stopping when the first
				// one reaches the end.
				return detail::tuple_foldl(
					detail::tuple_transform(__as_tuple(*this), __as_tuple(that), equal_to<>{}),
					false,
					logical_or<>{}
				);
			}

			constexpr bool equal(__sentinel<is_const> const& s) const
			{
				// By returning true if *any* of the iterators are equal, we allow
				// zipped ranges to be of different lengths, stopping when the first
				// one reaches the end.
				return detail::tuple_foldl(
					detail::tuple_transform(__as_tuple(*this), s.__as_tuple(s), equal_to<>{}),
					false,
					logical_or<>{}
				);
			}

			constexpr void prev()
			requires (BidirectionalRange<__maybe_const<is_const, Rs>> && ...)
			{
				detail::tuple_for_each(__as_tuple(*this), [](auto& x){ --x; });
			}

			constexpr void advance(difference_type n)
			requires (RandomAccessRange<__maybe_const<is_const, Rs>> && ...)
			{
				detail::tuple_for_each(__as_tuple(*this), [n](auto& x){ __stl2::advance(x, n); });
			}

			constexpr difference_type distance_to(__cursor const& that) const
			requires (SizedSentinel<
					iterator_t<__maybe_const<is_const, Rs>>,
					iterator_t<__maybe_const<is_const, Rs>>> && ...)
			{
				// Return the smallest distance (in magnitude) of any of the iterator
				// pairs. This is to accommodate zippers of sequences of different length.
				if (0 < std::get<0>(__as_tuple(that)) - std::get<0>(*this)) {
					return detail::tuple_foldl(
						detail::tuple_transform(__as_tuple(*this), __as_tuple(that), __stl2::distance),
						numeric_limits<difference_type>::max(),
						__stl2::min
					);
				}
				else {
					return detail::tuple_foldl(
						detail::tuple_transform(__as_tuple(*this), __as_tuple(that), __stl2::distance),
						numeric_limits<difference_type>::min(),
						__stl2::max
					);
				}
			}

			constexpr auto indirect_move()
			requires !is_const
			{
				return detail::tuple_transform(__as_tuple(*this), __stl2::iter_move);
			}

			constexpr void indirect_swap(__cursor<false>& that)
			requires !is_const
			{
				*this = detail::tuple_transform(__as_tuple(*this), __as_tuple(that), __stl2::iter_swap);
			}
		};

		template<class F, class... Rs>
		struct zip_with_view : __iter_zip_with_view<F, Rs...> {
			zip_with_view() = default;

			constexpr explicit zip_with_view(Rs... rs)
			: __iter_zip_with_view<F, Rs...>(std::move(rs)...)
			{}

			constexpr explicit zip_with_view(F f, Rs... rs)
			: __iter_zip_with_view<F, Rs...>(std::move(f), std::move(rs)...)
			{}
		};

		template<class F, class... Rs>
		zip_with_view(F, Rs...) -> zip_with_view<F, all_view<Rs>...>;
	} // namespace ext

	namespace view::ext {
		struct __iter_zip_with_fn {
			template<class... Rs, class F>
			requires __stl2::ext::__zippable<F, Rs...>
			auto operator()(F f, Rs&&... rs) const
			{
				return __stl2::ext::__iter_zip_with_view<F, all_view<Rs>...>(
					std::move(f),
					all(std::forward<Rs>(rs))...
				);
			}
		};

		inline constexpr __iter_zip_with_fn iter_zip_with {};

		struct __zip_with_fn {
			template<class F, class... Rs>
			requires
				(InputRange<Rs> && ...) &&
				CopyConstructible<F> &&
				Invocable<F&, __stl2::ext::range_reference_t<Rs>&&...>
			constexpr auto operator()(F f, Rs&&... rs) const
			{
				return __stl2::ext::zip_with_view(std::move(f), all(std::forward<Rs>(rs))...);
			}
		};

		inline constexpr __zip_with_fn zip_with {};

		struct __zip_fn {
			template<typename... Rs>
			requires (Range<Rs> && ...)
			constexpr auto operator()(Rs&&... rs) const
			{
				return zip_with(
					[](auto&&... xs) {
						return common_tuple<decltype(xs)...>(std::forward<decltype(xs)>(xs)...);
					},
					std::forward<Rs>(rs)...
				);
			}
		};

		inline constexpr __zip_fn zip {};
	} // namespace view::ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_ZIP_WITH_HPP
