#ifndef TEST_COPYMOVE_HPP
#define TEST_COPYMOVE_HPP

struct copyable {};

struct moveonly {
  moveonly() = default;
  moveonly(moveonly&&) = default;
  moveonly& operator=(moveonly&&) = default;
};

struct nonmovable {
  nonmovable() = default;
  nonmovable(nonmovable&&) = delete;
};

struct copyonly {
  copyonly() = default;
  copyonly(const copyonly&) = default;
  copyonly& operator=(const copyonly&) = default;
  copyonly(copyonly&&) = delete;
  copyonly& operator=(copyonly&&) = delete;
};

#endif // TEST_COPYMOVE_HPP
