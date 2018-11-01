#ifndef STL2_DETAIL_COMMON_TUPLE_HPP
#define STL2_DETAIL_COMMON_TUPLE_HPP

#include <stl2/detail/fwd.hpp>
#include <stl2/meta/meta.hpp>

#include <stl2/type_traits.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/functional/invoke.hpp>
#include <stl2/detail/tuple_like.hpp>
#include <stl2/detail/view/view_closure.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		template<typename... Us, typename Tup, std::size_t ...Is>
		tuple<Us...> to_std_tuple(Tup&& tup, index_sequence<Is...>)
		{ return tuple<Us...>{adl_get<Is>(static_cast<Tup&&>(tup))...}; }
	} // namespace detail
	template<typename... Ts>
	struct common_tuple : tuple<Ts...> {
	private:
		template<typename That, std::size_t... Is>
		common_tuple(That&& that, index_sequence<Is...>)
			: tuple<Ts...>{detail::adl_get<Is>(static_cast<That&&>(that))...}
		{}

		struct element_assign_ {
			template<typename T, typename U>
			int operator()(T& t, U&& u) const
			{
				t = static_cast<U &&>(u);
				return 0;
			}
		};
	public:
		// Construction
		common_tuple() noexcept((is_nothrow_default_constructible_v<Ts> && ...))
		requires (DefaultConstructible<Ts> && ...)
			: tuple<Ts...>{}
		{}
		template<typename... Us>
		explicit common_tuple(Us&&... us) noexcept((is_nothrow_constructible_v<Ts, Us> && ...))
			: tuple<Ts...>{static_cast<Us&&>(us)...}
		{ static_assert((Constructible<Ts, Us> && ...)); } // workaround for GCC #85241

		template<typename... Us>
		common_tuple(tuple<Us...>& that) noexcept((is_nothrow_constructible_v<Ts, Us&> && ...))
			: common_tuple(that, index_sequence_for<Ts...>{})
		{ static_assert((Constructible<Ts, Us&> && ...)); } // workaround for GCC #85241

		template<typename... Us>
		common_tuple(tuple<Us...> const& that) noexcept((is_nothrow_constructible_v<Ts, Us const&> && ...))
			: common_tuple(that, index_sequence_for<Ts...>{})
		{ static_assert((Constructible<Ts, Us const&> && ...)); } // workaround for GCC #85241

		template<typename... Us>
		common_tuple(tuple<Us...>&& that) noexcept((is_nothrow_constructible_v<Ts, Us> && ...))
			: common_tuple(std::move(that), index_sequence_for<Ts...>{})
		{ static_assert((Constructible<Ts, Us> && ...)); } // workaround for GCC #85241

		template<typename... Us>
		common_tuple(common_tuple<Us...>& that) noexcept((is_nothrow_constructible_v<Ts, Us&> && ...))
			: common_tuple(that, index_sequence_for<Ts...>{})
		{ static_assert((Constructible<Ts, Us&> && ...)); } // workaround for GCC #85241

		template<typename... Us>
		common_tuple(common_tuple<Us...> const& that) noexcept((is_nothrow_constructible_v<Ts, Us const&> && ...))
			: common_tuple(that, index_sequence_for<Ts...>{})
		{ static_assert((Constructible<Ts, Us const&> && ...)); } // workaround for GCC #85241

		template<typename... Us>
		common_tuple(common_tuple<Us...>&& that) noexcept((is_nothrow_constructible_v<Ts, Us> && ...))
			: common_tuple(std::move(that), index_sequence_for<Ts...>{})
		{ static_assert((Constructible<Ts, Us> && ...)); } // workaround for GCC #85241

		tuple<Ts...>& base() noexcept { return *this; }
		tuple<Ts...> const& base() const noexcept { return *this; }

		// Assignment
		template<typename... Us>
		common_tuple& operator=(tuple<Us...>& that) noexcept((is_nothrow_assignable_v<Ts&, Us&> && ...))
		{
			static_assert((Assignable<Ts&, Us&> && ...)); // workaround for GCC #85241
			(void)detail::tuple_transform(base(), that, element_assign_{});
			return *this;
		}

		template<typename... Us>
		common_tuple& operator=(tuple<Us...> const& that) noexcept((is_nothrow_assignable_v<Ts&, Us const&> && ...))
		{
			static_assert((Assignable<Ts&, Us const&> && ...)); // workaround for GCC #85241
			(void)detail::tuple_transform(base(), that, element_assign_{});
			return *this;
		}
		template<typename... Us>
		common_tuple& operator=(tuple<Us...>&& that) noexcept((is_nothrow_assignable_v<Ts&, Us> && ...))
		{
			static_assert((Assignable<Ts&, Us> && ...)); // workaround for GCC #85241
			(void)detail::tuple_transform(base(), std::move(that), element_assign_{});
			return *this;
		}

		template<typename... Us>
		common_tuple const& operator=(tuple<Us...>& that) const noexcept((is_nothrow_assignable_v<Ts const&, Us&> && ...))
		{
			static_assert((Assignable<Ts const&, Us&> && ...)); // workaround for GCC #85241
			(void)detail::tuple_transform(base(), that, element_assign_{});
			return *this;
		}
		template<typename... Us>
		common_tuple const& operator=(tuple<Us...> const& that) const noexcept((is_nothrow_assignable_v<Ts const&, Us const&> && ...))
		{
			static_assert((Assignable<Ts const&, Us const&> && ...)); // workaround for GCC #85241
			(void)detail::tuple_transform(base(), that, element_assign_{});
			return *this;
		}
		template<typename... Us>
		common_tuple const& operator=(tuple<Us...>&& that) const noexcept((is_nothrow_assignable_v<Ts const&, Us&&> && ...))
		{
			static_assert((Assignable<Ts const&, Us&&> && ...)); // workaround for GCC #85241
			(void)detail::tuple_transform(base(), std::move(that), element_assign_{});
			return *this;
		}

		// Conversion
		template<typename... Us>
		operator tuple<Us...>() & noexcept((is_nothrow_constructible_v<Us, Ts&> && ...))
		{
			static_assert((Constructible<Us, Ts&> && ...)); // workaround for GCC #85241
			return detail::to_std_tuple<Us...>(*this, index_sequence_for<Ts...>{});
		}
		template<typename... Us>
		operator tuple<Us...>() const& noexcept((is_nothrow_constructible_v<Us, Ts const&> && ...))
		{
			static_assert((Constructible<Us, Ts const&> && ...)); // workaround for GCC #85241
			return detail::to_std_tuple<Us...>(*this, index_sequence_for<Ts...>{});
		}
		template<typename... Us>
		operator tuple<Us...>() && noexcept((is_nothrow_constructible_v<Us, Ts> && ...))
		{
			static_assert((Constructible<Us, Ts> && ...)); // workaround for GCC #85241
			return detail::to_std_tuple<Us...>(std::move(*this), index_sequence_for<Ts...>{});
		}
	};

	template<std::size_t I, typename... Ts>
	requires (I < sizeof...(Ts))
	constexpr decltype(auto) get(common_tuple<Ts...>& ct)
	STL2_NOEXCEPT_RETURN
	(
		std::get<I>(ct.base())
	)
	template<std::size_t I, typename... Ts>
	requires (I < sizeof...(Ts))
	constexpr decltype(auto) get(common_tuple<Ts...> const& ct)
	STL2_NOEXCEPT_RETURN
	(
		std::get<I>(ct.base())
	)
	template<std::size_t I, typename... Ts>
	requires (I < sizeof...(Ts))
	constexpr decltype(auto) get(common_tuple<Ts...>&& ct)
	STL2_NOEXCEPT_RETURN
	(
		std::get<I>(std::move(ct.base()))
	)
	template<std::size_t I, typename... Ts>
	requires (I < sizeof...(Ts))
	void get(common_tuple<Ts...> const&& ct) = delete;

	template<typename T, typename... Ts>
	constexpr decltype(auto) get(common_tuple<Ts...>& ct)
	STL2_NOEXCEPT_RETURN
	(
		std::get<T>(ct.base())
	)
	template<typename T, typename... Ts>
	constexpr decltype(auto) get(common_tuple<Ts...> const& ct)
	STL2_NOEXCEPT_RETURN
	(
		std::get<T>(ct.base())
	)
	template<typename T, typename... Ts>
	constexpr decltype(auto) get(common_tuple<Ts...>&& ct)
	STL2_NOEXCEPT_RETURN
	(
		std::get<T>(std::move(ct.base()))
	)
	template<typename T, typename... Ts>
	void get(common_tuple<Ts...> const&& ct) = delete;

	// Logical operators
