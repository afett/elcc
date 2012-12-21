TARGET=libelcc.so

CPPFLAGS = $(ELCC_CPPFLAGS) -I.. -I.
CXXFLAGS = $(ELCC_CXXFLAGS) -fPIC
LDFLAGS  = $(ELCC_LDFLAGS) -shared -Wl,--as-needed

SRC=\
	completion.cc   \
	editor.cc       \
	editor_impl.cc  \
	history_impl.cc \

DEPS = libedit

ifeq ($(ELCC_USE_TSCB),1)
SRC += tscb_editor.cc
DEPS += libtscb
endif

CXXFLAGS += $(shell pkg-config --cflags $(DEPS))
LIBS = $(shell pkg-config --libs $(DEPS))

OBJ=$(SRC:%.cc=%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	rm -rf *.o $(TARGET)

.PHONY: all clean


