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

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/variant.hpp>
#include <stl2/detail/concepts/compare.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/iterator/operations.hpp>
#include <stl2/detail/memory/addressof.hpp>

///////////////////////////////////////////////////////////////////////////
// common_iterator [common.iterators]
//
STL2_OPEN_NAMESPACE {
	template <Iterator I, Sentinel<I> S>
		requires !Same<I, S>
	class common_iterator;

	namespace __common_iterator {
		template <class T>
		class operator_arrow_proxy {
			T value_;
		public:
			template <class U>
			constexpr explicit operator_arrow_proxy(U&& u)
			noexcept(std::is_nothrow_constructible<T, U>::value)
			requires Constructible<T, U>
			: value_(std::forward<U>(u)) {}

			constexpr const T* operator->() const noexcept {
				return detail::addressof(value_);
			}
		};

		template <class I>
		requires
			Readable<const I> &&
			(std::is_pointer<I>::value ||
				requires(const I& i) { i.operator->(); })
		constexpr I operator_arrow_(const I& i, ext::priority_tag<2>)
		noexcept(std::is_nothrow_copy_constructible<I>::value) {
			return i;
		}

		template <class I>
		requires
			Readable<const I> &&
			_Is<iter_reference_t<const I>, std::is_reference>
		constexpr auto operator_arrow_(const I& i, ext::priority_tag<1>)
		noexcept(noexcept(*i)) {
			auto&& tmp = *i;
			return detail::addressof(tmp);
		}

		template <class I>
		requires
			Readable<const I> &&
			!std::is_reference<iter_reference_t<I>>::value &&
			Constructible<iter_value_t<I>, iter_reference_t<I>>
		constexpr auto operator_arrow_(const I& i, ext::priority_tag<0>)
		noexcept(
			std::is_nothrow_move_constructible<
				operator_arrow_proxy<iter_value_t<I>>>::value &&
			std::is_nothrow_constructible<
				operator_arrow_proxy<iter_value_t<I>>, iter_reference_t<I>>::value) {
			return operator_arrow_proxy<iter_value_t<I>>{*i};
		}

		template <class I>
		requires
			Readable<const I> &&
			requires(const I& i) {
				__common_iterator::operator_arrow_(i, ext::max_priority_tag);
			}
		constexpr decltype(auto) operator_arrow(const I& i)
		STL2_NOEXCEPT_RETURN(
			__common_iterator::operator_arrow_(i, ext::max_priority_tag)
		)

		struct access {
			template <_SpecializationOf<common_iterator> C>
			static constexpr decltype(auto) v(C&& c) noexcept {
				return (std::forward<C>(c).v_);
			}
		};

		template <Iterator I, Sentinel<I> S, ConvertibleTo<I> II, ConvertibleTo<S> SS>
		struct convert_visitor {
			constexpr auto operator()(const II& i) const
			STL2_NOEXCEPT_RETURN(
				std::variant<I, S>{std::in_place_index<0>, i}
			)
			constexpr auto operator()(const SS& s) const
			STL2_NOEXCEPT_RETURN(
				std::variant<I, S>{std::in_place_index<1>, s}
			)
		};

		template <Iterator I, Sentinel<I> S, ConvertibleTo<I> II, ConvertibleTo<S> SS>
		struct assign_visitor {
			std::variant<I, S> *pv_;
			void operator()(const II& i) const
			STL2_NOEXCEPT_RETURN(
				(void)pv_->template emplace<I>(i)
			)
			void operator()(const SS& s) const
			STL2_NOEXCEPT_RETURN(
				(void)pv_->template emplace<S>(s)
			)
		};

		template <Iterator I1, Iterator I2, Sentinel<I2> S1, Sentinel<I1> S2>
		struct equal_visitor {
			constexpr bool operator()(const I1&, const I2&) const noexcept {
				return true;
			}
			constexpr bool operator()(const I1& i1, const I2& i2) const
			noexcept(noexcept(bool(i1 == i2)))
			requires EqualityComparableWith<I1, I2>
			{
				return i1 == i2;
			}
			constexpr bool operator()(const auto& lhs, const auto& rhs) const
			STL2_NOEXCEPT_RETURN(
				bool(lhs == rhs)
			)
			constexpr bool operator()(const S1&, const S2&) const noexcept {
				return true;
			}
		};

		template <class I2, SizedSentinel<I2> I1,
			SizedSentinel<I2> S1, SizedSentinel<I1> S2>
		struct difference_visitor {
			constexpr iter_difference_t<I2> operator()(
				const auto& lhs, const auto& rhs) const
			STL2_NOEXCEPT_RETURN(
				static_cast<iter_difference_t<I2>>(lhs - rhs)
			)
			constexpr iter_difference_t<I2> operator()(
				const S1&, const S2&) const noexcept
			{
				return 0;
			}
		};
	}

	template <Iterator I, Sentinel<I> S>
		requires !Same<I, S>
	class common_iterator {
		template <Iterator II, Sentinel<II> SS>
			requires !Same<II, SS>
		friend class common_iterator;
		friend __common_iterator::access;
		std::variant<I, S> v_;
	public:
		using difference_type = iter_difference_t<I>;

		constexpr common_iterator() = default;

		constexpr common_iterator(I i)
		noexcept(is_nothrow_constructible<std::variant<I, S>, I>::value)
		: v_{std::move(i)} {}

		constexpr common_iterator(S s)
		noexcept(is_nothrow_constructible<std::variant<I, S>, S>::value)
		: v_{std::move(s)} {}

		template <ConvertibleTo<I> II, ConvertibleTo<S> SS>
		constexpr common_iterator(const common_iterator<II, SS>& i)
		noexcept(
			std::is_nothrow_constructible<I, const II&>::value &&
			std::is_nothrow_constructible<S, const SS&>::value)
		: v_{std::visit(__common_iterator::convert_visitor<I, S, II, SS>{}, i.v_)}
		{}

		template <ConvertibleTo<I> II, ConvertibleTo<S> SS>
		common_iterator& operator=(const common_iterator<II, SS>& i)
		noexcept(
			std::is_nothrow_assignable<I&, const II&>::value &&
			std::is_nothrow_assignable<S&, const SS&>::value)
		{
			std::visit(
				__common_iterator::assign_visitor<I, S, II, SS>{&v_}, i.v_);
			return *this;
		}

		decltype(auto) operator*()
		noexcept(noexcept(*std::declval<I&>())) {
			STL2_EXPECT(std::holds_alternative<I>(v_));
			return *std::get<I>(v_);
		}
		decltype(auto) operator*() const
		noexcept(noexcept(*std::declval<const I&>()))
		requires detail::Dereferenceable<const I> {
			STL2_EXPECT(std::holds_alternative<I>(v_));
			return *__stl2::__get_unchecked<I>(v_);
		}
		decltype(auto) operator->() const
		noexcept(noexcept(__common_iterator::operator_arrow(std::declval<const I&>())))
		requires Readable<const I> {
			STL2_EXPECT(std::holds_alternative<I>(v_));
			return __common_iterator::operator_arrow(__stl2::__get_unchecked<I>(v_));
		}

		common_iterator& operator++()
		noexcept(noexcept(++declval<I&>()))	{
			STL2_EXPECT(std::holds_alternative<I>(v_));
			++__stl2::__get_unchecked<I>(v_);
			return *this;
		}
		decltype(auto) operator++(int)
		noexcept(noexcept((decltype(declval<I&>()++))declval<I&>()++)) {
			STL2_EXPECT(std::holds_alternative<I>(v_));
			return __stl2::__get_unchecked<I>(v_)++;
		}
		common_iterator operator++(int)
		noexcept(noexcept(common_iterator(common_iterator(++declval<common_iterator&>()))))
		requires ForwardIterator<I> {
			STL2_EXPECT(std::holds_alternative<I>(v_));
			auto tmp(*this);
			++__stl2::__get_unchecked<I>(v_);
			return tmp;
		}

		friend iter_rvalue_reference_t<I> iter_move(
			const common_iterator& i)
			noexcept(noexcept(__stl2::iter_move(std::declval<const I&>())))
			requires InputIterator<I> {
			STL2_EXPECT(std::holds_alternative<I>(i.v_));
			return __stl2::iter_move(__stl2::__get_unchecked<I>(i.v_));
		}

		template <IndirectlySwappable<I> I2, class S2>
		friend void iter_swap(const common_iterator& x,
			const common_iterator<I2, S2>& y)
			noexcept(noexcept(__stl2::iter_swap(std::declval<const I&>(),
				std::declval<const I2&>()))) {
			STL2_EXPECT(std::holds_alternative<I>(x.v_));
			STL2_EXPECT(std::holds_alternative<I2>(__common_iterator::access::v(y)));
			__stl2::iter_swap(__stl2::__get_unchecked<I>(x.v_),
				__stl2::__get_unchecked<I2>(__common_iterator::access::v(y)));
		}
	};

	template <Readable I, class S>
	struct readable_traits<common_iterator<I, S>> {
		using type = iter_value_t<I>;
	};
	template <InputIterator I, class S>
	struct iterator_category<common_iterator<I, S>> {
		using type = input_iterator_tag;
	};
	template <ForwardIterator I, class S>
	struct iterator_category<common_iterator<I, S>> {
		using type = forward_iterator_tag;
	};

	template <class I1, class I2, Sentinel<I2> S1, Sentinel<I1> S2>
	bool operator==(
		const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
	STL2_NOEXCEPT_RETURN(
		std::visit(__common_iterator::equal_visitor<I1, I2, S1, S2>{},
				   __common_iterator::access::v(x),
				   __common_iterator::access::v(y))
	)

	template <class I1, class I2, Sentinel<I2> S1, Sentinel<I1> S2>
	bool operator!=(
		const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
	STL2_NOEXCEPT_RETURN(
		!(x == y)
	)

	template <class I2, SizedSentinel<I2> I1, SizedSentinel<I2> S1,
		SizedSentinel<I1> S2>
	iter_difference_t<I2> operator-(
		const common_iterator<I1, S1>& x, const common_iterator<I2, S2>& y)
	STL2_NOEXCEPT_RETURN(
		std::visit(__common_iterator::difference_visitor<I1, I2, S1, S2>{},
				   __common_iterator::access::v(x),
				   __common_iterator::access::v(y))
	)
} STL2_CLOSE_NAMESPACE

#endif
