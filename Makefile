
ELCC_DEBUG    = 1

ELCC_CPPFLAGS =
ELCC_CXXFLAGS = -Wall -Wextra -Werror
ELCC_LDFLAGS  =

ELCC_USE_TSCB = 1

ifeq ($(ELCC_DEBUG),1)
ELCC_CXXFLAGS += -g -O0
else
ELCC_CXXFLAGS += -O2
endif

export ELCC_CPPFLAGS ELCC_CXXFLAGS ELCC_LDFLAGS ELCC_USE_TSCB

all: src examples

lib:
	$(MAKE) -C src

examples: lib
	$(MAKE) -C examples

clean:
	$(MAKE) -C src clean
	$(MAKE) -C examples clean

.PHONY: all clean

