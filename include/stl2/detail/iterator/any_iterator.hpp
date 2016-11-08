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

#include <new>
#include <atomic>
#include <stl2/type_traits.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/iterator/basic_iterator.hpp>
#include <stl2/detail/iterator/concepts.hpp>

STL2_OPEN_NAMESPACE {
	namespace __any_iterator {
        enum class op { copy, move, nuke, bump, comp, rval };

        struct counted {
            std::atomic<long> cnt{ 1 };
        };
        InputIterator{I} struct shared_iterator : counted {
            shared_iterator(I i) : it(std::move(i)) {}
            I it;
        };

        union blob {
            counted* big;
            std::aligned_storage_t<2 * sizeof(void *)> tiny;
        };

        template <class It>
        using is_small =
            std::integral_constant<bool, (sizeof(It) <= sizeof(blob::tiny))>;
        using small_tag = std::true_type;
        using big_tag = std::false_type;

        template <class RValueReference>
        using iter_move_fn = RValueReference (*)(blob const &);

        template <class RValueReference>
        inline iter_move_fn<RValueReference> uninit_noop(op, blob *, blob *) {
            return nullptr;
        }

        template <class Reference>
        [[noreturn]] inline Reference uninit_deref(blob const &) {
            std::terminate();
        }

        template <class Reference, InputIterator I>
        static Reference deref_small(blob const &src) {
            return **static_cast<I const *>(static_cast<void const *>(&src.tiny));
        }

        template <class Reference, InputIterator I>
        static Reference deref_big(blob const &src) {
            return *static_cast<shared_iterator<I> const *>(src.big)->it;
        }

        template <class RValueReference, InputIterator I>
        iter_move_fn<RValueReference> exec_small(op o, blob *src, blob *dst) {
            switch (o) {
            case op::copy:
                ::new (static_cast<void *>(&dst->tiny))
                    I(*static_cast<I const *>(static_cast<void *>(&src->tiny)));
                break;
            case op::move:
                ::new (static_cast<void *>(&dst->tiny))
                    I(std::move(*static_cast<I *>(static_cast<void *>(&src->tiny))));
                // fallthrough
            case op::nuke:
                static_cast<I *>(static_cast<void *>(&src->tiny))->~I();
                break;
            case op::bump:
                ++*static_cast<I *>(static_cast<void *>(&src->tiny));
                break;
            case op::comp:
                if (*static_cast<I const *>(static_cast<void *>(&src->tiny)) ==
                    *static_cast<I const *>(static_cast<void *>(&dst->tiny))) {
                    // fallthrough
            case op::rval:
                    return +[](blob const &src) -> RValueReference {
                        return iter_move(
                            *static_cast<I const *>(static_cast<void const *>(&src.tiny)));
                    };
                }
            }
            return nullptr;
        }

        template <class RValueReference, InputIterator I>
        iter_move_fn<RValueReference> exec_big(op o, blob *src, blob *dst) {
            switch (o) {
            case op::copy:
                ++(dst->big = src->big)->cnt;
               break;
            case op::move:
                dst->big = std::exchange(src->big, nullptr);
                break;
            case op::nuke:
                if (0 == --src->big->cnt)
                    delete static_cast<shared_iterator<I>*>(src->big);
                break;
            case op::bump:
                ++static_cast<shared_iterator<I> *>(src->big)->it;
                break;
            case op::comp:
                if (static_cast<shared_iterator<I> const *>(src->big)->it ==
                    static_cast<shared_iterator<I> const *>(dst->big)->it) {
                    // fallthrough
            case op::rval:
                    return +[](blob const &src) -> RValueReference {
                        return iter_move(
                            static_cast<shared_iterator<I> const *>(src.big)->it);
                    };
                }
            }
            return nullptr;
        }

        template<class Reference, class ValueType, class RValueReference>
        struct cursor {
        private:
            blob data_ = { nullptr };
            Reference (*deref_)(blob const &) =
                &uninit_deref<Reference>;
            iter_move_fn<RValueReference> (*exec_)(op, blob *, blob *) =
                &uninit_noop<RValueReference>;

            InputIterator{I} cursor(I i, small_tag) {
                ::new (static_cast<void *>(&data_.tiny)) I(std::move(i));
                deref_ = &deref_small<Reference, I>;
                exec_ = &exec_small<RValueReference, I>;
            }
            InputIterator{I} cursor(I i, big_tag) {
                data_.big = new shared_iterator<I>(std::move(i));
                deref_ = &deref_big<Reference, I>;
                exec_ = &exec_big<RValueReference, I>;
            }
        public:
            using value_type = ValueType;
            using single_pass = std::true_type;
            struct mixin;
            cursor() = default;
            cursor(cursor &&that) {
                that.exec_(op::move, &that.data_, &data_);
                swap(deref_, that.deref_);
                swap(exec_, that.exec_);
            }
            cursor(cursor const &that) {
                that.exec_(op::copy, const_cast<blob *>(&that.data_), &data_);
                deref_ = that.deref_;
                exec_ = that.exec_;
            }
            cursor &operator=(cursor &&that) {
                if (&that != this) {
                    this->~cursor();
                    ::new (static_cast<void *>(this))
                        cursor(std::move(that));
                }
                return *this;
            }
            cursor &operator=(cursor const &that) {
                if (&that != this) {
                    this->~cursor();
                    ::new (static_cast<void *>(this)) cursor(that);
                }
                return *this;
            }
            ~cursor() {
                exec_(op::nuke, &data_, nullptr);
            }
            Reference read() const {
                return deref_(data_);
            }
            bool equal(cursor const &that) const {
                return exec_(op::comp, const_cast<blob *>(&data_),
                    const_cast<blob *>(&that.data_)) != nullptr;
            }
            void next() {
                exec_(op::bump, &data_, nullptr);
            }
            RValueReference indirect_move() const {
                return exec_(op::rval, nullptr, nullptr)(data_);
            }
        };

        template<class Reference, class ValueType, class RValueReference>
        struct cursor<Reference, ValueType, RValueReference>::mixin
        : basic_mixin<cursor> {
            mixin() = default;
            using mixin::basic_mixin::basic_mixin;
            InputIterator{I} mixin(I i)
            requires
                ConvertibleTo<reference_t<I>, Reference>() &&
                ConvertibleTo<rvalue_reference_t<I>, RValueReference>()
            : basic_mixin<cursor>(cursor{std::move(i), is_small<I>{}}) {
            }
            template <class I, class C = cursor,
                class = meta::if_c<!is_same<I, mixin>::value>>
            requires
                !Same<I, basic_iterator<C>>() && InputIterator<I>() &&
                ConvertibleTo<reference_t<I>, Reference>() &&
                ConvertibleTo<rvalue_reference_t<I>, RValueReference>()
            mixin &operator=(I i) & {
                this->get() = cursor{i, is_small<I>{}};
                return *this;
            }
        };
	}

    template<class Reference,
        class ValueType = __uncvref<Reference>,
        class RValueReference = __iter_move::rvalue<Reference>>
    using any_input_iterator =
        basic_iterator<__any_iterator::cursor<Reference, ValueType, RValueReference>>;

} STL2_CLOSE_NAMESPACE

#endif
