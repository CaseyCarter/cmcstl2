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

#include <stl2/detail/gnu_tuple_hack.hpp>
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
			common_type<iter_difference_t<Ts>...>,
			meta::id<int>>>;

	// Writable wants const proxy references to be assignable, so we need a
	// tuple-like type with const-qualified assignment operators.
	// template<class... Ts>
	// struct __tuple_hack : std::tuple<Ts...> {
	// private:
	// 	template<class... Us>
	// 	requires sizeof...(Us) == sizeof...(Ts) &&
	// 		(Assignable<const Ts&, const Us&> && ...)
	// 	static constexpr bool assignable_hack = true;

	// 	template<class Other, std::size_t... Is>
	// 	requires Same<std::index_sequence_for<Ts...>, std::index_sequence<Is...>>
	// 	const __tuple_hack& assign_(Other&& other, std::index_sequence<Is...>) const {
	// 		((void)(std::get<Is>(*this) =
	// 			std::get<Is>(static_cast<Other&&>(other))), ...);
	// 		return *this;
	// 	}

	// 	template<class... Us>
	// 	requires sizeof...(Us) == sizeof...(Ts)
	// 	static constexpr bool can_construct = (std::is_constructible_v<Ts, Us> && ...);

	// public:
	// 	using std::tuple<Ts...>::tuple;
	// 	constexpr __tuple_hack(const std::tuple<Ts...>& t)
	// 	: std::tuple<Ts...>{t} {}
	// 	constexpr __tuple_hack(std::tuple<Ts...>&& t)
	// 	: std::tuple<Ts...>{static_cast<std::tuple<Ts...>&&>(t)} {}

	// 	template<class... Us>
	// 	requires sizeof...(Us) == sizeof...(Ts) &&
	// 		can_construct<Us&...> // gcc_bugs_bugs_bugs
	// 	constexpr __tuple_hack(std::tuple<Us...>& u)
	// 	: std::tuple<Ts...>{u} {}

	// 	__tuple_hack(const __tuple_hack&) = default;
	// 	__tuple_hack(__tuple_hack&&) = default;
	// 	__tuple_hack& operator=(const __tuple_hack&) = default;
	// 	__tuple_hack& operator=(__tuple_hack&&) = default;

	// 	using std::tuple<Ts...>::operator=;

	// 	constexpr const __tuple_hack& operator=(const std::tuple<Ts...>& t) const
	// 	requires (Assignable<const Ts&, const Ts&> && ...) {
	// 		using Indices = std::index_sequence_for<Ts...>;
	// 		return assign_(t, Indices{});
	// 	}
	// 	constexpr const __tuple_hack& operator=(std::tuple<Ts...>&& t) const
	// 	requires (Assignable<const Ts&, Ts> && ...) {
	// 		using Indices = std::index_sequence_for<Ts...>;
	// 		return assign_(static_cast<std::tuple<Ts...>&&>(t), Indices{});
	// 	}
	// 	template<class... Us>
	// 	requires assignable_hack<Us...>
	// 	constexpr const __tuple_hack& operator=(const std::tuple<Us...>& t) const {
	// 		using Indices = std::index_sequence_for<Ts...>;
	// 		return assign_(t, Indices{});
	// 	}
	// 	template<class... Us>
	// 	requires assignable_hack<Us...>
	// 	constexpr const __tuple_hack& operator=(std::tuple<Us...>&& t) const {
	// 		using Indices = std::index_sequence_for<Ts...>;
	// 		return assign_(static_cast<std::tuple<Us...>&&>(t), Indices{});
	// 	}
	// };

	// template<class... Ts>
	// __tuple_hack(Ts&&...) -> __tuple_hack<Ts&&...>;

	template<class... Ts>
	requires (requires { typename common_type_t<Ts, Ts&>; } && ...)
	struct common_type<tuple<Ts&...>, std::tuple<Ts...>> {
		using type = tuple<common_type_t<Ts&, Ts>...>;
	};
	template<class... Ts>
	requires (requires { typename common_type_t<Ts, Ts&>; } && ...)
	struct common_type<std::tuple<Ts...>, tuple<Ts&...>>	{
		using type = tuple<common_type_t<Ts&, Ts>...>;
	};

	template<class... Ts,
		template <class> class TQual, template <class> class UQual>
	requires (requires { typename common_reference_t<TQual<Ts>, UQual<Ts&>>; } && ...)
	struct basic_common_reference<tuple<Ts&...>, std::tuple<Ts...>, TQual, UQual> {
		using type = tuple<common_reference_t<TQual<Ts&>, UQual<Ts>>...>;
	};
	template<class... Ts,
		template <class> class TQual, template <class> class UQual>
	requires (requires { typename common_reference_t<TQual<Ts>, UQual<Ts&>>; } && ...)
	struct basic_common_reference<std::tuple<Ts...>, tuple<Ts&...>, TQual, UQual> {
		using type = tuple<common_reference_t<TQual<Ts>, UQual<Ts&>>...>;
	};

	// lvalue to rvalue reference
	template<class... Ts>
	requires (requires { typename common_type_t<Ts, Ts&&>; } && ...)
	struct common_type<tuple<Ts&&...>, std::tuple<Ts...>> {
		using type = tuple<common_type_t<Ts&&, Ts>...>;
	};
	template<class... Ts>
	requires (requires { typename common_type_t<Ts, Ts&&>; } && ...)
	struct common_type<std::tuple<Ts...>, tuple<Ts&&...>>	{
		using type = tuple<common_type_t<Ts&&, Ts>...>;
	};

	template<class... Ts,
		template <class> class TQual, template <class> class UQual>
	requires (requires { typename common_reference_t<TQual<Ts>, UQual<Ts&&>>; } && ...)
	struct basic_common_reference<tuple<Ts&&...>, std::tuple<Ts...>, TQual, UQual> {
		using type = tuple<common_reference_t<TQual<Ts&&>, UQual<Ts>>...>;
	};
	template<class... Ts,
		template <class> class TQual, template <class> class UQual>
	requires (requires { typename common_reference_t<TQual<Ts>, UQual<Ts&&>>; } && ...)
	struct basic_common_reference<std::tuple<Ts...>, tuple<Ts&&...>, TQual, UQual> {
		using type = tuple<common_reference_t<TQual<Ts>, UQual<Ts&&>>...>;
	};

	// lvalue reference to rvalue reference
	template<class... Ts>
	requires (requires { typename common_type_t<Ts&, remove_reference_t<Ts>&&>; } && ...)
	struct common_type<tuple<remove_reference_t<Ts>&&...>, std::tuple<Ts&...>> {
		using type = tuple<common_type_t<remove_reference_t<Ts>&&, Ts&>...>;
	};
	template<class... Ts>
	requires (requires { typename common_type_t<Ts&, remove_reference_t<Ts>&&>; } && ...)
	struct common_type<std::tuple<Ts&...>, tuple<remove_reference_t<Ts>&&...>>	{
		using type = tuple<common_type_t<remove_reference_t<Ts>&&, Ts&>...>;
	};

	template<class... Ts,
		template <class> class TQual, template <class> class UQual>
	requires (requires { typename common_reference_t<TQual<Ts&>, UQual<remove_reference_t<Ts>&&>>; } && ...)
	struct basic_common_reference<tuple<remove_reference_t<Ts>&&...>, std::tuple<Ts&...>, TQual, UQual> {
		using type = tuple<common_reference_t<TQual<remove_reference_t<Ts>&&>, UQual<Ts&>>...>;
	};
	template<class... Ts,
		template <class> class TQual, template <class> class UQual>
	requires (requires { typename common_reference_t<TQual<Ts&>, UQual<remove_reference_t<Ts>&&>>; } && ...)
	struct basic_common_reference<std::tuple<Ts&...>, tuple<remove_reference_t<Ts>&&...>, TQual, UQual> {
		using type = tuple<common_reference_t<TQual<Ts&>, UQual<remove_reference_t<Ts>&&>>...>;
	};

	namespace ext {
		template<View... Rs>
		struct __zip_view_base {
			std::tuple<iterator_t<Rs>...> current_ {};
		};
		template<View... Rs>
		requires (ForwardRange<Rs> && ...)
		struct __zip_view_base<Rs...> {};

		template<View... Rs>
		struct zip_view
		: view_interface<zip_view<Rs...>>
		, private __zip_view_base<Rs...>
		{
		private:
			template <bool Const, class Indices> struct __zipperator;
			template <bool Const, class Indices> struct __sentinel;
			using Indices = std::index_sequence_for<Rs...>;
			static constexpr bool all_simple = (ext::SimpleView<Rs> && ...);
			static constexpr bool all_forward = (ForwardRange<Rs> && ...);
			static constexpr bool all_forward_const = (ForwardRange<const Rs> && ...);
			static constexpr bool all_sized = (SizedRange<Rs> && ...);
			static constexpr bool all_sized_const = (SizedRange<const Rs> && ...);

			std::tuple<Rs...> ranges_ {};

			template<std::size_t... Is, class Self>
			static constexpr auto size_(std::index_sequence<Is...>, Self& self)
			{
				if constexpr (sizeof...(Rs) != 0) {
					static_assert(Same<std::index_sequence<Is...>, Indices>);
					using D = __common_difference_type_t<iterator_t<Rs>...>;
					return __stl2::min({static_cast<D>(
						__stl2::size(std::get<Is>(self.ranges_)))...});
				} else {
					return 0;
				}
			}
			template<std::size_t... Is>
			constexpr void begin_(std::index_sequence<Is...>)
			{ (((void)(std::get<Is>(this->current_) = __stl2::begin(std::get<Is>(ranges_)))), ...); }
		public:
			zip_view() = default;
			constexpr zip_view(Rs... rs)
			noexcept((std::is_nothrow_move_constructible_v<Rs> && ...))
			requires sizeof...(Rs) != 0
			: ranges_(std::move(rs)...) {}

			constexpr auto begin() {
				if constexpr (!all_forward) begin_(Indices{});
				return __zipperator<all_simple && all_forward, Indices>{*this};
			}
			constexpr auto begin() const requires all_forward_const
			{ return __zipperator<true, Indices>{*this}; }

			constexpr auto end() requires all_forward
			{
				if constexpr ((RandomAccessRange<Rs> && ...) && all_sized)
					return __zipperator<all_simple, Indices>{*this, size_(Indices{}, *this)};
				else
					return __sentinel<all_simple, Indices>{*this};
			}
			constexpr auto end() const
			{
				if constexpr ((RandomAccessRange<const Rs> && ...) && all_sized_const)
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

		template <class... Rs>
		zip_view(Rs&&...) -> zip_view<all_view<Rs>...>;

		template<bool, View... Rs>
		struct __zipperator_base {
			zip_view<Rs...>* parent_ = nullptr;
		};
		template<bool Const, View... Rs>
		requires (ForwardRange<__maybe_const<Const, Rs>> && ...)
		struct __zipperator_base<Const, Rs...> {
			std::tuple<iterator_t<__maybe_const<Const, Rs>>...> current_ {};
		};

		template<bool, View... Rs>
		struct __zipperator_vt_base {};
		template<bool Const, View... Rs>
		requires (InputRange<__maybe_const<Const, Rs>> && ...)
		struct __zipperator_vt_base<Const, Rs...> {
			using value_type = std::tuple<
				iter_value_t<iterator_t<__maybe_const<Const, Rs>>>...>;
		};

		template<View... Rs>
		template<bool Const, std::size_t... Is>
		struct zip_view<Rs...>::__zipperator<Const, std::index_sequence<Is...>>
		: private __zipperator_base<Const, Rs...>
		, __zipperator_vt_base<Const, Rs...>
		{
		private:
			using Parent = __maybe_const<Const, zip_view<Rs...>>;
			using Sent = typename zip_view<Rs...>::template
				__sentinel<Const, std::index_sequence<Is...>>;

			static constexpr bool all_random_access =
				(RandomAccessRange<__maybe_const<Const, Rs>> && ...);
			static constexpr bool all_bidi =
				(BidirectionalRange<__maybe_const<Const, Rs>> && ...);
			static constexpr bool all_forward =
				(ForwardRange<__maybe_const<Const, Rs>> && ...);
			static constexpr bool all_input =
				(InputRange<__maybe_const<Const, Rs>> && ...);

			static_assert(!Const || all_forward);
			static_assert(Same<std::index_sequence_for<Rs...>,
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
				iterator_t<__maybe_const<Const, Rs>>...>;

			__zipperator() = default;
			explicit constexpr __zipperator(Parent& parent)
			: __zipperator_base<Const, Rs...>{detail::addressof(parent)}
			{}
			constexpr __zipperator(Parent& parent)
			requires all_forward
			: __zipperator_base<Const, Rs...>{{
				(__stl2::begin(std::get<Is>(parent.ranges_)))...
			}} {}
			constexpr __zipperator(Parent& parent, [[maybe_unused]] difference_type n)
			requires all_random_access
			: __zipperator_base<Const, Rs...>{{
				(__stl2::begin(std::get<Is>(parent.ranges_)) +
					static_cast<iter_difference_t<iterator_t<__maybe_const<Const, Rs>>>>(n))...
			}} {}

			constexpr auto operator*()
			{ return tuple<iter_reference_t<iterator_t<__maybe_const<Const, Rs>>>...>{*std::get<Is>(current())...}; }
			constexpr auto operator*() const
			requires (detail::Dereferenceable<const iterator_t<__maybe_const<Const, Rs>>> && ...)
			{ return tuple<const iter_reference_t<iterator_t<__maybe_const<Const, Rs>>>...>{*std::get<Is>(current())...}; }

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
			requires (SizedSentinel<iterator_t<__maybe_const<Const, Rs>>,
				iterator_t<__maybe_const<Const, Rs>>> && ...)
			{
				if constexpr (sizeof...(Rs) != 0) {
					return __stl2::max({static_cast<difference_type>(
						std::get<Is>(x.current()) - std::get<Is>(y.current()))...});
				} else {
					return 0;
				}
			}

			constexpr auto operator[](difference_type n) const
			requires all_random_access
			{ return *(*this + n); }

			friend constexpr auto iter_move(const __zipperator& x)
			noexcept(noexcept(((void)__stl2::iter_move(
				std::declval<const iterator_t<__maybe_const<Const, Rs>>&>()), ...)))
			requires ((Readable<iterator_t<__maybe_const<Const, Rs>>> && ...))
			{ return tuple<decltype(__stl2::iter_move(std::get<Is>(x.current())))...>{
				__stl2::iter_move(std::get<Is>(x.current()))...}; }

			friend constexpr void iter_swap(const __zipperator& x, const __zipperator& y)
			noexcept(noexcept((__stl2::iter_swap(
				std::declval<const iterator_t<__maybe_const<Const, Rs>>&>(),
				std::declval<const iterator_t<__maybe_const<Const, Rs>>&>()), ...)))
			requires (IndirectlySwappable<iterator_t<__maybe_const<Const, Rs>>> && ...)
			{ (__stl2::iter_swap(std::get<Is>(x.current()), std::get<Is>(y.current())), ...); }
		};

		template<View... Rs>
		template<bool Const, std::size_t... Is>
		struct zip_view<Rs...>::__sentinel<Const, std::index_sequence<Is...>> {
		private:
			using Iter = typename zip_view<Rs...>::template
				__zipperator<Const, std::index_sequence<Is...>>;
			friend Iter;
			using Parent = __maybe_const<Const, zip_view<Rs...>>;
			static constexpr bool all_forward =
				(ForwardRange<__maybe_const<Const, Rs>> && ...);
			static_assert(all_forward);
			static_assert(Same<std::index_sequence_for<Rs...>,
				std::index_sequence<Is...>>);

			std::tuple<sentinel_t<__maybe_const<Const, Rs>>...> last_;

		public:
			__sentinel() = default;
			explicit constexpr __sentinel(Parent& parent)
			: last_{__stl2::end(std::get<Is>(parent.ranges_))...}
			{}
		};
	} // namespace ext

	namespace view {
		struct __zip_fn {
			template<class... Rs>
			requires requires(Rs&&... rs) {
				__stl2::ext::zip_view{view::all(static_cast<Rs>(rs))...};
			}
			constexpr auto operator()(Rs&&... rs) const
			STL2_NOEXCEPT_RETURN(
				__stl2::ext::zip_view{view::all(static_cast<Rs>(rs))...}
			)
		};

		inline constexpr __zip_fn zip{};
	} // namespace view
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_ZIP_HPP
