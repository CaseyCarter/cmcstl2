// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2015
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_COMMON_ITERATOR_HPP

#include <memory>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/variant.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>

///////////////////////////////////////////////////////////////////////////
// common_iterator [common.iterators]
//
STL2_OPEN_NAMESPACE {
	template<Iterator I, Sentinel<I> S>
	requires (!same_as<I, S>)
	class common_iterator;

	namespace __common_iterator {
		template<class T>
		struct operator_arrow_proxy {
			template<class U>
			constexpr explicit operator_arrow_proxy(U&& u)
			noexcept(std::is_nothrow_constructible_v<T, U>)
			requires constructible_from<T, U>
			: value_(std::forward<U>(u)) {}

			constexpr const T* operator->() const noexcept {
				return std::addressof(value_);
			}
		private:
			T value_;
		};

		struct access {
			template<_SpecializationOf<common_iterator> C>
			static constexpr decltype(auto) v(C&& c) noexcept {
				return (std::forward<C>(c).v_);
			}

			// Not to spec: Here to avoid LLVM 37556
			template<InputIterator I, class S>
			friend iter_rvalue_reference_t<I>
			iter_move(const common_iterator<I, S>& i)
			noexcept(noexcept(__stl2::iter_move(std::declval<const I&>()))) {
				return __stl2::iter_move(__stl2::__unchecked_get<I>(v(i)));
			}
			template<class I1, class S1, IndirectlySwappable<I1> I2, class S2>
			friend void iter_swap(
				const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
			noexcept(noexcept(__stl2::iter_swap(std::declval<const I1&>(),
				std::declval<const I2&>())))
			{
				__stl2::iter_swap(
					__stl2::__unchecked_get<I1>(v(x)),
					__stl2::__unchecked_get<I2>(v(y)));
			}

			// Not to spec: here avoid GCC hidden friend constraint bugs
			template<class I1, class S1, SizedSentinel<I1> I2, SizedSentinel<I1> S2>
			requires SizedSentinel<S1, I2>
			friend iter_difference_t<I2> operator-(
				const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
			{
				return __stl2::__unchecked_visit(
					difference_visitor<I1, S1, I2, S2>{}, v(x), v(y));
			}

		private:
			template<class I1, Sentinel<I1> S1, class I2, Sentinel<I2> S2>
			requires SizedSentinel<I2, I1> && SizedSentinel<S2, I1> &&
				SizedSentinel<S1, I2>
			struct difference_visitor {
				template<class T, class U>
				constexpr iter_difference_t<I2>
				operator()(const T& lhs, const U& rhs) const
				STL2_NOEXCEPT_RETURN(
					static_cast<iter_difference_t<I2>>(lhs - rhs)
				)
				constexpr iter_difference_t<I2>
				operator()(const S1&, const S2&) const noexcept {
					return 0;
				}
			};
		};

		template<class I1, Sentinel<I1> S1, class I2, Sentinel<I2> S2>
		requires convertible_to<const I2&, I1> && convertible_to<const S2&, S1>
		struct convert_visitor {
			constexpr auto operator()(const I2& i) const
			STL2_NOEXCEPT_RETURN(
				std::variant<I1, S1>{std::in_place_index<0>, i}
			)
			constexpr auto operator()(const S2& s) const
			STL2_NOEXCEPT_RETURN(
				std::variant<I1, S1>{std::in_place_index<1>, s}
			)
		};

		template<class I1, Sentinel<I1> S1, class I2, Sentinel<I2> S2>
		requires convertible_to<const I2&, I1> && convertible_to<const S2&, S1> &&
			assignable_from<I1&, const I2&> && assignable_from<S1&, const S2&>
		struct assign_visitor {
			std::variant<I1, S1>& v_;

			void operator()(I1& i1, const I2& i2) const
			STL2_NOEXCEPT_RETURN(
				(void)(i1 = i2)
			)
			void operator()(S1& s1, const S2& s2) const
			STL2_NOEXCEPT_RETURN(
				(void)(s1 = s2)
			)
			void operator()(const S1&, const I2& i2) const
			STL2_NOEXCEPT_RETURN(
				(void)v_.template emplace<I1>(i2)
			)
			void operator()(const I1&, const S2& s2) const
			STL2_NOEXCEPT_RETURN(
				(void)v_.template emplace<S1>(s2)
			)
		};

		template<class I1, Sentinel<I1> S1, class I2, Sentinel<I2> S2>
		requires Sentinel<S1, I2>
		struct equal_visitor {
			constexpr bool operator()(const I1&, const I2&) const noexcept {
				return true;
			}
			constexpr bool operator()(const I1& i1, const I2& i2) const
			noexcept(noexcept(bool(i1 == i2)))
			requires equality_comparable_with<I1, I2> {
				return i1 == i2;
			}
			constexpr bool operator()(const S1&, const S2&) const noexcept {
				return true;
			}
			template<class T, class U>
			constexpr bool operator()(const T& lhs, const U& rhs) const
			STL2_NOEXCEPT_RETURN(
				bool(lhs == rhs)
			)
		};
	}

	template<class I>
	META_CONCEPT _HasArrow = requires(I& i) { i.operator->(); };

	// common_iterator [common.iterator]
	template<Iterator I, Sentinel<I> S>
	requires (!same_as<I, S>)
	class common_iterator
	: __common_iterator::access
	{
		friend __common_iterator::access;

		std::variant<I, S> v_;

	public:
		constexpr common_iterator() = default;

		constexpr common_iterator(I i)
		noexcept(std::is_nothrow_constructible_v<std::variant<I, S>, I>) // strengthened
		: v_{std::in_place_type<I>, std::move(i)} {}

		constexpr common_iterator(S s)
		noexcept(std::is_nothrow_constructible_v<std::variant<I, S>, S>) // strengthened
		: v_{std::in_place_type<S>, std::move(s)} {}

		template<class I2, class S2>
		requires convertible_to<const I2&, I> && convertible_to<const S2&, S>
		constexpr common_iterator(const common_iterator<I2, S2>& i)
		noexcept(
			std::is_nothrow_constructible_v<I, const I2&> &&
			std::is_nothrow_constructible_v<S, const S2&>) // strengthened
		: v_{__stl2::__unchecked_visit(
			__common_iterator::convert_visitor<I, S, I2, S2>{},
			__common_iterator::access::v(i))}
		{}

		template<class I2, class S2>
		requires convertible_to<const I2&, I> && convertible_to<const S2&, S> &&
			assignable_from<I&, const I2&> && assignable_from<S&, const S2&>
		common_iterator& operator=(const common_iterator<I2, S2>& i)
		noexcept(
			std::is_nothrow_assignable_v<I&, const I2&> &&
			std::is_nothrow_assignable_v<S&, const S2&>) // strengthened
		{
			__stl2::__unchecked_visit(
				__common_iterator::assign_visitor<I, S, I2, S2>{v_}, v_,
				__common_iterator::access::v(i));
			return *this;
		}

		decltype(auto) operator*()
		noexcept(noexcept(*std::declval<I&>())) { // strengthened
			return *__stl2::__unchecked_get<I>(v_);
		}
		decltype(auto) operator*() const
		noexcept(noexcept(*std::declval<const I&>())) // strengthened
		requires __dereferenceable<const I> {
			return *__stl2::__unchecked_get<I>(v_);
		}
		decltype(auto) operator->() const
		requires Readable<const I> &&
			(_HasArrow<const I> ||
			 std::is_reference_v<iter_reference_t<I>> ||
			 constructible_from<iter_value_t<I>, iter_reference_t<I>>)
		{
			if constexpr (std::is_pointer_v<I> || _HasArrow<const I>)
				return __stl2::__unchecked_get<I>(v_);
			else if constexpr (std::is_reference_v<iter_reference_t<const I>>) { // TODO: file LWG issue (const I instead of I)
				auto&& tmp = *__stl2::__unchecked_get<I>(v_);
				return std::addressof(tmp);
			} else {
				return __common_iterator::operator_arrow_proxy<iter_value_t<I>>{
					*__stl2::__unchecked_get<I>(v_)
				};
			}
		}

		common_iterator& operator++()
		noexcept(noexcept(++std::declval<I&>())) { // strengthened
			++__stl2::__unchecked_get<I>(v_);
			return *this;
		}
		decltype(auto) operator++(int)
		{
			auto& i = __stl2::__unchecked_get<I>(v_);
			if constexpr (ForwardIterator<I>) {
				auto tmp = *this;
				++i;
				return tmp;
			} else
				return i++;
		}

		template<class I2, Sentinel<I> S2>
		requires Sentinel<S, I2>
		friend bool
		operator==(const common_iterator& x, const common_iterator<I2, S2>& y) {
			return __stl2::__unchecked_visit(
				__common_iterator::equal_visitor<I, S, I2, S2>{}, x.v_,
				__common_iterator::access::v(y));
		}
		template<class I2, Sentinel<I> S2>
		requires Sentinel<S, I2>
		friend bool
		operator!=(const common_iterator& x, const common_iterator<I2, S2>& y) {
			return !(x == y);
		}
	};

	template<class I, class S>
	struct incrementable_traits<common_iterator<I, S>> {
		using difference_type = iter_difference_t<I>;
	};

	template<Readable I, class S>
	struct readable_traits<common_iterator<I, S>> {
		using value_type = iter_value_t<I>;
	};

	template<InputIterator I, class S>
	struct iterator_category<common_iterator<I, S>> {
		using type = input_iterator_tag;
	};
	template<ForwardIterator I, class S>
	struct iterator_category<common_iterator<I, S>> {
		using type = forward_iterator_tag;
	};
} STL2_CLOSE_NAMESPACE

#endif
