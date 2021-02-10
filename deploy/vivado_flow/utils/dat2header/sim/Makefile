# #############################################################################
# Utilities
# #############################################################################

# Define V=1 for a more verbose compilation
ifndef V
	QUIET_AR            = @echo '   ' AR $@;
	QUIET_BUILD         = @echo '   ' BUILD $@;
	QUIET_C             = @echo '   ' CC $@;
	QUIET_CXX           = @echo '   ' CXX $@;
	QUIET_CHECKPATCH    = @echo '   ' CHECKPATCH $(subst .o,.cc,$@);
	QUIET_CHECK         = @echo '   ' CHECK $(subst .o,.cc,$@);
	QUIET_LINK          = @echo '   ' LINK $@;
	QUIET_CP            = @echo '   ' CP $@;
	QUIET_MKDIR         = @echo '   ' MKDIR $@;
	QUIET_MAKE          = @echo '   ' MAKE $@;
	QUIET_INFO          = @echo -n '   ' INFO '';
	QUIET_RUN           = @echo '   ' RUN '';
	QUIET_CLEAN         = @echo '   ' CLEAN ${PWD};
endif

DATA_DAT ?= ../test/data.dat
DATA_H ?= data.h
ARRAY ?= data
MAX ?= 2

# Define the main target
EXE = dat2header

all: debug

# We'll use gcc for C compilation.

CXX          = g++
#TARGET_ARCH = linux64

# Let's leave a place holder for additional include directories

INCDIR :=
INCDIR += -I../inc
#INCDIR += -I../inc/ac_types
INCDIR += -I../inc/ap_types

# Compilation options:
# -O0 -g for debugging info and -Wall enables all warnings

CXX_FLAGS :=
CXX_FLAGS += -Wall
CXX_FLAGS += -Wno-int-in-bool-context
CXX_FLAGS += -std=c++11

release: CXX_FLAGS += -O3
release: $(EXE)

debug: CXX_FLAGS += -DDEBUG
debug: CXX_FLAGS += -O0
debug: CXX_FLAGS += -g
debug: $(EXE)

# Linking options:

LD_FLAGS :=

# List the libraries you need to link with in LD_LIBS
# For example, use "-lm" for the math library

LD_LIBS :=

# The VPATH is a list of directories to be searched for missing source and
# headers files.

VPATH :=
VPATH += ../tb
VPATH += ../inc
VPATH += ../src

# List of the source and header files. Note that they will be searched first in
# the current directory and then in the directories specified in the VPATH
# variable.

CXX_SOURCES :=
CXX_SOURCES += main.cc

CXX_HEADERS :=

.SUFFIXES: .cc .h .o

CXX_OBJECTS := $(CXX_SOURCES:.cc=.o)

$(CXX_OBJECTS): $(CXX_HEADERS)

$(EXE): $(CXX_OBJECTS)
	$(QUIET_LINK)$(CXX) -o $@ $(CXX_OBJECTS) ${LD_LIBS} ${LD_FLAGS}

.cc.o:
	$(QUIET_CXX)$(CXX) $(CXX_FLAGS) ${INCDIR} -c $<

# Always provide the "clean" target that removes intermediate files. What you
# remove depend on your choice of coding tools (different editors generate
# different backup files for example).
#
# And the "clean" target is not a file name, so we tell make that it a "phony"
# target.

run: debug
	$(QUIET_RUN)./$(EXE) $(DATA_DAT) $(DATA_H) $(ARRAY) $(MAX)
.PHONY: run

run-release: release
	$(QUIET_RUN)./$(EXE) $(DATA_DAT) $(DATA_H) $(ARRAY) $(MAX)
.PHONY: run-release

valgrind: debug
	$(QUIET_RUN)valgrind --leak-check=full --max-stackframe=16777216 --show-leak-kinds=all --track-origins=yes ./$(EXE) $(DATA_DAT) $(DATA_H) $(ARRAY) $(MAX)
.PHONY: valgrind

gdb:
	$(QUIET_RUN)gdb ./$(EXE)
.PHONY: gdb

clean:
	$(QUIET_CLEAN)rm -f *.o $(EXE) *.bin *.log *.out
.PHONY: clean
