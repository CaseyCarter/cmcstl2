CXX = ~/concept-gcc/bin/g++
CXXFLAGS = -std=gnu++1z
LDFLAGS = -Wl,-rpath,$(HOME)/concept-gcc/lib64
WARN = -Wall -Wextra
CPPFLAGS = $(WARN)

all: test

test: foo
	./foo

clean:
	$(RM) *~ *.o *.obj foo a.out
