#
#  Copyright Christopher Di Bella
#  Copyright Morris Hafner
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
