# The pre-processor options used by the cpp (man cpp for more).
CPPFLAGS  	+= -I../../include/ -I../../dep/avalon/include/ -I../../dep/zlib/include/ -g -Wall -MMD

# The options used in linking as well as in any direct use of ld.
LDFLAGS   	+= -L../../lib/linux/ -L../../dep/avalon/lib/linux/ -L../../dep/zlib/lib/linux/\
			   -lCommonLib -lAvalonNetwork -lAvalonFramework -lAvalonDatabase -lAvalonXml -lAvalonLogger -lAvalonThread -lAvalonCore -lAvalonCodec -lpthread \
			   -lmysqlclient -lpthread -lz 


# preheader file.
PREHEADER 	= 

PREHEADERGCH = 


# The target file. Choose the right one.
TARGETPROG 	= AdminServer 

TARGETLIB  	=

TARGETSO   	=


#source directory
SRCDIRS 	= ./

#target directory
TARGETDIR  	= ../../bin/linux/


## Implicit Section: change the following only when necessary.
##==========================================================================
-include ../../Common.mak

