# cmcstl2
An implementation of the Ranges TS "C++ Extensions for Ranges" [N4560](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4560.pdf) with extensions for proxy iterators from [P0022: Proxy Iterators for the Ranges Extensions](http://wg21.link/p0022) and updates from [P0370: Ranges TS Design Updates Omnibus](http://wg21.link/p0370). There are still quite a few rough edges, but the library is now feature-complete.

Compilation requires [GCC 6.1](https://gcc.gnu.org/) with the `-fconcepts` command line option. Compile times are currently on the slow side, even for C++. The implementation of Concepts in GCC is brand-spanking-new and hasn't yet had proper performance tuning.

**Build status**
- on Travis-CI: [![Travis Build Status](https://travis-ci.org/CaseyCarter/cmcstl2.svg?branch=master)](https://travis-ci.org/CaseyCarter/cmcstl2)
