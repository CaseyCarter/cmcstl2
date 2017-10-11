// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_VARIANT_VISIT_HPP
#define STL2_DETAIL_VARIANT_VISIT_HPP

#include <utility>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/hash.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/variant/fwd.hpp>

///////////////////////////////////////////////////////////////////////////
// Visitation machinery for variant
//
STL2_OPEN_NAMESPACE {
	namespace __variant {
		using non_void_predicate =
			meta::compose<meta::quote<meta::not_>, meta::quote_trait<is_void>>;

		template <class Types>
		using non_void_types = meta::filter<Types, non_void_predicate>;

		// Convert a list of types into a list of the indices of the non-is_void types.
		template <class Types>
		using non_void_indices =
			meta::transform<
				meta::filter<
					meta::zip<meta::list<
						meta::as_list<meta::make_index_sequence<Types::size()>>,
						Types>>,
					meta::compose<non_void_predicate, meta::quote<meta::second>>>,
				meta::quote<meta::first>>;

		template <Variant... Variants>
		constexpr std::size_t total_alternatives = 1;
		template <Variant First, Variant... Rest>
		constexpr std::size_t total_alternatives<First, Rest...> =
			non_void_types<VariantTypes<First>>::size() *
			total_alternatives<Rest...>;

		///////////////////////////////////////////////////////////////////////////
		// Determine the return type and noexcept status of visitor F on Variants
		// with active alternatives Indices
		//
		template <class F, class Variants, class Indices>
		struct single_visit_properties {};
		template <class F, Variant... Variants, std::size_t... Is>
		struct single_visit_properties<F,
			meta::list<Variants...>, meta::list<meta::size_t<Is>...>>
		{
			using type =
				decltype(declval<F>()(std::index_sequence<Is...>{},
					v_access::raw_get(in_place_index<Is>, declval<Variants>())...));
			static constexpr bool nothrow =
				noexcept(declval<F>()(std::index_sequence<Is...>{},
					v_access::raw_get(in_place_index<Is>, declval<Variants>())...));
		};
		template <class F, class Variants, class Indices>
		using single_visit_return_t =
			meta::_t<single_visit_properties<F, Variants, Indices>>;
		template <class F, class Variants, class Indices>
		using single_visit_noexcept =
			meta::bool_<single_visit_properties<F, Variants, Indices>::nothrow>;

		template <Variant... Vs>
		using all_visit_vectors =
			meta::cartesian_product<meta::list<non_void_indices<VariantTypes<Vs>>...>>;

		///////////////////////////////////////////////////////////////////////////
		// Create a list of all possible return types from F visiting Vs
		//
		template <class F, Variant... Vs>
		using all_return_types =
			meta::transform<
				all_visit_vectors<Vs...>,
				meta::bind_front<meta::quote<single_visit_return_t>, F, meta::list<Vs...>>>;

		///////////////////////////////////////////////////////////////////////////
		// F is a valid visitor for Vs iff all possible return types meet a
		// particular criterion:
		//
		#if 0
		// Require visitation to return the same type for all alternatives.
		template <class F, Variant... Vs>
			requires meta::_v<meta::all_of<
				meta::pop_front<all_return_types<F, Vs...>>,
				meta::bind_front<
					meta::quote_trait<is_same>,
					meta::front<all_return_types<F, Vs...>>>>>
		using VisitReturn = meta::front<all_return_types<F, Vs...>>;

		#elif 0
		// Require the return type of all alternatives to have a common
		// type, which visit returns.
		template <class F, Variant... Vs>
		using VisitReturn =
			meta::apply<meta::quote<common_type_t>, all_return_types<F, Vs...>>;

		#else
		// Require the return type of all alternatives to have a common
		// reference type, which visit returns.
		template <class F, Variant... Vs>
		using VisitReturn =
			meta::apply<meta::quote<common_reference_t>, all_return_types<F, Vs...>>;
		#endif

		template <class F, class... Vs>
		concept bool RawVisitorWithIndices =
			sizeof...(Vs) > 0 && requires { typename VisitReturn<F, Vs...>; };

		RawVisitorWithIndices{F, ...Vs}
		constexpr bool VisitNothrow =
			meta::_v<meta::apply<meta::quote<meta::strict_and>,
				meta::transform<all_visit_vectors<Vs...>,
					meta::bind_front<meta::quote<single_visit_noexcept>,
						F, meta::list<Vs...>>>>>;

		// TODO: Tune.
		constexpr std::size_t O1_visit_threshold = 5;

		template <std::size_t... Is, Variant... Vs, RawVisitorWithIndices<Vs...> F>
		constexpr VisitReturn<F, Vs...>
		visit_handler(std::index_sequence<Is...> indices, F&& f, Vs&&... vs)
		STL2_NOEXCEPT_RETURN(
			std::forward<F>(f)(indices,
				v_access::raw_get(in_place_index<Is>, std::forward<Vs>(vs))...)
		)

		///////////////////////////////////////////////////////////////////////////
		// O(N) visitor implementation
		//
		RawVisitorWithIndices{F, ...Vs}
		class ON_dispatch {
			using R = VisitReturn<F, Vs...>;
			static constexpr std::size_t N = sizeof...(Vs);

			F&& f_;
			tuple<Vs&&...> vs_;

			template <std::size_t... Is, std::size_t... Js>
			requires sizeof...(Is) == N && sizeof...(Js) == N
			constexpr R invoke(std::index_sequence<Is...> i, std::index_sequence<Js...>)
			noexcept(noexcept(visit_handler(i, std::forward<F>(f_),
				get<Js>(std::move(vs_))...)))
			{
				return visit_handler(i, std::forward<F>(f_),
					get<Js>(std::move(vs_))...);
			}

			template <std::size_t... Is>
			requires sizeof...(Is) == N
			constexpr R find_indices(std::index_sequence<Is...> i)
			noexcept(noexcept(declval<ON_dispatch&>().
				invoke(i, std::make_index_sequence<N>{})))
			{
				return invoke(i, std::make_index_sequence<N>{});
			}

			template <std::size_t... Is, std::size_t Last>
			constexpr R find_one_index(std::index_sequence<Is...>, std::size_t n, std::index_sequence<Last>)
			noexcept(noexcept(declval<ON_dispatch&>().
				find_indices(std::index_sequence<Is..., Last>{})))
			{
				STL2_EXPECT(n == Last);
				return find_indices(std::index_sequence<Is..., Last>{});
			}

			template <std::size_t... Is, std::size_t First, std::size_t... Rest>
			constexpr R find_one_index(std::index_sequence<Is...> i, std::size_t n, std::index_sequence<First, Rest...>)
			noexcept(noexcept(
				declval<ON_dispatch&>().find_indices(std::index_sequence<Is..., First>{}),
				declval<ON_dispatch&>().find_one_index(i, n, std::index_sequence<Rest...>{})))
			{
				STL2_EXPECT(n >= First);
				if (n <= First) {
					return find_indices(std::index_sequence<Is..., First>{});
				} else {
					return find_one_index(i, n, std::index_sequence<Rest...>{});
				}
			}

			template <std::size_t... Is,
				class NVI = __as_integer_sequence<non_void_indices<
					VariantTypes<meta::at_c<meta::list<Vs...>, sizeof...(Is)>>>>>
			requires sizeof...(Is) < N
			constexpr R find_indices(std::index_sequence<Is...> i)
			noexcept(noexcept(declval<ON_dispatch&>()
				.find_one_index(i, std::size_t{0}, NVI{})))
			{
				auto& v = get<sizeof...(Is)>(vs_);
				STL2_EXPECT(v.valid());
				return find_one_index(i, v.index(), NVI{});
			}

		public:
			constexpr ON_dispatch(F&& f, Vs&&... vs) noexcept
			: f_(std::forward<F>(f)), vs_{std::forward<Vs>(vs)...} {}

			constexpr R visit() &&
			noexcept(VisitNothrow<F, Vs...>)
			{
				return find_indices(std::index_sequence<>{});
			}
		};

		RawVisitorWithIndices{F, ...Vs}
		constexpr VisitReturn<F, Vs...>
		raw_visit_with_indices(F&& f, Vs&&... vs)
		noexcept(VisitNothrow<F, Vs...>)
		requires total_alternatives<Vs...> < O1_visit_threshold
		{
			return ON_dispatch<F, Vs...>{
				std::forward<F>(f), std::forward<Vs>(vs)...
			}.visit();
		}

		///////////////////////////////////////////////////////////////////////////
		// O(1) visitor implementation
		//
		template <class I, class F, Variant... Vs>
		requires RawVisitorWithIndices<F, Vs...>
		constexpr VisitReturn<F, Vs...>
		o1_visit_handler(F&& f, Vs&&... vs)
		STL2_NOEXCEPT_RETURN(
			visit_handler(I{}, std::forward<F>(f),
				std::forward<Vs>(vs)...)
		)

		RawVisitorWithIndices{F, ...Vs}
		using visit_handler_ptr =
			VisitReturn<F, Vs...>(*)(F&&, Vs&&...);

		template <class Indices, class F, Variant... Vs>
		requires RawVisitorWithIndices<F, Vs...>
		constexpr visit_handler_ptr<F, Vs...> visit_handler_for = {};
		template <std::size_t... Is, class F, Variant... Vs>
		requires RawVisitorWithIndices<F, Vs...> &&
			meta::_v<meta::all_of<
				meta::list<meta::at_c<VariantTypes<Vs>, Is>...>,
				non_void_predicate>>
		constexpr visit_handler_ptr<F, Vs...>
			visit_handler_for<std::index_sequence<Is...>, F, Vs...> =
				&o1_visit_handler<std::index_sequence<Is...>, F, Vs...>;

		template <class Indices, class F, Variant... Vs>
		requires RawVisitorWithIndices<F, Vs...>
		struct O1_dispatch;
		template <class... Is, class F, Variant... Vs>
		requires RawVisitorWithIndices<F, Vs...>
		struct O1_dispatch<meta::list<Is...>, F, Vs...> {
			static constexpr visit_handler_ptr<F, Vs...> table[] = {
				visit_handler_for<Is, F, Vs...>...
			};
		};

		template <class... Is, class F, Variant... Vs>
		requires RawVisitorWithIndices<F, Vs...>
		constexpr visit_handler_ptr<F, Vs...>
			O1_dispatch<meta::list<Is...>, F, Vs...>::table[];

		constexpr std::size_t calc_index() noexcept {
			return 0;
		}

		template <Variant First, Variant... Rest>
		constexpr std::size_t
		calc_index(const First& f, const Rest&... rest) noexcept
		{
			STL2_EXPECT(f.valid());
			constexpr std::size_t M = meta::_v<meta::fold<
				meta::list<meta::size<VariantTypes<Rest>>...>,
				meta::size_t<1>,
				meta::quote<meta::multiplies>>>;
			return f.index() * M + calc_index(rest...);
		}

		template <Variant... Vs>
		using all_index_vectors =
			meta::transform<
				meta::cartesian_product<meta::list<
					meta::as_list<meta::make_index_sequence<
						VariantTypes<Vs>::size()>>...>>,
				meta::quote<__as_integer_sequence>>;

		RawVisitorWithIndices{F, ...Vs}
		constexpr VisitReturn<F, Vs...>
		raw_visit_with_indices(F&& f, Vs&&... vs)
		noexcept(VisitNothrow<F, Vs...>)
		requires total_alternatives<Vs...> >= O1_visit_threshold
		{
			using Dispatch = O1_dispatch<all_index_vectors<Vs...>, F, Vs...>;
			std::size_t i = calc_index(vs...);
			STL2_EXPECT(Dispatch::table[i]);
			return Dispatch::table[i](std::forward<F>(f), std::forward<Vs>(vs)...);
		}

		///////////////////////////////////////////////////////////////////////////
		// Adapt a visitor that accepts a single integral_constant index
		// to the variadic visitor model that accepts an integer_sequence of indices.
		//
		template <class F>
		struct single_index_visitor {
			F&& f_;

			template <std::size_t I, class T>
			constexpr decltype(auto) operator()(std::index_sequence<I>, T&& t) && {
				return std::forward<F>(f_)(meta::size_t<I>{}, std::forward<T>(t));
			}
		};

		template <class F, class V>
		concept bool RawVisitorWithIndex =
			RawVisitorWithIndices<single_index_visitor<F>, V>;

		RawVisitorWithIndex{F, V}
		constexpr VisitReturn<single_index_visitor<F>, V>
		raw_visit_with_index(F&& f, V&& v)
		noexcept(VisitNothrow<single_index_visitor<F>, V>)
		{
			return raw_visit_with_indices(
				single_index_visitor<F>{std::forward<F>(f)}, std::forward<V>(v));
		}

		///////////////////////////////////////////////////////////////////////////
		// Adapt a visitor that doesn't care about the indices of the alternatives
		// to the variadic visitor model.
		//
		template <class F, Variant... Vs>
		struct no_index_visitor {
			F&& f_;

			template <class I, class... Args>
			constexpr decltype(auto) operator()(I, Args&&... args) && {
				return std::forward<F>(f_)(std::forward<Args>(args)...);
			}
		};

		template <class F, class... Vs>
		concept bool RawVisitor =
			RawVisitorWithIndices<no_index_visitor<F>, Vs...>;

		RawVisitor{F, ...Vs}
		constexpr VisitReturn<no_index_visitor<F>, Vs...>
		raw_visit(F&& f, Vs&&... vs)
		noexcept(VisitNothrow<no_index_visitor<F>, Vs...>)
		{
			return raw_visit_with_indices(
				no_index_visitor<F>{std::forward<F>(f)}, std::forward<Vs>(vs)...);
		}

		///////////////////////////////////////////////////////////////////////////
		// Adaptor that "cooks" the alternative types - dereferences
		// reference_wrappers - before passing them to the visitor. Only these
		// "cooked" visit functions are exposed to users, the "raw" forms are
		// internal-only.
		//
		template <class F, Variant... Vs>
		struct cooked_visitor {
			F&& f_;

			template <std::size_t... Is, class... Args>
			constexpr decltype(auto) operator()(std::index_sequence<Is...> i, Args&&... args) &&
			noexcept(noexcept(std::forward<F>(f_)(i,
				cook<meta::at_c<VariantTypes<Vs>, Is>>(std::forward<Args>(args))...)))
			{
				return std::forward<F>(f_)(i,
					cook<meta::at_c<VariantTypes<Vs>, Is>>(
						std::forward<Args>(args))...);
			}
		};

		template <class F, class... Vs>
		concept bool VisitorWithIndices =
			RawVisitorWithIndices<cooked_visitor<F, Vs...>, Vs...>;

		VisitorWithIndices{F, ...Vs}
		constexpr VisitReturn<cooked_visitor<F, Vs...>, Vs...>
		visit_with_indices(F&& f, Vs&&...  vs)
		noexcept(VisitNothrow<cooked_visitor<F, Vs...>, Vs...>)
		{
			return raw_visit_with_indices(
				cooked_visitor<F, Vs...>{std::forward<F>(f)},
				std::forward<Vs>(vs)...);
		}

		template <class F, class V>
		concept bool VisitorWithIndex =
			VisitorWithIndices<single_index_visitor<F>, V>;

		VisitorWithIndex{F, V}
		constexpr VisitReturn<cooked_visitor<single_index_visitor<F>, V>, V>
		visit_with_index(F&& f, V&& v)
		noexcept(VisitNothrow<cooked_visitor<single_index_visitor<F>, V>, V>)
		{
			return visit_with_indices(
				single_index_visitor<F>{std::forward<F>(f)},
				std::forward<V>(v));
		}

		template <class F, class... Vs>
		concept bool Visitor =
			VisitorWithIndices<no_index_visitor<F>, Vs...>;

		Visitor{F, ...Vs}
		constexpr VisitReturn<cooked_visitor<no_index_visitor<F>, Vs...>, Vs...>
		visit(F&& f, Vs&&... vs)
		noexcept(VisitNothrow<cooked_visitor<no_index_visitor<F>, Vs...>, Vs...>)
		{
			return visit_with_indices(
				no_index_visitor<F>{std::forward<F>(f)},
				std::forward<Vs>(vs)...);
		}

		template <class>
		struct hashable_element : false_type {};
		template <>
		struct hashable_element<void> : true_type {};
		template <_IsNot<is_reference> T>
		requires ext::Hashable<T>
		struct hashable_element<T> : true_type {};
		template <class T>
		using hashable_element_t = meta::_t<hashable_element<T>>;

		template <class... Ts>
		constexpr bool HashableVariant =
			meta::all_of<meta::list<Ts...>, meta::quote<hashable_element_t>>::value;
	}

	using __variant::visit;
	using __variant::visit_with_index;
	using __variant::visit_with_indices;
} STL2_CLOSE_NAMESPACE

///////////////////////////////////////////////////////////////////////////
// std::hash specialization for variants
//
namespace std {
	template <class... Ts>
	requires ::__stl2::__variant::HashableVariant<Ts...>
	struct hash<::__stl2::variant<Ts...>> {
	private:
		struct hash_visitor {
			constexpr std::size_t operator()(auto i, const auto& e) {
				std::size_t seed = 0u;
				::__stl2::ext::hash_combine(seed, i());
				::__stl2::ext::hash_combine(seed, e);
				return seed;
			}
		};

	public:
		using result_type = size_t;
		using argument_type = ::__stl2::variant<Ts...>;

		constexpr size_t operator()(const argument_type& v) const {
			return ::__stl2::__variant::visit_with_index(hash_visitor{}, v);
		}
	};
} // namespace std

#endif
