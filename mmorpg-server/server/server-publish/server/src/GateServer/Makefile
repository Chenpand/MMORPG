# The pre-processor options used by the cpp (man cpp for more).
CPPFLAGS  	+= -I../../include/ -I../../dep/avalon/include/ -I../../dep/zlib/include/ -I../../dep/fmt/include/ -g -O1 -Wall -MMD

# The options used in linking as well as in any direct use of ld.
LDFLAGS   	+= -L../../lib/linux/ -L../../dep/avalon/lib/linux/ -L../../dep/zlib/lib/linux/ -L../../dep/fmt/lib/linux/ \
			   -lCommonLib -lAvalonNetwork -lAvalonCodec -lAvalonFramework -lAvalonXml -lAvalonGame -lAvalonLogger -lAvalonThread -lAvalonCore \
			   -lpthread -lz -lfmt


# preheader file.
PREHEADER 	= 

PREHEADERGCH = 


# The target file. Choose the right one.
TARGETPROG 	= GateServer 

TARGETLIB  	=

TARGETSO   	=


#source directory
SRCDIRS 	= ./

#target directory
TARGETDIR  	= ../../bin/linux/


## Implicit Section: change the following only when necessary.
##==========================================================================
-include ../../Common.mak

