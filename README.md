# cmcstl2
An implementation of the Ranges TS "C++ Extensions for Ranges" as specified in working paper [N4620](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/n4620.pdf) with many many proposed resolutions implemented for issues as documented at http://github.com/ericniebler/stl2/issues. There are still quite a few rough edges, but the library is now feature-complete.

Compilation requires [GCC 6](https://gcc.gnu.org/) with the `-std=c++1z` and `-fconcepts` command line options. Compile times are currently on the slow side, even for C++.

**Build status**
- on Travis-CI: [![Travis Build Status](https://travis-ci.org/CaseyCarter/cmcstl2.svg?branch=master)](https://travis-ci.org/CaseyCarter/cmcstl2)
