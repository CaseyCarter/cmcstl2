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
#include <stl2/detail/algorithm/min.hpp>
#include <stl2/detail/concepts/callable.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/common_iterator.hpp>
#include <stl2/detail/iterator/increment.hpp>
#include <stl2/detail/range/concepts.hpp>
#include <stl2/detail/common_tuple.hpp>
#include <stl2/detail/ebo_box.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/type_traits.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>
#include <tuple>
#include <utility>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template <CopyConstructibleObject Fun, View... Rngs>
		requires
			(InputRange<Rngs> && ...) &&
			IndirectInvocable<Fun, iterator_t<Rngs>...>
		class iter_zip_with_view
		: public view_interface<iter_zip_with_view<Fun, Rngs...>>
		, private detail::semiregular_box<Fun>
		, private tuple<Rngs...> {
			using semiregular_base = detail::semiregular_box<Fun>;
			using tuple_base = tuple<Rngs...>;

			using __difference_type = common_type_t<iter_difference_t<iterator_t<Rngs>>...>;
			using __size_type = make_unsigned_t<__difference_type>;

			using semiregular_base::get;

			struct __iterator;
			class __sentinel : tuple<sentinel_t<Rngs>...> {
				using iterator_base = tuple<sentinel_t<Rngs>...>;
			public:
				constexpr __sentinel() = default;

				template <typename X>
				constexpr explicit __sentinel(X&&, tuple<sentinel_t<Rngs>...> ends)
				: tuple<sentinel_t<Rngs>...>(std::move(ends))
				{}

				friend class __iterator;
			};

			using __actual_sentinel = conditional_t<(CommonRange<Rngs> && ...), __iterator, __sentinel>;
		public:
			iter_zip_with_view() = default;

			constexpr explicit iter_zip_with_view(Rngs... rngs)
			: semiregular_base(Fun{}), tuple_base(std::move(rngs)...)
			{}

			constexpr explicit iter_zip_with_view(Fun fun, Rngs... rngs)
			: semiregular_base(std::move(fun)), tuple_base(std::move(rngs)...)
			{}

			constexpr auto begin() requires !(SimpleView<Rngs> && ...)
			{ return begin_impl(*this); }

			constexpr auto begin() const requires (Range<Rngs const> && ...)
			{ return begin_impl(*this); }

			constexpr auto end() requires !(SimpleView<Rngs> && ...)
			{ return end_impl(*this); }

			constexpr auto end() const requires (Range<Rngs const> && ...)
			{ return end_impl(*this); }

			constexpr __size_type size() requires (SizedRange<Rngs> && ...)
			{ return size_impl(*this); }

			constexpr __size_type size() const requires (SizedRange<const Rngs> && ...)
			{ return size_impl(*this); }
		protected:
			constexpr Fun const& fun() const noexcept
			{ return get(); }

			constexpr tuple<Rngs...>& rngs() noexcept requires !(SimpleView<Rngs> && ...)
			{ return static_cast<tuple<Rngs...>&>(*this); }

			constexpr tuple<Rngs...> const& rngs() const noexcept requires (Range<Rngs const> && ...)
			{ return static_cast<tuple<Rngs...> const&>(*this); }
		private:
			template <class Self>
			static constexpr auto begin_impl(Self& self) noexcept
			{
				return __iterator(
					self.fun(),
					detail::tuple_transform(self.rngs(), __stl2::begin)
				);
			}

			template <class Self>
			static constexpr auto end_impl(Self& self) noexcept
			{
				return __actual_sentinel(
					self.fun(),
					detail::tuple_transform(self.rngs(), __stl2::end)
				);
			}

			template <class Self>
			static constexpr auto size_impl(Self& self) requires (SizedRange<Rngs> && ...)
			{
				return detail::tuple_foldl(
					detail::tuple_transform(static_cast<__maybe_const<is_const_v<Self>, tuple_base>&>(self), __stl2::size),
					(std::numeric_limits<__size_type>::max)(),
					[](auto const& a, auto const& b) -> decltype(__stl2::min(a, b)) {
						return __stl2::min(a, b); });
			}
		};

		template <class Fun, class... Rngs>
		iter_zip_with_view(Fun, Rngs...) -> iter_zip_with_view<Fun, all_view<Rngs>...>;

		template <class Fun, class... Rngs>
		class iter_zip_with_view<Fun, Rngs...>::__iterator
		: tuple<iterator_t<Rngs>...> {
		public:
			using difference_type = common_type_t<iter_difference_t<iterator_t<Rngs>>...>;
			using value_type = decay_t<indirect_result_t<Fun&, iterator_t<Rngs>...>>;
			using iterator_category = common_type_t<iterator_category_t<iterator_t<Rngs>>...>;
		private:
			using iterator_base = tuple<iterator_t<Rngs>...>;
			using __sentinel = iter_zip_with_view<Fun, Rngs...>::__sentinel;
			using __actual_sentinel = iter_zip_with_view<Fun, Rngs...>::__actual_sentinel;
			Fun const* fun_; // TODO: EBO me
								  // TODO: possibly remove const

			template <class Self, size_t... Is>
			static constexpr decltype(auto) deref_all(Self& self, index_sequence<Is...>)
			{
				auto iterator_self = static_cast<__maybe_const<is_const_v<Self>, iterator_base>&>(self);
				return std::tie((*std::get<Is>(iterator_self))...);
			}

			template <class Self>
			static constexpr decltype(auto) read_impl(Self& self)
			STL2_NOEXCEPT_RETURN(
				std::apply(*self.fun_, deref_all(self, index_sequence_for<Rngs...>{}))
			)

			template <class Self>
			static constexpr decltype(auto) subscript_impl(Self& self, difference_type n)
			STL2_NOEXCEPT_RETURN(
				*(self + n)
			)

			template <class T1, class T2, Predicate<T1, T2> Comp>
			static constexpr bool relation_impl(T1 const& t1, T2 const& t2, Comp comp)
			{
				return detail::tuple_foldl(
					detail::tuple_transform(static_cast<typename T1::iterator_base const&>(t1),
													static_cast<typename T2::iterator_base const&>(t2),
													comp),
					false,
					logical_or<>{}
				);
			}

			static constexpr difference_type distance_impl(__iterator const& a, __actual_sentinel const& b)
			{
				auto abs = [](auto const x) noexcept { return x < 0 ? -x : x; };
				return detail::tuple_min(
					detail::tuple_transform(
						static_cast<iterator_base const&>(a),
						static_cast<typename __actual_sentinel::iterator_base const&>(b),
						[](auto const& a, auto const& b) { return __stl2::distance(a, b); }),
					[&abs](auto const a, auto const b) noexcept { return abs(a) < abs(b); });
			}

			template <class T1, class T2>
			static constexpr bool equal_impl(T1 const& t1, T2 const& t2)
			{ return relation_impl(t1, t2, [](auto const& a, auto const& b){ return a == b; }); }

			static constexpr bool less_impl(__iterator const& a, __iterator const& b)
			{ return relation_impl(a, b, [](auto const& a, auto const& b){ return a < b; }); }

			template <class F>
			constexpr void advance_impl(F f)
			{ detail::tuple_for_each(static_cast<iterator_base&>(*this), std::move(f)); }
		public:
			constexpr __iterator() = default;
			constexpr __iterator(Fun const& fun, std::tuple<iterator_t<Rngs>...> its)
			: iterator_base(std::move(its)), fun_(std::addressof(fun))
			{}

			constexpr decltype(auto) operator*()
			noexcept(noexcept(read_impl(std::declval<__iterator&>())))
			{ return read_impl(*this); }

			constexpr decltype(auto) operator*() const
			noexcept(noexcept(read_impl(std::declval<__iterator&>())))
			requires (Range<Rngs const> && ...)
			{ return read_impl(*this); }

			constexpr __iterator& operator++()
			{
				advance_impl([](auto& x){ ++x; });
				return *this;
			}

			constexpr __iterator operator++(int)
			{
				auto result = *this;
				++*this;
				return result;
			}

			constexpr __iterator& operator--() requires (BidirectionalRange<Rngs> && ...)
			{
				advance_impl([](auto& x){ return --x; });
				return *this;
			}

			constexpr __iterator operator--(int) requires (BidirectionalRange<Rngs> && ...)
			{
				auto result = *this;
				--*this;
				return result;
			}

			constexpr decltype(auto) operator[](difference_type n)
			requires (RandomAccessRange<Rngs> && ...)
			{ return subscript_impl(*this, n); }

			constexpr decltype(auto) operator[](difference_type n) const
			requires (RandomAccessRange<const Rngs> && ...)
			{ return subscript_impl(*this, n); }

			constexpr __iterator& operator+=(difference_type n)
			requires (RandomAccessRange<Rngs> && ...)
			{
				advance_impl([n](auto& x){ return __stl2::advance(x, n); });
				return *this;
			}

			constexpr __iterator& operator-=(difference_type n)
			requires (RandomAccessRange<Rngs> && ...)
			{
				advance_impl([n](auto& x){ return __stl2::advance(x, -n); });
				return *this;
			}

			friend constexpr __iterator operator+(__iterator a, difference_type n)
			requires (RandomAccessRange<Rngs> && ...)
			{ return (a += n); }

			friend constexpr __iterator operator+(difference_type n, __iterator a)
			requires (RandomAccessRange<Rngs> && ...)
			{ return (a += n); }

			friend constexpr __iterator operator-(__iterator a, difference_type n)
			requires (RandomAccessRange<Rngs> && ...)
			{ return (a -= n); }

			friend constexpr difference_type operator-(const __iterator& first, const __actual_sentinel& last)
			requires (SizedSentinel<iterator_t<Rngs>, sentinel_t<Rngs>> && ...)
			{ return distance_impl(last, first); }

			friend constexpr difference_type operator-(const __sentinel& last, const __iterator& first)
			requires !Same<__iterator, __actual_sentinel> &&
				(SizedSentinel<iterator_t<Rngs>, sentinel_t<Rngs>> && ...)
			{ return -(first - last); }

			constexpr friend bool operator==(const __iterator& a, const __iterator& b)
			requires (Sentinel<iterator_t<Rngs>, iterator_t<Rngs>> && ...)
			{ return equal_impl(a, b); }

			constexpr friend bool operator==(const __iterator& a, const __sentinel& b)
			{ return equal_impl(a, b); }

			constexpr friend bool operator==(const __sentinel& a, const __iterator& b)
			{ return (b == a); }

			constexpr friend bool operator==(const __iterator& a, const default_sentinel b)
			{ return equal_impl(a, b); }

			constexpr friend bool operator==(const default_sentinel a, const __iterator& b)
			{ return (b == a); }

			constexpr friend bool operator!=(const __iterator& a, const __iterator& b)
			requires (Sentinel<iterator_t<Rngs>, iterator_t<Rngs>> && ...)
			{ return !(a == b); }

			constexpr friend bool operator!=(const __iterator& a, const __sentinel& b)
			{ return !(a == b); }

			constexpr friend bool operator!=(const __sentinel& a, const __iterator& b)
			{ return !(b == a); }

			constexpr friend bool operator!=(const __iterator& a, const default_sentinel b)
			{ return !(a == b); }

			constexpr friend bool operator!=(const default_sentinel a, const __iterator& b)
			{ return !(b == a); }

			constexpr friend bool operator<(const __iterator& a, const __iterator& b)
			requires (RandomAccessRange<Rngs> && ...)
			{ return less_impl(a, b); }

			constexpr friend bool operator>(const __iterator& a, const __iterator& b)
			requires (RandomAccessRange<Rngs> && ...)
			{ return b < a; }

			constexpr friend bool operator<=(const __iterator& a, const __iterator& b)
			requires (RandomAccessRange<Rngs> && ...)
			{ return !(a > b); }

			constexpr friend bool operator>=(const __iterator& a, const __iterator& b)
			requires (RandomAccessRange<Rngs> && ...)
			{ return !(a < b); }
		};

		template <CopyConstructibleObject Fun, View... Rngs>
		requires
			(InputRange<Rngs> && ...) &&
			IndirectInvocable<Fun, iterator_t<Rngs>...>
		struct zip_with_view : iter_zip_with_view<Fun, Rngs...> {
			zip_with_view() = default;

			constexpr explicit zip_with_view(Rngs... rngs)
				: iter_zip_with_view<Fun, Rngs...>{
					{Fun{}}, std::move(rngs)...}
			{}

			constexpr explicit zip_with_view(Fun fun, Rngs... rngs)
				: iter_zip_with_view<Fun, Rngs...>{
					std::move(fun), std::move(rngs)...}
			{}
		};

		template <class Fun, class... Rngs>
		zip_with_view(Fun, Rngs...) -> zip_with_view<Fun, all_view<Rngs>...>;
	} // namespace ext

	template <class Fun, class... Rngs>
	struct enable_view<ext::zip_with_view<Fun, Rngs...>> : std::true_type {};

	namespace view::ext {
		struct __iter_zip_with_fn {
			template <class Fun, class... Rngs>
			constexpr auto operator()(Fun fun, Rngs&&... rngs) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::iter_zip_with_view{std::move(fun), all(static_cast<Rngs&&>(rngs))...}
			)
		};

		inline constexpr __iter_zip_with_fn iter_zip_with {};

		struct __zip_with_fn {
			template <class Fun, class... Rngs>
			constexpr auto operator()(Fun fun, Rngs&&... rngs) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::zip_with_view{std::move(fun), all(static_cast<Rngs&&>(rngs))...}
			)
		};

		inline constexpr __zip_with_fn zip_with {};

		class __zip_fn {
		public:
			template <class... Rngs>
			constexpr auto operator()(Rngs&&... rngs) const
			{
				return zip_with(
					[](auto&&... xs){
						return common_tuple<decltype(xs)...>(static_cast<decltype(xs)>(xs)...);
					},
					static_cast<Rngs&&>(rngs)...); 
			}
		};

		inline constexpr __zip_fn zip {};
	}
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_ZIP_WITH_HPP
