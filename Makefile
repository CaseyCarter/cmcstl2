CXX = ~/concept-gcc/bin/g++
CXXFLAGS = -std=gnu++1z
LDFLAGS = -Wl,-rpath,$(HOME)/concept-gcc/lib64
WARN = -Wall -Wextra
CPPFLAGS = $(WARN)

all: foo

test: foo
	./foo
