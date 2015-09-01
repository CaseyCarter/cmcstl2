#include <stl2/iterator.hpp>
#include <algorithm>
#include "../simple_test.hpp"

// Believe it or not, this generates reasonable code:
// _Z11strlen_testPKc:
// .LFB9467:
// 	.cfi_startproc
// 	cmpb	$0, (%rdi)
// 	movq	%rdi, %rax
// 	je	.L4
// 	.p2align 4,,10
// 	.p2align 3
// .L5:
// 	addq	$1, %rax
// 	cmpb	$0, (%rax)
// 	jne	.L5
// .L4:
// 	subq	%rdi, %rax
// 	ret
// 	.cfi_endproc

int strlen_test(const char* p) noexcept {
  using C = stl2::CommonType<const char*, stl2::unreachable>;
  return stl2::distance(C{p}, std::find(C{p}, C{stl2::unreachable{}}, '\0'));
}

int main() {
  // Not yet: static_assert(strlen_test("Hello, world!") == 13);
  CHECK(strlen_test("123This is a test, this is only a test.456") == 42);
  return ::test_result();
}
