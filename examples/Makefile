CPPFLAGS = $(ELCC_CPPFLAGS) -I.. -I. 
CXXFLAGS = $(ELCC_CXXFLAGS)
LDFLAGS  = $(ELCC_LDFLAGS) -L ../src -lelcc

SRC = \
	line_edit.cc

ifeq ($(ELCC_USE_TSCB),1)
SRC += line_edit_tscb.cc
endif

OBJ=$(SRC:%.cc=%.o)
TARGETS=$(OBJ:%.o=%)

all: $(TARGETS)

$(TARGETS): $(OBJ)
	$(CXX) $(LDFLAGS) $@.o -o $@

clean:
	rm -rf *.o $(TARGETS)

.PHONY: all clean