#define LOGICAL_OP(OP, CONCEPT)\
	template<typename...Ts, typename... Us>\
	requires (CONCEPT<Ts, Us> && ...)\
	bool operator OP(common_tuple<Ts...> const& a, common_tuple<Us...> const& b)\
	{\
		return a.base() OP b.base();\
	}\
	template<typename...Ts, typename... Us>\
	requires (CONCEPT<Ts, Us> && ...)\
	bool operator OP(tuple<Ts...> const& a, common_tuple<Us...> const& b)\
	{\
		return a OP b.base();\
	}\
	template<typename...Ts, typename... Us>\
	requires (CONCEPT<Ts, Us> && ...)\
	bool operator OP(common_tuple<Ts...> const& a, tuple<Us...> const& b)\
	{\
		return a.base() OP b;\
	}\
	/**/
	LOGICAL_OP(==, EqualityComparable)
	LOGICAL_OP(!=, EqualityComparable)
	LOGICAL_OP(<, StrictTotallyOrdered)
	LOGICAL_OP(<=, StrictTotallyOrdered)
	LOGICAL_OP(>, StrictTotallyOrdered)
	LOGICAL_OP(>=, StrictTotallyOrdered)
#undef LOGICAL_OP

	template<typename T>
	struct bind_element : meta::if_<is_same<decay_t<T>, T>, meta::id<T>, bind_element<decay_t<T>>>
	{};

	template <class T>
	using bind_element_t = meta::_t<bind_element<T>>;

	struct unwrap_reference_fn : detail::__pipeable<unwrap_reference_fn> {
		template<typename T>
		requires (!__invoke::is_reference_wrapper<T>)
		T&& operator()(T&& t) const noexcept
		{
			return static_cast<T&&>(t);
		}

		template<typename T>
		typename reference_wrapper<T>::reference
		operator()(reference_wrapper<T> t) const noexcept
		{
			return t.get();
		}

		template<typename T>
		T& operator()(std::reference_wrapper<T> t) const noexcept
		{
			return t.get();
		}
	};

	inline constexpr unwrap_reference_fn unwrap_reference {};

	template<typename T>
	using unwrap_reference_t = decltype(unwrap_reference(std::declval<T>()));

	struct __make_common_tuple_fn {
		template<typename ...Args>
		common_tuple<bind_element_t<Args>...> operator()(Args&&... args) const
		noexcept((is_nothrow_constructible_v<bind_element_t<Args>, unwrap_reference_t<Args>> && ...))
		{
			return common_tuple<bind_element_t<Args>...>{
				unwrap_reference(static_cast<Args&&>(args))...};
		}
	};

	inline constexpr __make_common_tuple_fn make_common_tuple {};

	template<typename F, typename S>
	struct common_pair : pair<F, S> {
	private:
		pair<F, S> const& base() const noexcept
		{ return *this; }
	public:
		// Construction
		common_pair()
		noexcept(is_nothrow_default_constructible_v<F> && is_nothrow_default_constructible_v<S>)
		requires DefaultConstructible<F> && DefaultConstructible<S>
			: pair<F, S>{}
		{}
		template<typename F2, typename S2>
		requires (Constructible<F, F2> && Constructible<S, S2>)
		common_pair(F2&& f2, S2&& s2)
		noexcept(is_nothrow_constructible_v<F, F2> && is_nothrow_constructible_v<S, S2>)
			: pair<F, S>{static_cast<F2&&>(f2), static_cast<S2&&>(s2)}
		{}
		template<typename F2, typename S2>
		requires (Constructible<F, F2&> && Constructible<S, S2&>)
		common_pair(pair<F2, S2>& that)
		noexcept(is_nothrow_constructible_v<F, F2&> && is_nothrow_constructible_v<S, S2&>)
			: pair<F, S>{that.first, that.second}
		{}
		template<typename F2, typename S2>
		requires (Constructible<F, F2 const&> && Constructible<S, S2 const&>)
		common_pair(pair<F2, S2> const& that)
		noexcept(is_nothrow_constructible_v<F, F2 const&> && is_nothrow_constructible_v<S, S2 const&>)
			: pair<F, S>{that.first, that.second}
		{}
		template<typename F2, typename S2>
		requires (Constructible<F, F2> && Constructible<S, S2>)
		common_pair(pair<F2, S2>&& that)
		noexcept(is_nothrow_constructible_v<F, F2> && is_nothrow_constructible_v<S, S2>)
			: pair<F, S>{std::forward<F2>(that.first), std::forward<S2>(that.second)}
		{}

		// Conversion
		template<typename F2, typename S2>
		requires (Constructible<F2, F&> && Constructible<S2, S&>)
		operator pair<F2, S2>() &
		noexcept(is_nothrow_constructible_v<F2, F&> && is_nothrow_constructible_v<S2, S&>)
		{ return {this->first, this->second}; }

		template<typename F2, typename S2>
		requires (Constructible<F2, F const&> && Constructible<S2, S const&>)
		operator pair<F2, S2>() const&
		noexcept(is_nothrow_constructible_v<F2, F const&> && is_nothrow_constructible_v<S2, S const&>)
		{ return {this->first, this->second}; }

		template<typename F2, typename S2>
		requires (Constructible<F2, F> && Constructible<S2, S>)
		operator pair<F2, S2>() &&
		noexcept(is_nothrow_constructible_v<F2, F> && is_nothrow_constructible_v<S2, S>)
		{ return {std::forward<F>(this->first), std::forward<S>(this->second)}; }

		// Assignment
		template<typename F2, typename S2>
		requires (Assignable<F&, F2&> && Assignable<S&, S2&>)
		common_pair& operator=(pair<F2, S2>& that)
		noexcept(is_nothrow_assignable_v<F&, F2&> && is_nothrow_assignable_v<S&, S2&>)
		{
			this->first = that.first;
			this->second = that.second;
			return *this;
		}

		template<typename F2, typename S2>
		requires (Assignable<F&, F2 const&> && Assignable<S&, S2 const&>)
		common_pair& operator=(pair<F2, S2> const& that)
		noexcept(is_nothrow_assignable_v<F&, F2 const&> && is_nothrow_assignable_v<S&, S2 const&>)
		{
			this->first = that.first;
			this->second = that.second;
			return *this;
		}

		template<typename F2, typename S2>
		requires (Assignable<F&, F2> && Assignable<S&, S2>)
		common_pair& operator=(pair<F2, S2> &&that)
		noexcept(is_nothrow_assignable_v<F&, F2> && is_nothrow_assignable_v<S&, S2>)
		{
			this->first = static_cast<F2&&>(that.first);
			this->second = static_cast<S2&&>(that.second);
			return *this;
		}

		template<typename F2, typename S2>
		requires (Assignable<F const&, F2&> && Assignable<S const&, S2&>)
		common_pair const& operator=(pair<F2, S2> &that) const
		noexcept(is_nothrow_assignable_v<F const&, F2&> && is_nothrow_assignable_v<S const&, S2&>)
		{
			this->first = that.first;
			this->second = that.second;
			return *this;
		}

		template<typename F2, typename S2>
		requires (Assignable<F const&, F2 const&> && Assignable<S const&, S2 const&>)
		common_pair const& operator=(pair<F2, S2> const&  that) const
		noexcept(is_nothrow_assignable_v<F const&, F2 const&> && is_nothrow_assignable_v<S const&, S2 const&>)
		{
			this->first = that.first;
			this->second = that.second;
			return *this;
		}

		template<typename F2, typename S2>
		requires (Assignable<F const&, F2&&> && Assignable<S const&, S2&&>)
		common_pair const& operator=(pair<F2, S2> &&that) const
		noexcept(is_nothrow_assignable_v<F const&, F2&&> && is_nothrow_assignable_v<S const&, S2&&>)
		{
			this->first = static_cast<F2&&>(that.first);
			this->second = static_cast<S2&&>(that.second);
			return *this;
		}
	};

	// Logical operators
	template<typename F1, typename S1, typename F2, typename S2>
	requires (EqualityComparableWith<F1, F2> && EqualityComparableWith<S1, S2>)
	bool operator==(common_pair<F1, S1> const& a, common_pair<F2, S2> const& b)
	{ return a.first == b.first && a.second == b.second; }

	template<typename F1, typename S1, typename F2, typename S2>
	requires (EqualityComparableWith<F1, F2> && EqualityComparableWith<S1, S2>)
	bool operator==(common_pair<F1, S1> const& a, pair<F2, S2> const& b)
	{ return a.first == b.first && a.second == b.second; }

	template<typename F1, typename S1, typename F2, typename S2>
	requires (EqualityComparableWith<F1, F2> && EqualityComparableWith<S1, S2>)
	bool operator==(pair<F1, S1> const& a, common_pair<F2, S2> const& b)
	{ return a.first == b.first && a.second == b.second; }

	template<typename F1, typename S1, typename F2, typename S2>
	requires (StrictTotallyOrderedWith<F1, F2> && StrictTotallyOrderedWith<S1, S2>)
	bool operator<(common_pair<F1, S1> const& a, common_pair<F2, S2> const& b)
	{ return a.first < b.first || (!(b.first < a.first) && a.second < b.second); }

	template<typename F1, typename S1, typename F2, typename S2>
	requires (StrictTotallyOrderedWith<F1, F2> && StrictTotallyOrderedWith<S1, S2>)
	bool operator <(pair<F1, S1> const& a, common_pair<F2, S2> const& b)
	{ return a.first < b.first || (!(b.first < a.first) && a.second < b.second); }

	template<typename F1, typename S1, typename F2, typename S2>
	requires (StrictTotallyOrderedWith<F1, F2> && StrictTotallyOrderedWith<S1, S2>)
	bool operator <(common_pair<F1, S1> const& a, pair<F2, S2> const& b)
	{ return a.first < b.first || (!(b.first < a.first) && a.second < b.second); }

