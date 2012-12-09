TARGET=elcc_test

CXXFLAGS=-g -O0 -Wall -Werror
CPPFLAGS=-D_GNU_SOURCE -I include/ -I libedit-20120601-3.0/src/ -I./libtscb/include -I./boost.atomic -I.
LDFLAGS=-L libedit-20120601-3.0/src/.libs -L./libtscb -ledit -ltscb 

SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

clean:
	rm -rf  $(TARGET) $(OBJ)

.PHONY: all clean


