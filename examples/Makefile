TARGET=line_edit

CPPFLAGS = $(ELCC_CPPFLAGS) -I.. -I. 
CXXFLAGS = $(ELCC_CXXFLAGS)
LDFLAGS  = $(ELCC_LDFLAGS) -L ../src -lelcc

SRC=

ifeq ($(ELCC_USE_TSCB),1)
SRC += line_edit.cc
endif

OBJ=$(SRC:%.cc=%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

clean:
	rm -rf *.o $(TARGET)

.PHONY: all clean


