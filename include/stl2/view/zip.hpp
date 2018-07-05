// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2018
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_VIEW_ZIP_HPP
#define STL2_VIEW_ZIP_HPP

#include <tuple>
#include <type_traits>
#include <utility>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/algorithm/max.hpp>
#include <stl2/detail/algorithm/min.hpp>
#include <stl2/detail/iterator/default_sentinel.hpp>
#include <stl2/detail/memory/addressof.hpp>
#include <stl2/view/all.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	template<WeaklyIncrementable... Ts>
	using __common_difference_type_t =
		meta::_t<meta::if_c<sizeof...(Ts) != 0,
			common_type<difference_type_t<Ts>...>,
			meta::id<int>>>;

	namespace ext {
		template<View... Ranges>
		struct __zip_view_base {
			std::tuple<iterator_t<Ranges>...> current_ {};
		};
		template<View... Ranges>
		requires (ForwardRange<Ranges> && ...)
		struct __zip_view_base<Ranges...> {};

		template<View... Ranges>
		struct zip_view
		: ext::view_interface<zip_view<Ranges...>>
		, private __zip_view_base<Ranges...>
		{
		private:
			template <bool Const, class Indices> struct __zipperator;
			template <bool Const, class Indices> struct __sentinel;
			using Indices = std::index_sequence_for<Ranges...>;
			static constexpr bool all_simple = (ext::SimpleView<Ranges> && ...);
			static constexpr bool all_forward = (ForwardRange<Ranges> && ...);
			static constexpr bool all_forward_const = (ForwardRange<const Ranges> && ...);
			static constexpr bool all_sized = (SizedRange<Ranges> && ...);
			static constexpr bool all_sized_const = (SizedRange<const Ranges> && ...);

			std::tuple<Ranges...> ranges_ {};

			template<std::size_t... Is, class Self>
			static constexpr auto size_(std::index_sequence<Is...>, Self& self)
			{
				if constexpr (sizeof...(Ranges) != 0) {
					static_assert(Same<std::index_sequence<Is...>, Indices>);
					using D = __common_difference_type_t<iterator_t<Ranges>...>;
					return __stl2::min(static_cast<D>(__stl2::size(std::get<Is>(self.ranges_)))...);
				} else {
					return 0;
				}
			}
			template<std::size_t... Is>
			constexpr void begin_(std::index_sequence<Is...>)
			{ (((void)(std::get<Is>(this->current_) = __stl2::begin(std::get<Is>(ranges_)))), ...); }
		public:
			zip_view() = default;
			constexpr zip_view(Ranges... ranges)
			noexcept((std::is_nothrow_move_constructible_v<Ranges> && ...))
			requires sizeof...(Ranges) != 0
			: ranges_(std::move(ranges)...) {}

			constexpr auto begin() {
				if constexpr (!all_forward) begin_(Indices{});
				return __zipperator<all_simple && all_forward, Indices>{*this};
			}
			constexpr auto begin() const requires all_forward_const
			{ return __zipperator<true, Indices>{*this}; }

			constexpr auto end() requires all_forward
			{
				if constexpr ((RandomAccessRange<Ranges> && ...) && all_sized)
					return __zipperator<all_simple, Indices>{*this, size_(Indices{}, *this)};
				else
					return __sentinel<all_simple, Indices>{*this};
			}
			constexpr auto end() const
			{
				if constexpr ((RandomAccessRange<const Ranges> && ...) && all_sized_const)
					return __zipperator<true, Indices>{*this, size_(Indices{}, *this)};
				else if constexpr (all_forward && all_forward_const)
					return __sentinel<true, Indices>{*this};
				else
					return default_sentinel{};
			}

			constexpr auto size() requires all_sized
			{ return size_(Indices{}, *this); }
			constexpr auto size() const requires all_sized_const
			{ return size_(Indices{}, *this); }
		};

		template <class... Ranges>
		zip_view(Ranges&&...) -> zip_view<ext::all_view<Ranges>...>;

		// Writable wants const proxy references to be assignable, so we need a
		// tuple-like type with const-qualified assignment operators
		template<class... Ts>
		struct __tuple_hack : std::tuple<Ts...> {
		private:
			template<class... Us>
			requires sizeof...(Us) == sizeof...(Ts) &&
				(Assignable<const Ts&, const Us&> && ...)
			static constexpr bool assignable_hack = true;

			template<class Other, std::size_t... Is>
			requires Same<std::index_sequence_for<Ts...>, std::index_sequence<Is...>>
			const __tuple_hack& assign_(Other&& other, std::index_sequence<Is...>) const {
				((void)(std::get<Is>(*this) =
					std::get<Is>(static_cast<Other&&>(other))), ...);
				return *this;
			}
		public:
			using std::tuple<Ts...>::tuple;
			__tuple_hack(const std::tuple<Ts...>& t)
			: std::tuple<Ts...>{t} {}
			__tuple_hack(std::tuple<Ts...>&& t)
			: std::tuple<Ts...>{static_cast<std::tuple<Ts...>&&>(t)} {}

			__tuple_hack(const __tuple_hack&) = default;
			__tuple_hack(__tuple_hack&&) = default;
			__tuple_hack& operator=(const __tuple_hack&) = default;
			__tuple_hack& operator=(__tuple_hack&&) = default;

			using std::tuple<Ts...>::operator=;

			const __tuple_hack& operator=(const std::tuple<Ts...>& t) const
			requires (Assignable<const Ts&, const Ts&> && ...) {
				using Indices = std::index_sequence_for<Ts...>;
				return assign_(t, Indices{});
			}
			const __tuple_hack& operator=(std::tuple<Ts...>&& t) const
			requires (Assignable<const Ts&, Ts> && ...) {
				using Indices = std::index_sequence_for<Ts...>;
				return assign_(static_cast<std::tuple<Ts...>&&>(t), Indices{});
			}
			template<class... Us>
			requires assignable_hack<Us...>
			const __tuple_hack& operator=(const std::tuple<Us...>& t) const {
				using Indices = std::index_sequence_for<Ts...>;
				return assign_(t, Indices{});
			}
			template<class... Us>
			requires assignable_hack<Us...>
			const __tuple_hack& operator=(std::tuple<Us...>&& t) const {
				using Indices = std::index_sequence_for<Ts...>;
				return assign_(static_cast<std::tuple<Us...>&&>(t), Indices{});
			}
		};

		template<class... Ts>
		__tuple_hack(Ts&&...) -> __tuple_hack<Ts&&...>;

		template<bool, View... Ranges>
		struct __zipperator_base {
			zip_view<Ranges...>* parent_ = nullptr;
		};
		template<bool Const, View... Ranges>
		requires (ForwardRange<__maybe_const<Const, Ranges>> && ...)
		struct __zipperator_base<Const, Ranges...> {
			std::tuple<iterator_t<__maybe_const<Const, Ranges>>...> current_ {};
		};

		template<bool, View... Ranges>
		struct __zipperator_vt_base {};
		template<bool Const, View... Ranges>
		requires (InputRange<__maybe_const<Const, Ranges>> && ...)
		struct __zipperator_vt_base<Const, Ranges...> {
			using value_type = std::tuple<
				value_type_t<iterator_t<__maybe_const<Const, Ranges>>>...>;
		};

		template<View... Ranges>
		template<bool Const, std::size_t... Is>
		struct zip_view<Ranges...>::__zipperator<Const, std::index_sequence<Is...>>
		: private __zipperator_base<Const, Ranges...>
		, __zipperator_vt_base<Const, Ranges...>
		{
		private:
			using Parent = __maybe_const<Const, zip_view<Ranges...>>;
			using Sent = typename zip_view<Ranges...>::template
				__sentinel<Const, std::index_sequence<Is...>>;

			static constexpr bool all_random_access =
				(RandomAccessRange<__maybe_const<Const, Ranges>> && ...);
			static constexpr bool all_bidi =
				(BidirectionalRange<__maybe_const<Const, Ranges>> && ...);
			static constexpr bool all_forward =
				(ForwardRange<__maybe_const<Const, Ranges>> && ...);
			static constexpr bool all_input =
				(InputRange<__maybe_const<Const, Ranges>> && ...);

			static_assert(!Const || all_forward);
			static_assert(Same<std::index_sequence_for<Ranges...>,
				std::index_sequence<Is...>>);

			auto& current() noexcept requires all_forward
			{ return this->current_; }
			auto& current() const noexcept {
				if constexpr (all_forward)
					return this->current_;
				else
					return this->parent_->current_;
			}

		public:
			using iterator_category =
				meta::if_c<all_random_access, random_access_iterator_tag,
				meta::if_c<all_bidi, bidirectional_iterator_tag,
				meta::if_c<all_forward, forward_iterator_tag,
				meta::if_c<all_input, input_iterator_tag,
				output_iterator_tag>>>>;
			using difference_type = __common_difference_type_t<
				iterator_t<__maybe_const<Const, Ranges>>...>;

			__zipperator() = default;
			explicit constexpr __zipperator(Parent& parent)
			: __zipperator_base<Const, Ranges...>{detail::addressof(parent)}
			{}
			constexpr __zipperator(Parent& parent)
			requires all_forward
			: __zipperator_base<Const, Ranges...>{{
				(__stl2::begin(std::get<Is>(parent.ranges_)))...
			}} {}
			constexpr __zipperator(Parent& parent, [[maybe_unused]] difference_type n)
			requires all_random_access
			: __zipperator_base<Const, Ranges...>{{
				(__stl2::begin(std::get<Is>(parent.ranges_)) +
					static_cast<difference_type_t<iterator_t<__maybe_const<Const, Ranges>>>>(n))...
			}} {}

			constexpr auto operator*()
			{ return __tuple_hack{*std::get<Is>(current())...}; }
			constexpr auto operator*() const
			requires (detail::Dereferenceable<const iterator_t<__maybe_const<Const, Ranges>>> && ...)
			{ return __tuple_hack{*std::get<Is>(current())...}; }

			constexpr __zipperator& operator++() {
				((void)++std::get<Is>(current()), ...);
				return *this;
			}
			constexpr auto operator++(int) {
				if constexpr (all_forward) {
					auto tmp = *this;
					++*this;
					return tmp;
				} else
					++*this;
			}

			constexpr __zipperator& operator--()
			requires all_bidi
			{
				((void)--std::get<Is>(current()), ...);
				return *this;
			}
			constexpr __zipperator operator--(int)
			requires all_bidi
			{
				auto tmp = *this;
				--*this;
				return tmp;
			}

			friend constexpr bool operator==(const __zipperator& x, const __zipperator& y)
			requires all_forward
			{ return x.current_ == y.current_; }
			friend constexpr bool operator!=(const __zipperator& x, const __zipperator& y)
			requires all_forward
			{ return !(x == y); }

			friend constexpr bool operator==(const __zipperator& x, default_sentinel)
			requires !all_forward
			{ return ((std::get<Is>(x.current()) == __stl2::end(std::get<Is>(x.parent_->ranges_))) || ...); }
			friend constexpr bool operator==(default_sentinel y, const __zipperator& x)
			requires !all_forward
			{ return (x == y); }
			friend constexpr bool operator!=(const __zipperator& x, default_sentinel y)
			requires !all_forward
			{ return !(x == y); }
			friend constexpr bool operator!=(default_sentinel y, const __zipperator& x)
			requires !all_forward
			{ return !(y == x); }

			friend constexpr bool operator==(const __zipperator& x, const Sent& y)
			requires all_forward
			{ return ((std::get<Is>(current()) == std::get<Is>(y.last_)) || ...); }
			friend constexpr bool operator==(const Sent& y, const __zipperator& x)
			requires all_forward
			{ return (x == y); }
			friend constexpr bool operator!=(const __zipperator& x, const Sent& y)
			requires all_forward
			{ return !(x == y); }
			friend constexpr bool operator!=(const Sent& y, const __zipperator& x)
			requires all_forward
			{ return !(y == x); }

			friend constexpr bool operator<(const __zipperator& x, const __zipperator& y)
			requires all_random_access
			{ return x.current_ < y.current_; }
			friend constexpr bool operator>(const __zipperator& x, const __zipperator& y)
			requires all_random_access
			{ return y < x; }
			friend constexpr bool operator<=(const __zipperator& x, const __zipperator& y)
			requires all_random_access
			{ return !(y < x); }
			friend constexpr bool operator>=(const __zipperator& x, const __zipperator& y)
			requires all_random_access
			{ return !(x < y); }

			constexpr __zipperator& operator+=([[maybe_unused]] difference_type n)
			requires all_random_access
			{
				((void)(std::get<Is>(current()) += n), ...);
				return *this;
			}
			friend constexpr __zipperator operator+(const __zipperator& x, difference_type n)
			requires all_random_access
			{
				auto tmp = x;
				tmp += n;
				return tmp;
			}
			friend constexpr __zipperator operator+(difference_type n, const __zipperator& x)
			requires all_random_access
			{ return x + n; }

			constexpr __zipperator& operator-=([[maybe_unused]] difference_type n)
			requires all_random_access
			{
				((void)(std::get<Is>(current()) -= n), ...);
				return *this;
			}
			friend constexpr __zipperator operator-(const __zipperator& x, difference_type n)
			requires all_random_access
			{
				auto tmp = x;
				tmp -= n;
				return tmp;
			}
			friend constexpr difference_type operator-(const __zipperator& x, const __zipperator& y)
			requires (SizedSentinel<iterator_t<__maybe_const<Const, Ranges>>,
				iterator_t<__maybe_const<Const, Ranges>>> && ...)
			{
				if constexpr (sizeof...(Ranges) != 0) {
					return __stl2::max(static_cast<difference_type>(
						std::get<Is>(x.current()) - std::get<Is>(y.current()))...);
				} else {
					return 0;
				}
			}

			constexpr auto operator[](difference_type n) const
			requires all_random_access
			{ return *(*this + n); }

			friend constexpr auto iter_move(const __zipperator& x)
			noexcept(noexcept(((void)__stl2::iter_move(
				std::declval<const iterator_t<__maybe_const<Const, Ranges>>&>()), ...)))
			requires ((Readable<iterator_t<__maybe_const<Const, Ranges>>> && ...))
			{ return __tuple_hack{__stl2::iter_move(std::get<Is>(x.current()))...}; }

			friend constexpr void iter_swap(const __zipperator& x, const __zipperator& y)
			noexcept(noexcept((__stl2::iter_swap(
				std::declval<const iterator_t<__maybe_const<Const, Ranges>>&>(),
				std::declval<const iterator_t<__maybe_const<Const, Ranges>>&>()), ...)))
			requires (IndirectlySwappable<iterator_t<__maybe_const<Const, Ranges>>> && ...)
			{ (__stl2::iter_swap(std::get<Is>(x.current()), std::get<Is>(y.current())), ...); }
		};

		template<View... Ranges>
		template<bool Const, std::size_t... Is>
		struct zip_view<Ranges...>::__sentinel<Const, std::index_sequence<Is...>> {
		private:
			using Iter = typename zip_view<Ranges...>::template
				__zipperator<Const, std::index_sequence<Is...>>;
			friend Iter;
			using Parent = __maybe_const<Const, zip_view<Ranges...>>;
			static constexpr bool all_forward =
				(ForwardRange<__maybe_const<Const, Ranges>> && ...);
			static_assert(all_forward);
			static_assert(Same<std::index_sequence_for<Ranges...>,
				std::index_sequence<Is...>>);

			std::tuple<sentinel_t<__maybe_const<Const, Ranges>>...> last_;

		public:
			__sentinel() = default;
			explicit constexpr __sentinel(Parent& parent)
			: last_{__stl2::end(std::get<Is>(parent.ranges_))...}
			{}
		};
	} // namespace ext

	namespace view {
		struct __zip_fn {
			template<class... Ranges>
			requires requires(Ranges&&... ranges) {
				ext::zip_view{view::all(static_cast<Ranges>(ranges))...};
			}
			constexpr auto operator()(Ranges&&... ranges) const
			STL2_NOEXCEPT_RETURN(
				ext::zip_view{view::all(static_cast<Ranges>(ranges))...}
			)
		};

		inline constexpr __zip_fn zip{};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_ZIP_HPP
