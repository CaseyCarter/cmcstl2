// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Casey Carter 2015
//  Copyright Eric Niebler 2014-2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_BASIC_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_BASIC_ITERATOR_HPP

#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/raw_ptr.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/fundamental.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/iterator/concepts.hpp>
#include <stl2/detail/memory/addressof.hpp>

// TODO:
// * Fix the noexcept clauses that assume that get() is noexcept.
// * Think long and hard about the various proxies and reference
//   validity requirements.

STL2_OPEN_NAMESPACE {
	template <ext::DestructibleObject T>
	requires std::is_class<T>::value && !std::is_final<T>::value
	class basic_mixin : T {
	public:
		basic_mixin() = default;

		constexpr explicit basic_mixin(const T& t)
		noexcept(std::is_nothrow_copy_constructible<T>::value)
		requires CopyConstructible<T>
		: T(t) {}

		constexpr explicit basic_mixin(T&& t)
		noexcept(std::is_nothrow_move_constructible<T>::value)
		requires MoveConstructible<T>
		: T(std::move(t)) {}

	protected:
		constexpr T& get() & noexcept { return *this; }
		constexpr const T& get() const& noexcept { return *this; }
		constexpr T&& get() && noexcept { return std::move(*this); }
		constexpr const T&& get() const&& noexcept { return std::move(*this); }
	};

	namespace detail {
		template <class T>
		struct mixin_base {
			using type = basic_mixin<T>;
		};
		template <class T>
		requires requires { typename T::mixin; }
		struct mixin_base<T> {
			using type = typename T::mixin;
		};

		template <class T>
		constexpr bool IsValueType = !is_void<T>::value;
	} // namespace detail
	template <class T>
	requires
		requires {
			typename meta::_t<detail::mixin_base<T>>;
			requires ext::DestructibleObject<meta::_t<detail::mixin_base<T>>>;
			requires std::is_class<meta::_t<detail::mixin_base<T>>>::value;
			requires !std::is_final<meta::_t<detail::mixin_base<T>>>::value;
		}
	using mixin_t = meta::_t<detail::mixin_base<T>>;

	namespace cursor {
		// This would perhaps more appropriately be named "disable_multi_pass." It
		// is an opt-out trait that tells basic_iterator that an Input cursor is NOT
		// Forward despite having equality.
		template <class>
		constexpr bool single_pass = false;
		template <class C>
		requires
			requires {
				typename C::single_pass;
				requires bool(C::single_pass::value);
			}
		constexpr bool single_pass<C> = true;

		// A RandomAccess cursor whose reference_t is a reference type and
		// whose member type "contiguous" looks like true_type will generate
		// a ContiguousIterator. i.e., this is an opt-in trait for contiguity.
		template <class>
		constexpr bool contiguous = false;
		template <class C>
		requires
			requires {
				typename C::contiguous;
				requires bool(C::contiguous::value);
			}
		constexpr bool contiguous<C> =  true;

		template <class>
		struct difference_type {
			using type = std::ptrdiff_t;
		};
		template <detail::MemberDifferenceType C>
		struct difference_type<C> {
			using type = typename C::difference_type;
			static_assert(SignedIntegral<type>,
				"Cursor's member difference_type is not a signed integer type.");
		};
		template <class C>
		requires
			!detail::MemberDifferenceType<C> &&
			requires(const C& lhs, const C& rhs) { rhs.distance_to(lhs); }
		struct difference_type<C> {
			using type = decltype(declval<const C&>().distance_to(declval<const C&>()));
			static_assert(SignedIntegral<type>,
				"Return type of Cursor's member distance_to is not a signed integer type.");
		};
		template <class C>
		requires
			SignedIntegral<meta::_t<difference_type<C>>>
		using difference_type_t = meta::_t<difference_type<C>>;

		template <class> struct reference_type {};
		template <class C>
		requires
			requires(const C& c) { { c.read() } -> auto&&; }
		struct reference_type<C> {
			using type = decltype(declval<const C&>().read());
		};
		template <class C>
		using reference_t = meta::_t<reference_type<C>>;

		template <class> struct value_type {};
		template <detail::MemberValueType C>
		struct value_type<C> {
			using type = typename C::value_type;
			static_assert(detail::IsValueType<type>,
				"Cursor's member \"value_type\" is not a value type.");
		};
		template <class C>
		requires
			!detail::MemberValueType<C> && requires { typename reference_t<C>; }
		struct value_type<C> {
			using type = decay_t<reference_t<C>>;
			static_assert(detail::IsValueType<type>,
				"Cursor's reference type does not decay to a value type.");
		};
		template <class C>
		requires
			detail::IsValueType<meta::_t<value_type<C>>>
		using value_type_t = meta::_t<value_type<C>>;

		template <class C, class M>
		concept bool _Cursor =
			Semiregular<C> &&
			Semiregular<M> &&
			Constructible<M, C> &&
			Constructible<M, const C&>;

		template <class C>
		concept bool Cursor =
			requires {
				typename difference_type_t<C>;
				typename mixin_t<remove_cv_t<C>>;
			} &&
			_Cursor<remove_cv_t<C>, mixin_t<remove_cv_t<C>>>;

		template <class C>
		concept bool Readable =
			Cursor<C> &&
			requires(const C& c) {
				{ c.read() } -> auto&&;
				typename reference_t<C>;
				typename value_type_t<C>;
			};
		template <class C>
		concept bool Arrow =
			Readable<C> &&
			requires(const C& c) {
				{ c.arrow() } -> auto&&;
			};
		template <class C, class T>
		concept bool Writable =
			Cursor<C> &&
			requires(C& c, T&& t) {
				c.write(std::forward<T>(t)); // Not required to be equality-preserving
			};

		template <class S, class C>
		concept bool Sentinel =
			Cursor<C> &&
			Semiregular<S> &&
			requires(const C& c, const S& s) {
				{ c.equal(s) } -> bool;
			};
		template <class S, class C>
		concept bool SizedSentinel =
			Sentinel<S, C> &&
			requires(const C& c, const S& s) {
				{ c.distance_to(s) } -> Same<difference_type_t<C>>&&;
			};

		template <class C>
		concept bool Next =
			Cursor<C> && requires(C& c) { c.next(); };
		template <class C>
		concept bool PostIncrement =
			Cursor<C> &&
			requires(C& c) {
				c.post_increment();
			};
		template <class C>
		concept bool Prev =
			Cursor<C> && requires(C& c) { c.prev(); };
		template <class C>
		concept bool Advance =
			Cursor<C> && requires(C& c, difference_type_t<C> n) {
				c.advance(n);
			};

		template <class C>
		concept bool IndirectMove =
			Readable<C> && requires(const C& c) {
				{ c.indirect_move() } -> auto&&;
			};

		template <class> struct rvalue_reference {};
		template <class C>
		requires
			Readable<C>
		struct rvalue_reference<C> {
			using type = meta::if_<
				is_reference<reference_t<C>>,
				remove_reference_t<reference_t<C>>&&,
				decay_t<reference_t<C>>>;
		};
		template <class C>
		requires
			IndirectMove<C>
		struct rvalue_reference<C> {
			using type = decltype(declval<const C&>().indirect_move());
		};
		template <class C>
		using rvalue_reference_t = meta::_t<rvalue_reference<C>>;

		template <class C1, class C2>
		concept bool IndirectSwap =
			Readable<C1> &&
			Readable<C2> &&
			requires(const C1& c1, const C2& c2) {
				c1.indirect_swap(c2);
				c2.indirect_swap(c1);
				// Axiom: If c1.read() == x and c2.read() == y then after either
				//   c1.indirect_swap(c2) or c2.indirect_swap(c1), c1.read() == y
				//   and c2.read() == x.
			};

		template <class C>
		concept bool Input =
			Readable<C> && Next<C>;
		template <class C>
		concept bool Forward =
			Input<C> && !single_pass<C> && Sentinel<C, C>;
		template <class C>
		concept bool Bidirectional =
			Forward<C> && Prev<C>;
		template <class C>
		concept bool RandomAccess =
			Bidirectional<C> && Advance<C> && SizedSentinel<C, C>;
		template <class C>
		concept bool Contiguous =
			RandomAccess<C> &&
			contiguous<C> &&
			is_lvalue_reference<reference_t<C>>::value;

		template <class From, class To>
		concept bool ConvertibleTo =
			Cursor<From> &&
			Cursor<To> &&
			__stl2::ConvertibleTo<From, To> &&
			Constructible<mixin_t<To>, From> &&
			Constructible<mixin_t<To>, const From&>;

		template <class>
		struct category {};
		template <Input C>
		struct category<C> { using type = input_iterator_tag; };
		template <Forward C>
		struct category<C> { using type = forward_iterator_tag; };
		template <Bidirectional C>
		struct category<C> { using type = bidirectional_iterator_tag; };
		template <RandomAccess C>
		struct category<C> { using type = random_access_iterator_tag; };
		template <Contiguous C>
		struct category<C> { using type = ext::contiguous_iterator_tag; };
		template <class C>
		using category_t = meta::_t<category<C>>;
	}

	template <cursor::Cursor>
	class basic_iterator;

	namespace detail {
		template <class Derived, class Head>
		struct proxy_reference_conversion {
			operator Head() const
			noexcept(noexcept(Head(Head(declval<const Derived&>().get_()))))
			{
				return Head(static_cast<const Derived*>(this)->get_());
			}
		};

		template <class>
		struct cursor_traits {
		private:
			struct private_ {};
		public:
			using value_t_ = private_;
			using reference_t_ = private_;
			using rvalue_reference_t_ = private_;
			using common_refs = meta::list<>;
		};

		cursor::Readable{C}
		struct cursor_traits<C> {
			using value_t_ = cursor::value_type_t<C>;
			using reference_t_ = cursor::reference_t<C>;
			using rvalue_reference_t_ = cursor::rvalue_reference_t<C>;
		private:
			using R1 = reference_t_;
			using R2 = common_reference_t<reference_t_&&, value_t_&>;
			using R3 = common_reference_t<reference_t_&&, rvalue_reference_t_&&>;
			using tmp1 = meta::list<value_t_, R1>;
			using tmp2 =
				meta::if_<meta::in<tmp1, __uncvref<R2>>, tmp1, meta::push_back<tmp1, R2>>;
			using tmp3 =
				meta::if_<meta::in<tmp2, __uncvref<R3>>, tmp2, meta::push_back<tmp2, R3>>;
		public:
			using common_refs = meta::unique<meta::pop_front<tmp3>>;
		};

		template <class Cur>
		struct basic_proxy_reference
		: cursor_traits<Cur>
		, meta::inherit<
				meta::transform<
					typename cursor_traits<Cur>::common_refs,
					meta::bind_front<
						meta::quote<proxy_reference_conversion>,
						basic_proxy_reference<Cur>>>>
		{
		private:
			raw_ptr<Cur> cur_;
			template <class>
			friend struct basic_proxy_reference;
			template <class, class>
			friend struct proxy_reference_conversion;
			using typename cursor_traits<Cur>::value_t_;
			using typename cursor_traits<Cur>::reference_t_;
			using typename cursor_traits<Cur>::rvalue_reference_t_;
			static_assert(CommonReference<value_t_&, reference_t_&&>,
				"Your readable and writable cursor must have a value type and a reference "
				"type that share a common reference type. See the ranges::common_reference "
				"type trait.");

			constexpr reference_t_ get_() const
			STL2_NOEXCEPT_RETURN(
				reference_t_(cur_->read())
			)
			template <class T>
			constexpr void set_(T&& t) const
			STL2_NOEXCEPT_RETURN(
				static_cast<void>(cur_->write(static_cast<T&&>(t)))
			)

		public:
			basic_proxy_reference() = default;
			basic_proxy_reference(const basic_proxy_reference&) = default;
			template <class OtherCur>
			requires ConvertibleTo<OtherCur*, Cur*>
			constexpr basic_proxy_reference(
				const basic_proxy_reference<OtherCur>& that) noexcept
			: cur_(that.cur_)
			{}
			explicit constexpr basic_proxy_reference(Cur& cur) noexcept
			: cur_(&cur)
			{}
			constexpr basic_proxy_reference& operator=(basic_proxy_reference&& that)
			requires cursor::Readable<Cur>
			{
				return *this = that;
			}
			constexpr basic_proxy_reference& operator=(const basic_proxy_reference& that)
			requires cursor::Readable<Cur>
			{
				this->set_(that.get_());
				return *this;
			}
			constexpr const basic_proxy_reference& operator=(basic_proxy_reference&& that) const
			requires cursor::Readable<Cur>
			{
				return *this = that;
			}
			constexpr const basic_proxy_reference& operator=(const basic_proxy_reference& that) const
			requires cursor::Readable<Cur>
			{
				this->set_(that.get_());
				return *this;
			}

			template <class OtherCur>
			requires
				cursor::Readable<OtherCur> &&
				cursor::Writable<Cur, cursor::reference_t<OtherCur>>
			constexpr basic_proxy_reference& operator=(
				basic_proxy_reference<OtherCur>&& that)
			{
				return *this = that;
			}
			template <class OtherCur>
			requires
				cursor::Readable<OtherCur> &&
				cursor::Writable<Cur, cursor::reference_t<OtherCur>>
			constexpr basic_proxy_reference& operator=(
				const basic_proxy_reference<OtherCur>& that)
			{
				this->set_(that.get_());
				return *this;
			}
			template <class OtherCur>
			requires
				cursor::Readable<OtherCur> &&
				cursor::Writable<Cur, cursor::reference_t<OtherCur>>
			constexpr const basic_proxy_reference& operator=(
				basic_proxy_reference<OtherCur>&& that) const
			{
				return *this = that;
			}
			template <class OtherCur>
			requires
				cursor::Readable<OtherCur> &&
				cursor::Writable<Cur, cursor::reference_t<OtherCur>>
			constexpr const basic_proxy_reference& operator=(
				const basic_proxy_reference<OtherCur>& that) const
			{
				this->set_(that.get_());
				return *this;
			}
			template <class T>
			requires cursor::Writable<Cur, T&&>
			constexpr basic_proxy_reference& operator=(T&& t)
			{
				this->set_(static_cast<T&&>(t));
				return *this;
			}
			template <class T>
			requires cursor::Writable<Cur, T&&>
			constexpr const basic_proxy_reference& operator=(T&& t) const
			{
				this->set_(static_cast<T&&>(t));
				return *this;
			}
			friend constexpr bool operator==(
				const basic_proxy_reference& x, const value_t_& y)
			requires cursor::Readable<Cur> && EqualityComparable<value_t_>
			{
				return x.get_() == y;
			}
			friend constexpr bool operator!=(
				const basic_proxy_reference& x, const value_t_& y)
			requires cursor::Readable<Cur> && EqualityComparable<value_t_>
			{
				return !(x == y);
			}
			friend constexpr bool operator==(
				const value_t_& x, const basic_proxy_reference& y)
			requires cursor::Readable<Cur> && EqualityComparable<value_t_>
			{
				return x == y.get_();
			}
			friend constexpr bool operator!=(
				const value_t_& x, const basic_proxy_reference& y)
			requires cursor::Readable<Cur> && EqualityComparable<value_t_>
			{
				return !(x == y);
			}
			friend constexpr bool operator==(
				const basic_proxy_reference& x, const basic_proxy_reference& y)
			requires cursor::Readable<Cur> && EqualityComparable<value_t_>
			{
				return x.get_() == y.get_();
			}
			friend constexpr bool operator!=(
				const basic_proxy_reference& x, const basic_proxy_reference& y)
			requires cursor::Readable<Cur> && EqualityComparable<value_t_>
			{
				return !(x == y);
			}
		};

		template <class>
		constexpr bool is_writable = true;
		template <class C>
		requires
			cursor::Readable<remove_cv_t<C>>
		constexpr bool is_writable<C> = false;
		template <class C>
		requires
			cursor::Readable<remove_cv_t<C>> &&
			cursor::Writable<C, cursor::value_type_t<C>&&>
		constexpr bool is_writable<C> = true;

		template <class Cur>
		struct iterator_associated_types_base {
			using reference_t = basic_proxy_reference<Cur>;
			using const_reference_t = basic_proxy_reference<const Cur>;
		};

		cursor::Readable{Cur}
		struct iterator_associated_types_base<Cur> {
			using reference_t =
				meta::if_c<
					is_writable<const Cur>,
					basic_proxy_reference<const Cur>,
					meta::if_c<
						is_writable<Cur>,
						basic_proxy_reference<Cur>,
						cursor::reference_t<Cur>>>;
			using const_reference_t =
				meta::if_c<
					is_writable<const Cur>,
					basic_proxy_reference<const Cur>,
					cursor::reference_t<Cur>>;
		};

		template <class C>
		concept bool PostIncrementCursor =
			requires(C& c) {
				{ c.post_increment() } -> Same<C>&&;
			};
	} // namespace detail

	// common_reference specializations for basic_proxy_reference
	template <cursor::Readable Cur, class U,
		template <class> class TQual, template <class> class UQual>
	struct basic_common_reference<
		detail::basic_proxy_reference<Cur>, U, TQual, UQual>
	: basic_common_reference<cursor::reference_t<Cur>, U, TQual, UQual>
	{};
	template <class T, cursor::Readable Cur,
		template <class> class TQual, template <class> class UQual>
	struct basic_common_reference<
		T, detail::basic_proxy_reference<Cur>, TQual, UQual>
	: basic_common_reference<T, cursor::reference_t<Cur>, TQual, UQual>
	{};
	template <cursor::Readable Cur1, cursor::Readable Cur2,
		template <class> class TQual, template <class> class UQual>
	struct basic_common_reference<
		detail::basic_proxy_reference<Cur1>, detail::basic_proxy_reference<Cur2>,
		TQual, UQual>
	: basic_common_reference<
			cursor::reference_t<Cur1>, cursor::reference_t<Cur2>, TQual, UQual>
	{};

	// common_type specializations for basic_proxy_reference
	template <cursor::Readable Cur, class U>
	struct common_type<detail::basic_proxy_reference<Cur>, U>
	: common_type<cursor::value_type_t<Cur>, U>
	{};
	template <class T, cursor::Readable Cur>
	struct common_type<T, detail::basic_proxy_reference<Cur>>
	: common_type<T, cursor::value_type_t<Cur>>
	{};
	template <cursor::Readable Cur1, cursor::Readable Cur2>
	struct common_type<
		detail::basic_proxy_reference<Cur1>, detail::basic_proxy_reference<Cur2>>
	: common_type<cursor::value_type_t<Cur1>, cursor::value_type_t<Cur2>>
	{};

	template <_SpecializationOf<basic_iterator> BI>
	constexpr decltype(auto) get_cursor(BI&& i)
	STL2_NOEXCEPT_RETURN(
		std::forward<BI>(i).get()
	)

	namespace basic_iterator_adl {
		struct hook {};

		template <class C>
		requires cursor::IndirectMove<C>
		constexpr decltype(auto) iter_move(const basic_iterator<C>& i)
		STL2_NOEXCEPT_RETURN(
			i.get().indirect_move()
		)

		template <class C1, class C2>
		requires cursor::IndirectSwap<C1, C2>
		constexpr void iter_swap(
			const basic_iterator<C1>& x, const basic_iterator<C2>& y)
		STL2_NOEXCEPT_RETURN(
			static_cast<void>(x.get().indirect_swap(y.get()))
		)
	} // namespace basic_iterator_adl

	cursor::Cursor{C}
	class basic_iterator
	: public mixin_t<C>
	, detail::iterator_associated_types_base<C>
	, basic_iterator_adl::hook
	{
		template <_SpecializationOf<basic_iterator> BI>
		friend constexpr decltype(auto) get_cursor(BI&& i);

		using mixin = mixin_t<C>;
		using mixin::get;

		using assoc_t = detail::iterator_associated_types_base<C>;
		using typename assoc_t::reference_t;
		using typename assoc_t::const_reference_t;

		using D = cursor::difference_type_t<C>;

	public:
		basic_iterator() = default;
		template <cursor::ConvertibleTo<C> O>
		constexpr basic_iterator(basic_iterator<O>&& that)
		noexcept(std::is_nothrow_constructible<mixin, O>::value)
		: mixin(__stl2::get_cursor(std::move(that))) {}
		template <cursor::ConvertibleTo<C> O>
		constexpr basic_iterator(const basic_iterator<O>& that)
		noexcept(std::is_nothrow_constructible<mixin, const O&>::value)
		: mixin(__stl2::get_cursor(that)) {}
		constexpr explicit basic_iterator(const C& c)
		noexcept(std::is_nothrow_constructible<mixin, const C&>::value)
		: mixin(c) {}
		constexpr explicit basic_iterator(C&& c)
		noexcept(std::is_nothrow_constructible<mixin, C>::value)
		: mixin(std::move(c)) {}
		using mixin::mixin;

		template <cursor::ConvertibleTo<C> O>
		constexpr basic_iterator& operator=(basic_iterator<O>&& that) &
		noexcept(is_nothrow_assignable<C&, O>::value)
		{
			get() = __stl2::get_cursor(std::move(that));
			return *this;
		}
		template <cursor::ConvertibleTo<C> O>
		constexpr basic_iterator& operator=(const basic_iterator<O>& that) &
		noexcept(is_nothrow_assignable<C&, const O&>::value)
		{
			get() = __stl2::get_cursor(that);
			return *this;
		}
		template <class T>
		requires
			!Same<decay_t<T>, basic_iterator> && !cursor::Next<C> &&
			cursor::Writable<C, T>
		constexpr basic_iterator& operator=(T&& t)
		noexcept(noexcept(
			declval<C&>().write(static_cast<T&&>(t))))
		{
			get().write(static_cast<T&&>(t));
			return *this;
		}
		// Not to spec: This operator= needs to be proposed for
		// http://wg21.link/P0186
		template <class O>
		requires
			!Same<decay_t<O>, basic_iterator> &&
			Assignable<C&, O>
		constexpr basic_iterator& operator=(O&& o) &
		noexcept(is_nothrow_assignable<C&, O>::value)
		{
			get() = std::forward<O>(o);
			return *this;
		}

		constexpr decltype(auto) operator*() const
		noexcept(noexcept(declval<const C&>().read()))
		requires cursor::Readable<C> && !detail::is_writable<C>
		{
			return get().read();
		}
		constexpr decltype(auto) operator*()
		noexcept(noexcept(reference_t{declval<mixin&>().get()}))
		requires cursor::Next<C> && detail::is_writable<C>
		{
			return reference_t{get()};
		}
		constexpr decltype(auto) operator*() const
		noexcept(noexcept(
			const_reference_t{declval<const mixin&>().get()}))
		requires cursor::Next<C> && detail::is_writable<C>
		{
			return const_reference_t{get()};
		}
		constexpr basic_iterator& operator*() noexcept
		requires !cursor::Next<C>
		{
			return *this;
		}

		// Use cursor's arrow() member, if any.
		constexpr decltype(auto) operator->() const
		noexcept(noexcept(std::declval<const C&>().arrow()))
		requires cursor::Arrow<C>
		{
			return get().arrow();
		}
		// Otherwise, if reference_t is an lvalue reference to cv-qualified
		// value_type_t, return the address of **this.
		template<class BugsBugs = C> // Workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71965
		constexpr auto operator->() const
		noexcept(noexcept(*std::declval<const basic_iterator&>()))
		requires
			!cursor::Arrow<C> && cursor::Readable<C> &&
			std::is_lvalue_reference<const_reference_t>::value &&
			Same<cursor::value_type_t<BugsBugs>, __uncvref<const_reference_t>>
		{
			return detail::addressof(**this);
		}

		constexpr basic_iterator& operator++() & noexcept {
			return *this;
		}
		constexpr basic_iterator& operator++() &
		noexcept(noexcept(declval<C&>().next()))
		requires cursor::Next<C>
		{
			get().next();
			return *this;
		}

		constexpr basic_iterator operator++(int) &
		noexcept(is_nothrow_copy_constructible<basic_iterator>::value &&
			is_nothrow_move_constructible<basic_iterator>::value &&
			noexcept(++declval<basic_iterator&>()))
		{
			auto tmp(*this);
			++*this;
			return tmp;
		}

		constexpr void operator++(int) &
		noexcept(noexcept(++declval<basic_iterator&>()))
		requires cursor::Input<C> && !cursor::Forward<C>
			&& !cursor::PostIncrement<C>
		{
			++*this;
		}

		constexpr decltype(auto) operator++(int) &
		noexcept(noexcept(declval<C&>().post_increment()))
		requires cursor::PostIncrement<C>
		{
			return get().post_increment();
		}
		constexpr basic_iterator operator++(int) &
		noexcept(noexcept(basic_iterator{std::declval<C&>().post_increment()}))
		requires
			cursor::PostIncrement<C> &&
			detail::PostIncrementCursor<C>
		{
			return basic_iterator{get().post_increment()};
		}

		constexpr basic_iterator& operator--() &
		noexcept(noexcept(declval<C&>().prev()))
		requires cursor::Bidirectional<C>
		{
			get().prev();
			return *this;
		}
		constexpr basic_iterator operator--(int) &
		noexcept(is_nothrow_copy_constructible<basic_iterator>::value &&
			is_nothrow_move_constructible<basic_iterator>::value &&
			noexcept(--declval<basic_iterator&>()))
		requires cursor::Bidirectional<C>
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		constexpr basic_iterator& operator+=(D n) &
		noexcept(noexcept(declval<C&>().advance(n)))
		requires cursor::RandomAccess<C>
		{
			get().advance(n);
			return *this;
		}
		constexpr basic_iterator& operator-=(D n) &
		noexcept(noexcept(declval<C&>().advance(-n)))
		requires cursor::RandomAccess<C>
		{
			get().advance(-n);
			return *this;
		}

		constexpr decltype(auto) operator[](D n) const
		noexcept(noexcept(*(declval<basic_iterator&>() + n)))
		requires cursor::RandomAccess<C>
		{
			return *(*this + n);
		}

		// non-template type-symmetric operators to enable
		// implicit conversions.
		friend constexpr bool operator==(
			const basic_iterator& x, const basic_iterator& y)
		noexcept(noexcept(x.get().equal(y.get())))
		requires cursor::Sentinel<C, C>
		{
			return x.get().equal(y.get());
		}
		friend constexpr bool operator!=(
			const basic_iterator& x, const basic_iterator& y)
		noexcept(noexcept(!(x == y)))
		requires cursor::Sentinel<C, C>
		{
			return !(x == y);
		}

		friend constexpr D operator-(
			const basic_iterator& x, const basic_iterator& y)
		noexcept(noexcept(y.get().distance_to(x.get())))
		requires cursor::SizedSentinel<C, C>
		{
			return y.get().distance_to(x.get());
		}

		friend constexpr bool operator<(
			const basic_iterator& x, const basic_iterator& y)
		noexcept(noexcept(x - y))
		requires cursor::SizedSentinel<C, C>
		{
			return x - y < 0;
		}
		friend constexpr bool operator>(
			const basic_iterator& x, const basic_iterator& y)
		noexcept(noexcept(x - y))
		requires cursor::SizedSentinel<C, C>
		{
			return x - y > 0;
		}
		friend constexpr bool operator<=(
			const basic_iterator& x, const basic_iterator& y)
		noexcept(noexcept(x - y))
		requires cursor::SizedSentinel<C, C>
		{
			return x - y <= 0;
		}
		friend constexpr bool operator>=(
			const basic_iterator& x, const basic_iterator& y)
		noexcept(noexcept(x - y))
		requires cursor::SizedSentinel<C, C>
		{
			return x - y >= 0;
		}
	};

	template <class C>
	struct incrementable_traits<basic_iterator<C>> {
		using type = cursor::difference_type_t<C>;
	};

	template <cursor::Input C>
	struct iterator_category<basic_iterator<C>> {
		using type = cursor::category_t<C>;
	};

	template <cursor::Input C>
	struct readable_traits<basic_iterator<C>> {
		using type = cursor::value_type_t<C>;
	};

	template <class C>
	constexpr basic_iterator<C> operator+(
		const basic_iterator<C>& i, cursor::difference_type_t<C> n)
	noexcept(is_nothrow_copy_constructible<basic_iterator<C>>::value &&
		is_nothrow_move_constructible<basic_iterator<C>>::value &&
		noexcept(declval<basic_iterator<C>&>() += n))
	requires cursor::RandomAccess<C>
	{
		auto tmp = i;
		tmp += n;
		return tmp;
	}
	template <class C>
	constexpr basic_iterator<C> operator+(
		cursor::difference_type_t<C> n, const basic_iterator<C>& i)
	noexcept(noexcept(i + n))
	requires cursor::RandomAccess<C>
	{
		return i + n;
	}
	template <class C>
	constexpr basic_iterator<C> operator-(
		const basic_iterator<C>& i, cursor::difference_type_t<C> n)
	noexcept(noexcept(i + -n))
	requires cursor::RandomAccess<C>
	{
		return i + -n;
	}

	template <class C1, class C2>
	requires cursor::Sentinel<C2, C1>
	constexpr bool operator==(
		const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
	STL2_NOEXCEPT_RETURN(
		__stl2::get_cursor(lhs).equal(__stl2::get_cursor(rhs))
	)

	template <class C, class S>
	requires cursor::Sentinel<S, C>
	constexpr bool operator==(
		const basic_iterator<C>& lhs, const S& rhs)
	STL2_NOEXCEPT_RETURN(
		__stl2::get_cursor(lhs).equal(rhs)
	)

	template <class C, class S>
	requires cursor::Sentinel<S, C>
	constexpr bool operator==(
		const S& lhs, const basic_iterator<C>& rhs)
	STL2_NOEXCEPT_RETURN(
		rhs == lhs
	)

	template <class C1, class C2>
	requires cursor::Sentinel<C2, C1>
	constexpr bool operator!=(
		const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
	STL2_NOEXCEPT_RETURN(
		!(lhs == rhs)
	)

	template <class C, class S>
	requires cursor::Sentinel<S, C>
	constexpr bool operator!=(
		const basic_iterator<C>& lhs, const S& rhs)
	STL2_NOEXCEPT_RETURN(
		!__stl2::get_cursor(lhs).equal(rhs)
	)

	template <class C, class S>
	requires cursor::Sentinel<S, C>
	constexpr bool operator!=(
		const S& lhs, const basic_iterator<C>& rhs)
	STL2_NOEXCEPT_RETURN(
		!__stl2::get_cursor(rhs).equal(lhs)
	)

	template <class C1, class C2>
	requires cursor::SizedSentinel<C1, C2>
	constexpr cursor::difference_type_t<C2> operator-(
		const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
	STL2_NOEXCEPT_RETURN(
		__stl2::get_cursor(rhs).distance_to(__stl2::get_cursor(lhs))
	)

	template <class C, class S>
	requires cursor::SizedSentinel<S, C>
	constexpr cursor::difference_type_t<C> operator-(
		const S& lhs, const basic_iterator<C>& rhs)
	STL2_NOEXCEPT_RETURN(
		__stl2::get_cursor(rhs).distance_to(lhs)
	)

	template <class C, class S>
	requires cursor::SizedSentinel<S, C>
	constexpr cursor::difference_type_t<C> operator-(
		const basic_iterator<C>& lhs, const S& rhs)
	STL2_NOEXCEPT_RETURN(
		-(rhs - lhs)
	)

	template <class C1, class C2>
	requires cursor::SizedSentinel<C1, C2>
	constexpr bool operator<(
		const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
	STL2_NOEXCEPT_RETURN(
		lhs - rhs < 0
	)

	template <class C1, class C2>
	requires cursor::SizedSentinel<C1, C2>
	constexpr bool operator>(
		const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
	STL2_NOEXCEPT_RETURN(
		lhs - rhs > 0
	)

	template <class C1, class C2>
	requires cursor::SizedSentinel<C1, C2>
	constexpr bool operator<=(
		const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
	STL2_NOEXCEPT_RETURN(
		lhs - rhs <= 0
	)

	template <class C1, class C2>
	requires cursor::SizedSentinel<C1, C2>
	constexpr bool operator>=(
		const basic_iterator<C1>& lhs, const basic_iterator<C2>& rhs)
	STL2_NOEXCEPT_RETURN(
		lhs - rhs >= 0
	)
} STL2_CLOSE_NAMESPACE

#endif