#define LOGICAL_OP(OP, CONCEPT, RET)\
	template<typename F1, typename S1, typename F2, typename S2>\
	requires (CONCEPT<F1, F2> && CONCEPT<S1, S2>)\
	bool operator OP(common_pair<F1, S1> const& a, common_pair<F2, S2> const& b)\
	{\
		return RET;\
	}\
	template<typename F1, typename S1, typename F2, typename S2>\
	requires (CONCEPT<F1, F2> && CONCEPT<S1, S2>)\
	bool operator OP(pair<F1, S1> const& a, common_pair<F2, S2> const& b)\
	{\
		return RET;\
	}\
	template<typename F1, typename S1, typename F2, typename S2>\
	requires (CONCEPT<F1, F2> && CONCEPT<S1, S2>)\
	bool operator OP(common_pair<F1, S1> const& a, pair<F2, S2> const& b)\
	{\
		return RET;\
	}\
	/**/
	LOGICAL_OP(!=, EqualityComparableWith, !(a == b))
	LOGICAL_OP(<=, StrictTotallyOrderedWith, !(b < a))
	LOGICAL_OP(>, StrictTotallyOrderedWith, (b < a))
	LOGICAL_OP(>=, StrictTotallyOrderedWith, !(a < b))
#undef LOGICAL_OP

	// struct __make_common_pair_fn {
	// 	template<typename First, typename Second,
	// 			typename F = bind_element_t<First>,
	// 			typename S = bind_element_t<Second>>
	// 	common_pair<F, S> operator()(First&& f, Second&& s) const
	// 	noexcept(is_nothrow_constructible_v<F, unwrap_reference_t<First>> &&
	// 				is_nothrow_constructible_v<F, unwrap_reference_t<Second>>)
	// 	{
	// 			return {
	// 				unwrap_reference(static_cast<First&&>(f)),
	// 				unwrap_reference(static_cast<Second&&>(s))};
	// 	}
	// };

	// inline constexpr __make_common_pair_fn make_common_pair {};

	namespace detail {
		template<typename, typename, typename, typename = void>
		struct common_type_tuple_like
		{};

		template<template<typename...> class T0, typename... Ts,
					template<typename...> class T1, typename ... Us,
					typename TupleLike>
		struct common_type_tuple_like<T0<Ts...>, T1<Us...>, TupleLike,
				meta::if_c<sizeof...(Ts) == sizeof...(Us)>>
			: meta::lazy::let<
					meta::lazy::invoke<TupleLike, meta::lazy::_t<common_type<Ts, Us>>...>>
		{};

		template<typename T, typename U>
		using make_common_pair =
				meta::if_<
					meta::or_<std::is_reference<T>, std::is_reference<U>>,
					common_pair<T, U>,
					pair<T, U>>;

		template<typename...Ts>
		using make_common_tuple =
				meta::if_<
					meta::any_of<meta::list<Ts...>, meta::quote<std::is_reference>>,
					common_tuple<Ts...>,
					tuple<Ts...>>;
	} // namespace detail

	// common_type for pairs
	template<typename F1, typename S1, typename F2, typename S2>
	struct common_type<pair<F1, S1>, common_pair<F2, S2>>
		: detail::common_type_tuple_like<pair<F1, S1>, common_pair<F2, S2>,
				meta::quote<detail::make_common_pair>>
	{};

	template<typename F1, typename S1, typename F2, typename S2>
	struct common_type<common_pair<F1, S1>, pair<F2, S2>>
		: detail::common_type_tuple_like<common_pair<F1, S1>, pair<F2, S2>,
				meta::quote<detail::make_common_pair>>
	{};

	template<typename F1, typename S1, typename F2, typename S2>
	struct common_type<common_pair<F1, S1>, common_pair<F2, S2>>
		: detail::common_type_tuple_like<common_pair<F1, S1>, common_pair<F2, S2>,
				meta::quote<common_pair>>
	{};

	// common_type for tuples
	template<typename... Ts, typename ... Us>
	struct common_type<common_tuple<Ts...>, tuple<Us...>>
		: detail::common_type_tuple_like<common_tuple<Ts...>, tuple<Us...>,
				meta::quote<detail::make_common_tuple>>
	{};

	template<typename... Ts, typename ... Us>
	struct common_type<tuple<Ts...>, common_tuple<Us...>>
		: detail::common_type_tuple_like<tuple<Ts...>, common_tuple<Us...>,
				meta::quote<detail::make_common_tuple>>
	{};

	template<typename... Ts, typename ... Us>
	struct common_type<common_tuple<Ts...>, common_tuple<Us...>>
		: detail::common_type_tuple_like<common_tuple<Ts...>, common_tuple<Us...>,
				meta::quote<common_tuple>>
	{};

	namespace detail {
		template<typename, typename, typename, typename = void>
		struct common_ref_tuple_like
		{};

		template<template<typename...> class T0, typename... Ts,
					template<typename...> class T1, typename ... Us, typename TupleLike>
		struct common_ref_tuple_like<T0<Ts...>, T1<Us...>, TupleLike,
				meta::if_c<sizeof...(Ts) == sizeof...(Us)>>
			: meta::lazy::let<
					meta::lazy::invoke<TupleLike, meta::lazy::_t<common_reference<Ts, Us>>...>>
		{};
	}

	// common reference for pairs
	template<typename F1, typename S1, typename F2, typename S2, template <class> class Qual1, template <class> class Qual2>
	struct basic_common_reference<common_pair<F1, S1>, pair<F2, S2>, Qual1, Qual2>
		: detail::common_ref_tuple_like<
				common_pair<Qual1<F1>, Qual1<S1>>,
				pair<Qual2<F2>, Qual2<S2>>,
				meta::quote<detail::make_common_pair>>
	{};

	template<typename F1, typename S1, typename F2, typename S2, template <class> class Qual1, template <class> class Qual2>
	struct basic_common_reference<pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>
		: detail::common_ref_tuple_like<
				pair<Qual1<F1>, Qual1<S1>>,
				common_pair<Qual2<F2>, Qual2<S2>>,
				meta::quote<detail::make_common_pair>>
	{};

	template<typename F1, typename S1, typename F2, typename S2, template <class> class Qual1, template <class> class Qual2>
	struct basic_common_reference<common_pair<F1, S1>, common_pair<F2, S2>, Qual1, Qual2>
		: detail::common_ref_tuple_like<
				common_pair<Qual1<F1>, Qual1<S1>>,
				common_pair<Qual2<F2>, Qual2<S2>>,
				meta::quote<common_pair>>
	{};

	// // common reference for tuples
	template<typename... Ts, typename... Us, template <class> class Qual1, template <class> class Qual2>
	struct basic_common_reference<common_tuple<Ts...>, tuple<Us...>, Qual1, Qual2>
		: detail::common_ref_tuple_like<
				common_tuple<Qual1<Ts>...>,
				tuple<Qual2<Us>...>,
				meta::quote<detail::make_common_tuple>>
	{};

	template<typename... Ts, typename ... Us, template <class> class Qual1, template <class> class Qual2>
	struct basic_common_reference<tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>
		: detail::common_ref_tuple_like<
				tuple<Qual1<Ts>...>,
				common_tuple<Qual2<Us>...>,
				meta::quote<detail::make_common_tuple>>
	{};

	template<typename... Ts, typename ... Us, template <class> class Qual1, template <class> class Qual2>
	struct basic_common_reference<common_tuple<Ts...>, common_tuple<Us...>, Qual1, Qual2>
		: detail::common_ref_tuple_like<
				common_tuple<Qual1<Ts>...>,
				common_tuple<Qual2<Us>...>,
				meta::quote<common_tuple>>
	{};
} STL2_CLOSE_NAMESPACE

namespace std {
	template<typename First, typename Second>
	struct tuple_size<__stl2::common_pair<First, Second>>
	: std::integral_constant<size_t, 2>
	{};

	template<typename First, typename Second>
	struct tuple_element<0, __stl2::common_pair<First, Second>> {
		using type = First;
	};

	template<typename First, typename Second>
	struct tuple_element<1, __stl2::common_pair<First, Second>> {
		using type = Second;
	};

	template<typename... Ts>
	struct tuple_size< __stl2::common_tuple<Ts...>>
	: std::integral_constant<size_t, sizeof...(Ts)>
	{};

	template<size_t N, typename... Ts>
	struct tuple_element<N, __stl2::common_tuple<Ts...>>
	: tuple_element<N, tuple<Ts...>>
	{};
} // namespace std

#endif // STL2_DETAIL_COMMON_TUPLE_HPP