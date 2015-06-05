#ifndef TEST_COPYMOVE_HPP
#define TEST_COPYMOVE_HPP

struct copyable_t {};

struct moveonly_t {
  moveonly_t() = default;
  moveonly_t(moveonly_t&&) = default;
  moveonly_t& operator=(moveonly_t&&) = default;
};

struct nonmovable_t {
  nonmovable_t() = default;
  nonmovable_t(nonmovable_t&&) = delete;
};

struct copyonly_t {
  copyonly_t() = default;
  copyonly_t(const copyonly_t&) = default;
  copyonly_t& operator=(const copyonly_t&) = default;
  copyonly_t(copyonly_t&&) = delete;
  copyonly_t& operator=(copyonly_t&&) = delete;
};

#endif // TEST_COPYMOVE_HPP
