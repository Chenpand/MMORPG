# The pre-processor options used by the cpp (man cpp for more).
CPPFLAGS  	+= -I../../include/ -I../../dep/avalon/include/ -I../../dep/libcurl/include -g -O1 -Wall -MMD

# The options used in linking as well as in any direct use of ld.
LDFLAGS   	+= -L../../lib/linux/ -L../../dep/avalon/lib/linux/ -L../../dep/libcurl/lib/linux/\
			   -lCommonLib -lAvalonNetwork -lAvalonFramework -lAvalonGame \
			   -lAvalonXml -lAvalonLogger -lAvalonThread -lAvalonDatabase -lAvalonCore -lAvalonCodec\
			   -lmysqlclient -lpthread -lz -lcurl


# preheader file.
PREHEADER 	= 

PREHEADERGCH = 


# The target file. Choose the right one.
TARGETPROG 	= CenterServer 

TARGETLIB  	=

TARGETSO   	=


#source directory
SRCDIRS 	= ./

#target directory
TARGETDIR  	= ../../bin/linux/


## Implicit Section: change the following only when necessary.
##==========================================================================
-include ../../Common.mak

