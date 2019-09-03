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
#ifndef STL2_VIEW_GENERATE_HPP
#define STL2_VIEW_GENERATE_HPP

#include <utility>

#include <stl2/type_traits.hpp>
#include <stl2/detail/concepts/function.hpp>
#include <stl2/detail/concepts/object.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/iterator/unreachable.hpp>
#include <stl2/detail/meta.hpp>
#include <stl2/detail/non_propagating_cache.hpp>
#include <stl2/detail/semiregular_box.hpp>
#include <stl2/detail/view/view_closure.hpp>
#include <stl2/view/view_interface.hpp>

STL2_OPEN_NAMESPACE {
	namespace ext {
		template<class F>
		META_CONCEPT __ConstInvocable =
			invocable<F&> &&
			invocable<const F&> &&
			same_as<invoke_result_t<F&>, invoke_result_t<const F&>>;

		template<copy_constructible_object F>
		requires invocable<F&>
		struct STL2_EMPTY_BASES generate_view
		: view_interface<generate_view<F>>
		, private detail::semiregular_box<F>
		, private detail::non_propagating_cache<invoke_result_t<F&>> {
		private:
			using generator_base = detail::semiregular_box<F>;
			using result_t = invoke_result_t<F&>;
			using cache_base = detail::non_propagating_cache<result_t>;

			struct __iterator;

			constexpr cache_base& as_cache() noexcept
			{ return static_cast<cache_base&>(*this); }
		public:
			generate_view() = default;

			explicit constexpr generate_view(F f) noexcept(std::is_nothrow_move_constructible_v<F>)
				: generator_base(std::move(f))
			{}

			constexpr __iterator begin()
			{ return __iterator{*this}; }

			constexpr unreachable end() const noexcept
			{ return {}; }
		};

		template<copy_constructible_object F>
		requires invocable<F&>
		struct generate_view<F>::__iterator {
			using value_type = result_t;
			using difference_type = std::intmax_t;
			using iterator_category = std::input_iterator_tag;

			__iterator() = default;

			explicit __iterator(generate_view& view) : view_{std::addressof(view)} {}

			auto&& operator*()
			requires (!__ConstInvocable<F>)
			{ return deref_impl(*this); }

			auto&& operator*() const
			requires __ConstInvocable<F>
			{ return deref_impl(*this); }

			__iterator& operator++()
			{
				view_->as_cache().reset();
				return *this;
			}

			__iterator& operator++(int)
			{ return ++(*this); }
		private:
			generate_view* view_{};

			template<class Self>
			static auto&& deref_impl(Self&& self)
			{
				auto& view = *self.view_;
				if (!view.as_cache()) {
					view.as_cache().emplace(view.get()());
				}
				using reference_t = std::conditional_t<
					std::is_lvalue_reference_v<result_t>,
					result_t,
					__uncvref<result_t>&&>;
				return static_cast<reference_t>(*view.as_cache());
			}
		};
	} // namespace ext

	namespace views::ext {
		struct __generate_fn : detail::__pipeable<__generate_fn> {
			template<class F>
			constexpr auto operator()(F&& f) const
			STL2_NOEXCEPT_REQUIRES_RETURN(
				__stl2::ext::generate_view{std::forward<F>(f)}
			)
		};

		inline constexpr __generate_fn generate {};
	} // namespace views::ext
} STL2_CLOSE_NAMESPACE

#endif // STL2_VIEW_GENERATE_HPP
