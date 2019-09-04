// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2016
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ITERATOR_ANY_ITERATOR_HPP
#define STL2_DETAIL_ITERATOR_ANY_ITERATOR_HPP

#include <atomic>
#include <exception>
#include <new>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace __any_iterator {
		enum class op { copy, move, nuke, bump, comp, rval };

		template<input_iterator I>
		struct shared_iterator {
			explicit shared_iterator(I i)
			noexcept(std::is_nothrow_move_constructible_v<I>)
			: it(std::move(i)) {}

			STL2_NO_UNIQUE_ADDRESS I it;
			std::atomic<long> cnt{1};
		};

		union blob {
			void* big;
			alignas(std::max_align_t) unsigned char tiny[2 * sizeof(void*)];
		};

		template<class It>
		inline constexpr bool is_small = sizeof(It) <= sizeof(blob::tiny) &&
			alignof(It) <= alignof(std::max_align_t);

		template<class RValueReference>
		using iter_move_fn = RValueReference (*)(const blob&);

		template<class RValueReference>
		inline iter_move_fn<RValueReference> uninit_noop(op, blob*, blob*) {
			return nullptr;
		}

		template<class Reference>
		[[noreturn]] inline Reference uninit_deref(const blob&) {
			std::terminate();
		}

		template<class Reference, input_iterator I>
		Reference deref_small(const blob& src) {
			return *reinterpret_cast<const I&>(src.tiny);
		}

		template<class Reference, input_iterator I>
		Reference deref_big(const blob& src) {
			return *static_cast<const shared_iterator<I>*>(src.big)->it;
		}

		template<class I, class J>
		constexpr bool iter_equal(const I& i, const J& j) {
			if constexpr (sentinel_for<J, I>) {
				return i == j;
			} else {
				return true;
			}
		}

		template<class RValueReference, input_iterator I>
		iter_move_fn<RValueReference> exec_small(op o, blob* src, blob* dst) {
			switch (o) {
			case op::copy:
				::new (static_cast<void*>(&dst->tiny))
					I(reinterpret_cast<const I&>(src->tiny));
				break;
			case op::move:
				::new (static_cast<void*>(&dst->tiny))
					I(reinterpret_cast<I&&>(src->tiny));
				[[fallthrough]];
			case op::nuke:
				reinterpret_cast<I&>(src->tiny).~I();
				break;
			case op::bump:
				++reinterpret_cast<I&>(src->tiny);
				break;
			case op::comp:
				if (__any_iterator::iter_equal(
					reinterpret_cast<const I&>(src->tiny),
					reinterpret_cast<const I&>(dst->tiny))) {
					[[fallthrough]];
			case op::rval:
					return +[](const blob& src) -> RValueReference {
						return iter_move(reinterpret_cast<const I&>(src.tiny));
					};
				}
			}
			return nullptr;
		}

		template<class RValueReference, input_iterator I>
		iter_move_fn<RValueReference> exec_big(op o, blob* src, blob* dst) {
			switch (o) {
			case op::copy:
				dst->big = src->big;
				++static_cast<shared_iterator<I>*>(src->big)->cnt;
				break;
			case op::move:
				dst->big = __stl2::exchange(src->big, nullptr);
				break;
			case op::nuke:
				if (--static_cast<shared_iterator<I>*>(src->big)->cnt == 0) {
					delete static_cast<shared_iterator<I>*>(src->big);
				}
				break;
			case op::bump:
				++static_cast<shared_iterator<I>*>(src->big)->it;
				break;
			case op::comp:
				if (__any_iterator::iter_equal(
					static_cast<const shared_iterator<I>*>(src->big)->it,
					static_cast<const shared_iterator<I>*>(dst->big)->it)) {
					[[fallthrough]];
			case op::rval:
					return +[](const blob& src) -> RValueReference {
						return iter_move(
							static_cast<const shared_iterator<I>*>(src.big)->it);
					};
				}
			}
			return nullptr;
		}

		template<class Reference, class ValueType, class RValueReference>
		struct cursor {
		private:
			blob data_{nullptr};
			Reference (*deref_)(const blob&) = &uninit_deref<Reference>;
			iter_move_fn<RValueReference> (*exec_)(op, blob*, blob*) =
				&uninit_noop<RValueReference>;

			void reset() noexcept {
				exec_(op::nuke, &data_, nullptr);
				deref_ = &uninit_deref<Reference>;
				exec_ = &uninit_noop<RValueReference>;
			}
			void copy_from(const cursor& that) {
				// Pre: *this is empty
				that.exec_(op::copy, const_cast<blob*>(&that.data_), &data_);
				deref_ = that.deref_;
				exec_ = that.exec_;
			}
			void move_from(cursor& that) {
				// Pre: *this is empty
				that.exec_(op::move, &that.data_, &data_);
				__stl2::swap(deref_, that.deref_);
				__stl2::swap(exec_, that.exec_);
			}
		public:
			using value_type = ValueType;
			using single_pass = std::true_type;

			struct mixin : basic_mixin<cursor> {
			private:
				using base_t = basic_mixin<cursor>;
			public:
				mixin() = default;
				template<class I>
				requires !derived_from<I, mixin> && input_iterator<I>
				explicit mixin(I i)
				: base_t(cursor{std::move(i)})
				{}
				using base_t::base_t;
#if defined(__GNUC__) && !defined(__clang__) && __GNUC__ < 8
				explicit mixin(cursor c)
				: base_t(std::move(c))
				{}
#endif // unknown gcc7 bug
			};

			cursor() = default;
			cursor(const cursor& that) {
				copy_from(that);
			}
			cursor(cursor&& that) {
				move_from(that);
			}
			template<class I>
			requires !derived_from<I, cursor> && input_iterator<I>
			cursor(I i) {
				if constexpr (is_small<I>) {
					::new (static_cast<void *>(&data_.tiny)) I(std::move(i));
					deref_ = &deref_small<Reference, I>;
					exec_ = &exec_small<RValueReference, I>;
				} else {
					data_.big = new shared_iterator<I>(std::move(i));
					deref_ = &deref_big<Reference, I>;
					exec_ = &exec_big<RValueReference, I>;
				}
			}

			~cursor() {
				exec_(op::nuke, &data_, nullptr);
			}

			cursor& operator=(const cursor& that) {
				if (&that != this) {
					reset();
					copy_from(that);
				}
				return *this;
			}
			cursor& operator=(cursor&& that) {
				if (&that != this) {
					reset();
					move_from(that);
				}
				return *this;
			}

			Reference read() const {
				return deref_(data_);
			}
			bool equal(const cursor& that) const {
				return exec_(op::comp, const_cast<blob*>(&data_),
					const_cast<blob*>(&that.data_)) != nullptr;
			}
			void next() {
				exec_(op::bump, &data_, nullptr);
			}
			RValueReference indirect_move() const {
				return exec_(op::rval, nullptr, nullptr)(data_);
			}
		};
	} // namespace any_iterator

	namespace ext {
		template<class Reference,
			class ValueType = __uncvref<Reference>,
			class RValueReference = __iter_move::rvalue<Reference>>
		using any_input_iterator =
			basic_iterator<__any_iterator::cursor<Reference, ValueType, RValueReference>>;
	} // namespace ext
} STL2_CLOSE_NAMESPACE

#endif
