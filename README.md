# cmcstl2
An implementation of "C++ Extensions for Ranges" P0021 (was [N4382](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4382.pdf)). There are still quite a few rough edges, but the library is now feature-complete.

Compilation requires [GCC 6.1](https://gcc.gnu.org/) with the `-fconcepts` command line option. Compile times are currently on the slow side, even for C++. The implementation of Concepts in GCC is still brand-spanking-new and hasn't yet had proper performance tuning.
