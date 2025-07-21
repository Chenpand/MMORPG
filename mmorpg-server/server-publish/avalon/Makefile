# Project directory
SUB_DIRS = src/AvalonCodec src/AvalonCore src/AvalonThread src/AvalonLogger src/AvalonDatabase src/AvalonXml \
		   src/AvalonScript src/AvalonFramework src/AvalonGame src/AvalonNetwork \
		   src/testcore src/testnetwork src/testclient

.PHONY: all objs clean help

# Delete the default suffixes
.SUFFIXES:

all:
	@$(foreach N, $(SUB_DIRS), make -C $(N);)

objs:
	@$(foreach N, $(SUB_DIRS), make -C $(N) objs;)

clean:
	@$(foreach N, $(SUB_DIRS), make -C $(N) clean;)
install:
	cp lib/linux/libAvalon* ../server/lib/linux/

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
