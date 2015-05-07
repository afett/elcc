ELCC_DEBUG ?= 
ELCC_TSCB_EXAMPLE ?=
PREFIX ?= /usr/local

CXX ?= g++
CXXFLAGS = -Wall -Wextra -Werror -fPIC
CPPFLAGS = -I. -Isrc
LDFLAGS = -L.

ifeq ($(ELCC_DEBUG),1)
ELCC_CXXFLAGS += -g -O0
else
ELCC_CXXFLAGS += -O2
endif

DEPS = libedit

SRC = \
	src/completion.cc   \
	src/editor.cc       \
	src/editor_impl.cc  \
	src/history_impl.cc \
	src/tokenizer.cc    \

EXAMPLE_SRC = \
	examples/line_edit.cc

ifeq ($(ELCC_TSCB_EXAMPLE),1)
EXAMPLE_SRC += examples/line_edit_tscb.cc
DEPS += libtscb
endif

CXXFLAGS += $(shell pkg-config --cflags $(DEPS))
LIBS = -Wl,--as-needed $(shell pkg-config --libs $(DEPS))

OBJ = $(SRC:%.cc=%.o)
EXAMPLE_OBJ = $(EXAMPLE_SRC:%.cc=%.o)
EXAMPLE = $(EXAMPLE_SRC:%.cc=%)
TARGET = libelcc.so

ALL_OBJ = $(OBJ) $(EXAMPLE_OBJ)

all: $(TARGET) $(EXAMPLE)

$(EXAMPLE): $(EXAMPLE_OBJ) $(TARGET)
	$(CXX) -o $@ $< $(LDFLAGS) $(LIBS) -lelcc

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS) -shared $(LIBS)

install: all
	install -d $(PREFIX)/lib
	install -m 755 $(TARGET) $(PREFIX)/lib/
	install -d $(PREFIX)/include/elcc
	install -m 644 elcc/*.h $(PREFIX)/include/elcc

clean:
	rm -f $(TARGET) $(EXAMPLE) $(ALL_OBJ)

.PHONY: all clean
