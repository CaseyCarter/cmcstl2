#
#  Copyright Christopher Di Bella
#  Copyright Morris Hafner
#
# SPDX-License-Identifier: Apache-2.0
#
include(FindPackageHandleStandardArgs)
include(CheckCXXCompilerFlag)
include(CMakePushCheckState)

add_library(Sanitizer::all INTERFACE IMPORTED)
set(Sanitizer_FOUND ON)

foreach(san ${Sanitizer_FIND_COMPONENTS})
   if(TARGET Sanitizer::${san})
      continue()
   endif()

   cmake_push_check_state(RESET)
   list(APPEND CMAKE_REQUIRED_LIBRARIES -fsanitize=${san})
   check_cxx_compiler_flag(-fsanitize=${san} ${san}_sanitizer_supported)
   cmake_pop_check_state()

   if(${${san}_sanitizer_supported})
      add_library(Sanitizer::${san} INTERFACE IMPORTED)
      set_target_properties(Sanitizer::${san} PROPERTIES
                       INTERFACE_COMPILE_OPTIONS -fsanitize=${san}
                       INTERFACE_LINK_LIBRARIES -fsanitize=${san})
      set(Sanitizer_${san}_FOUND TRUE)
      set_property(TARGET Sanitizer::all APPEND PROPERTY
         INTERFACE_LINK_LIBRARIES Sanitizer::${san})
   endif()
endforeach()

find_package_handle_standard_args(Sanitizer
   REQUIRED_VARS Sanitizer_FOUND
HANDLE_COMPONENTS)
