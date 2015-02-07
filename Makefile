ELCC_DEBUG ?= 
ELCC_USE_TSCB ?= 
PREFIX ?= /usr/local

CXX = g++
CXXFLAGS = -Wall -Wextra -Werror
CPPFLAGS = -I. -Isrc -fPIC
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

ifeq ($(ELCC_USE_TSCB),1)
SRC += src/tscb_editor.cc
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

clean:
	rm -f $(TARGET) $(EXAMPLE) $(ALL_OBJ)

.PHONY: all clean
