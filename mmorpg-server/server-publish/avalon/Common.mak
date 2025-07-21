## Implicit Section: change the following only when necessary.
##==========================================================================

# The target file name.
TARGETFILE  =

ifdef TARGETPROG
	TARGETFILE = $(TARGETDIR)$(TARGETPROG)
endif

ifdef TARGETLIB
	TARGETFILE = $(TARGETDIR)$(TARGETLIB)
endif

ifdef TARGETSO
	TARGETFILE = $(TARGETDIR)$(TARGETSO)
endif


# The source file types (headers excluded).
# .c indicates C source files, and others C++ ones.
SRCEXTS = .cpp .CPP .c

# The header file types.
HDREXTS = .h .hpp 

# The pre-processor and compiler options.
# Users can override those variables from the command line.
SOCXXFLAGS = -fpic -shared


# The C++ program compiler.
CXX    = g++

# The command used to delete file.
RM     = rm -f

# the commnd used to make lib
AR     = ar r


## Stable Section: usually no need to be changed. But you can add more.
##==========================================================================
SHELL   = /bin/sh
EMPTY   =
SPACE   = $(EMPTY) $(EMPTY)

SOURCES = $(foreach d, $(SRCDIRS), $(wildcard $(addprefix $(d)*,$(SRCEXTS))))
HEADERS = $(foreach d, $(SRCDIRS), $(wildcard $(addprefix $(d)*,$(HDREXTS))))

OBJS    = $(foreach s, $(SOURCES), $(dir $(s)).$(basename $(notdir $(s))).o)
DEPS    = $(addsuffix .d, $(basename $(OBJS)))
MISSING_DEPS = $(filter-out $(wildcard $(DEPS)),$(DEPS))

COMPILE.cxx = $(CXX) $(CPPFLAGS) -std=c++11 -c


.PHONY: all objs clean help

# Delete the default suffixes
.SUFFIXES:

all: $(TARGETFILE)

-include $(DEPS)

# Rules for generating object files (.o).
#----------------------------------------
objs:$(OBJS)

.%.o:%.cpp $(PREHEADERGCH)
	$(COMPILE.cxx) $< $(PREHEADERGCH) -o $@

.%.o:%.CPP $(PREHEADERGCH)
	$(COMPILE.cxx) $< $(PREHEADERGCH) -o $@

.%.o:%.c $(PREHEADERGCH)
	$(COMPILE.cxx) $< $(PREHEADERGCH) -o $@

# Rules for precompiled headers
# -------------------------------------
ifdef PREHEADER
$(PREHEADERGCH):$(PREHEADER)
	$(CXX) $(CPPFLAGS) -x c++-header $^ -o $@
endif


ifdef MISSING_DEPS
$(MISSING_DEPS) :
	@$(RM) $(patsubst %.d,%.o,$@)
endif

# Rules for generating the executable.
#-------------------------------------
$(TARGETFILE):$(OBJS)
	@mkdir -p $(TARGETDIR)
ifdef TARGETPROG
	$(CXX) $^ -o $@ $(LDFLAGS)
endif
ifdef TARGETSO
	$(CXX) $(SOCXXFLAGS) $^ -o $@
endif
ifdef TARGETLIB
	$(AR) $@ $^
	ranlib -t $@
endif

clean:
	$(RM) $(OBJS) $(DEPS) $(TARGETFILE) 

# Show help.
help:
	@echo 'Generic Makefile for C/C++ Programs (gcmakefile) version 0.5'
	@echo
	@echo 'Usage: make [TARGET]'
	@echo 'TARGETS:'
	@echo '  all       (=make) compile and link.'
	@echo '  objs      compile only (no linking).'
	@echo '  clean     clean objects and the executable file.'
	@echo '  help      print this message.'
	@echo
## End of the Makefile ##  Suggestions are welcome  ## All rights reserved ##
#############################################################################
