# cmcstl2 - A concept-enabled C++ standard library
#
#  Copyright Eric Niebler 2018
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/caseycarter/cmcstl2
#

find_package(Threads REQUIRED)

include_directories("../external/google-benchmark/include")
file(GLOB GOOGLE_BENCHMARK_FILES "../external/google-benchmark/src/*.cc")

add_executable(itinerary itinerary.cpp ${GOOGLE_BENCHMARK_FILES})
target_link_libraries(itinerary stl2 Threads::Threads)
