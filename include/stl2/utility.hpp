#ifndef STL2_UTILITY_HPP
#define STL2_UTILITY_HPP

#include <cassert>
#include <type_traits>
#include <utility>

#include <meta/meta.hpp>

#include <stl2/detail/fwd.hpp>
#include <stl2/detail/swap.hpp>
#include <stl2/detail/tagged.hpp>
#include <stl2/detail/concepts/core.hpp>
#include <stl2/detail/concepts/object.hpp>

namespace stl2 { inline namespace v1 {

using std::initializer_list;

namespace rel_ops = std::rel_ops;

using std::forward;
using std::move;
using std::move_if_noexcept;
using std::declval;

using std::pair;
using std::make_pair;
using std::piecewise_construct_t;
using std::tuple_size;
using std::tuple_element;
using std::get;

using std::integer_sequence;
using std::index_sequence;
using std::make_integer_sequence;
using std::make_index_sequence;
using std::index_sequence_for;
}} // namespace stl2::v1

#endif
