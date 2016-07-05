# cmcstl2
An implementation of the Ranges TS "C++ Extensions for Ranges" [N4560](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4560.pdf) with extensions for proxy iterators from [P0022: Proxy Iterators for the Ranges Extensions](http://wg21.link/p0022) and updates from [P0370: Ranges TS Design Updates Omnibus](http://wg21.link/p0370). There are still quite a few rough edges, but the library is now feature-complete.

Compilation requires [GCC 6.1](https://gcc.gnu.org/) with the `-fconcepts` command line option. Compile times are currently on the slow side, even for C++. The implementation of Concepts in GCC is brand-spanking-new and hasn't yet had proper performance tuning.

NOTE WELL: This library achieves reasonable performance from concepts by using constexpr variable templates as concept predicates. This was a great idea, until bugs introduced late in GCC 6 development caused the compiler to puke when it sees ODR uses of partially specialized variable templates. Consequently, programs using the library concepts will fail to link if compiled without optimizations. (The relevant bugs have been fixed on GCC trunk, so non-optimizing builds succeed with tip-of-trunk GCC.)
