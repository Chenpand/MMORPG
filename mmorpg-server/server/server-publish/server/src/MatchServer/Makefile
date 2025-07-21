# The pre-processor options used by the cpp (man cpp for more).
CPPFLAGS  	+= -I../../include/ -I../../dep/avalon/include/ -g -O1 -Wall -MMD -D_NEW_MATCH_ -D_ADJUEST_MATCH_ALGORITHM

# The options used in linking as well as in any direct use of ld.
LDFLAGS   	+= -L../../lib/linux/ -L../../dep/avalon/lib/linux/ \
			   -lCommonLib -lAvalonNetwork -lAvalonFramework -lAvalonGame \
			   -lAvalonXml -lAvalonLogger -lAvalonThread -lAvalonScript -lAvalonCore -lAvalonCodec \
			   -lpthread


# preheader file.
PREHEADER 	= 

PREHEADERGCH = 


# The target file. Choose the right one.
TARGETPROG 	= MatchServer 

TARGETLIB  	=

TARGETSO   	=


#source directory
SRCDIRS 	= ./

#target directory
TARGETDIR  	= ../../bin/linux/


## Implicit Section: change the following only when necessary.
##==========================================================================
-include ../../Common.mak

