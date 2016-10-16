# cmcstl2 - A concept-enabled C++ standard library
#
#  Copyright Casey Carter 2015
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/caseycarter/cmcstl2
#
CPP=g++ -std=c++1z -fconcepts -I ../../include
CPPFLAGS=-Wall -Wextra -pedantic -Werror

all: uninitialized_copy \
     uninitialized_fill \
     uninitialized_move \
     uninitialized_default_construct \
     uninitialized_value_construct
	echo "All tests complete!"

uninitialized_copy: $(HEADERS) uninitialized_copy.cpp
	$(CPP) $(CPPFLAGS) -o uninitialized_copy uninitialized_copy.cpp
	./uninitialized_copy

uninitialized_fill: $(HEADERS) uninitialized_fill.cpp
	$(CPP) $(CPPFLAGS) -o uninitialized_fill uninitialized_fill.cpp
	./uninitialized_fill

uninitialized_move: $(HEADERS) uninitialized_move.cpp
	$(CPP) $(CPPFLAGS) -o uninitialized_move uninitialized_move.cpp
	./uninitialized_move

uninitialized_default_construct: $(HEADERS) uninitialized_default_construct.cpp
	$(CPP) $(CPPFLAGS) -o uninitialized_default_construct uninitialized_default_construct.cpp
	./uninitialized_default_construct

uninitialized_value_construct: $(HEADERS) uninitialized_value_construct.cpp
	$(CPP) $(CPPFLAGS) -o uninitialized_value_construct uninitialized_value_construct.cpp
	./uninitialized_value_construct