// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter
//  Copyright Christopher Di Bella
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_NUMERIC_TRAITS_IDENTITY_HPP
#define STL2_DETAIL_NUMERIC_TRAITS_IDENTITY_HPP

#include <stl2/detail/concepts/numeric/magma.hpp>
#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		/// \brief
		/// \note Users may specialise this type.
		/// \note left_identity<BOp, T, U> may store a reference to T, so ensure that the lifetime of
		///       the identity object is exceeded by the lifetime of object of type T.
		/// \ref [numeric.identity.left]
		///
		template<class BOp, class T, class U = T>
		requires Magma<BOp, T, U>
		struct left_identity {};

		template<class BOp, class T, class U>
		struct left_identity<BOp&, T, U> : left_identity<__uncvref<BOp>, T, U> {};

		template<class BOp, class T, class U = T>
		using left_identity_t = decltype(std::declval<left_identity<BOp, T, U>>().value());

		/// \brief
		/// \note Users may specialise this type.
		/// \note right_identity<BOp, T, U> may store a reference to T, so ensure that the lifetime of
		///       the identity object is exceeded by the lifetime of object of type U.
		/// \ref [numeric.identity.right]
		///
		template<class BOp, class T, class U = T>
		requires Magma<BOp, T, U>
		struct right_identity {};

		template<class BOp, class T, class U>
		struct right_identity<BOp&, T, U> : right_identity<__uncvref<BOp>, T, U> {};

		template<class BOp, class T, class U = T>
		using right_identity_t = decltype(std::declval<right_identity<BOp, T, U>>().value());

		template<class BOp, class T, class U = T>
		META_CONCEPT __has_two_sided_identity = requires(BOp bop, const T& t, const U& u) {
			typename left_identity_t<BOp, T, U>;
			typename right_identity_t<BOp, T, U>;
			typename left_identity_t<BOp, U, T>;
			typename right_identity_t<BOp, U, T>;

			requires Same<left_identity_t<BOp, T, U>, left_identity_t<BOp, U, T>>;
			requires Same<right_identity_t<BOp, T, U>, right_identity_t<BOp, U, T>>;
			requires Same<left_identity_t<BOp, T, U>, right_identity_t<BOp, T, U>>;

			// axiom {
			//    auto left = left_identity<T, U, BOp>{};
			//    auto right = right_identity<T, U, BOp>{};
			//    [[assert: left(bop, t, u) == left(bop, u, t)]];
			//    [[assert: right(bop, t, u) == right(bop, u, t)]];
			//    [[assert: left(bop, t, u) == right(bop, u, t)]];
			//
			//    [[assert axiom: t != left(bop, t, u)]];
			//    [[assert axiom: u != right(bop, t, u)]];
			//    [[assert: t == invoke(bop, t, right(bop, t, u))]];
			//    [[assert: u == invoke(bop, left(bop, t, u), u))]];
			// };
		};

		/// \brief
		/// \note Users may not specialise this type.
		/// \note two_sided_identity<BOp, T, U> may store references to both T and U, so ensure
		///       that the lifetime of the identity object is exceeded by the lifetime of the objects
		///       of types T and U.
		/// \ref [numeric.identity.two_sided]
		///
		template<class BOp, class T, class U = T>
		requires __has_two_sided_identity<BOp, T, U>
		struct two_sided_identity : private left_identity<BOp, T, U>
			                       , private right_identity<BOp, T, U> {
			constexpr explicit two_sided_identity(const BOp& bop, const T& t, const U& u)
			: left_identity<BOp, T, U>{bop, t, u}
			, right_identity<BOp, T, U>{bop, t, u}
			{
				STL2_EXPECT(static_cast<left_identity<BOp, T, U>&>(*this).value()
					      == static_cast<right_identity<BOp, T, U>&>(*this).value());
			}

			constexpr auto value() const {
				return static_cast<const left_identity<BOp, T, U>&>(*this).value();
			}
		};

		template<class BOp, class T, class U>
		struct two_sided_identity<BOp&, T, U> : two_sided_identity<__uncvref<BOp>, T, U> {};

		template<class BOp, class T, class U = T>
		using two_sided_identity_t = decltype(std::declval<two_sided_identity<BOp, T, U>>().value());
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_NUMERIC_TRAITS_IDENTITY_HPP
