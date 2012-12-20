
ELCC_DEBUG    = 1

ELCC_CPPFLAGS =
ELCC_CXXFLAGS = -Wall -Werror
ELCC_LDFLAGS  =

# FIXME must be on to build example for the moment
ELCC_USE_TSCB = 1

ifeq ($(ELCC_DEBUG),1)
ELCC_CXXFLAGS += -g -O0
endif

export ELCC_CPPFLAGS ELCC_CXXFLAGS ELCC_LDFLAGS ELCC_USE_TSCB

all: src examples

lib:
	make -C src

examples: lib
	make -C examples

clean:
	make -C src clean
	make -C examples clean

.PHONY: all clean

