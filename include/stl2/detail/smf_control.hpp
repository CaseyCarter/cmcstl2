#ifndef STL2_DETAIL_SMF_CONTROL_HPP
#define STL2_DETAIL_SMF_CONTROL_HPP

#include <stl2/detail/fwd.hpp>

STL2_OPEN_NAMESPACE {
	namespace detail {
		namespace smf_control {
			template<bool> struct copy {};
			template<> struct copy<false> {
				copy() = default;
				copy(const copy&) = delete;
				copy(copy&&) = default;
				copy& operator=(const copy&) = default;
				copy& operator=(copy&&) = default;
			};

			template<bool> struct move {};
			template<> struct move<false> {
				move() = default;
				move(const move&) = default;
				move(move&&) = delete;
				move& operator=(const move&) = default;
				move& operator=(move&&) = default;
			};

			template<bool> struct copy_assign {};
			template<> struct copy_assign<false> {
				copy_assign() = default;
				copy_assign(const copy_assign&) = default;
				copy_assign(copy_assign&&) = default;
				copy_assign& operator=(const copy_assign&) = delete;
				copy_assign& operator=(copy_assign&&) = default;
			};

			template<bool> struct move_assign {};
			template<> struct move_assign<false> {
				move_assign() = default;
				move_assign(const move_assign&) = default;
				move_assign(move_assign&&) = default;
				move_assign& operator=(const move_assign&) = default;
				move_assign& operator=(move_assign&&) = delete;
			};
		} // namespace smf_control
	} // namespace detail
} STL2_CLOSE_NAMESPACE

#endif // STL2_DETAIL_SMF_CONTROL_HPP
