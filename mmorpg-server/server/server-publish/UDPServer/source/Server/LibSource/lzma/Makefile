include ${SERVER_ROOT}/MakeFile_Public

CPPFLAGS= -g -Wall -I./ -I../../include/lzma

TARGET	= $(DEBUG_TARGET)

DEBUG_TARGET = $(DEBUG_TARGET_DIR)/liblzma.a
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/liblzma.a

OBJ_DIR	= ../.objs

DEBUG_TARGET_DIR = $(SERVER_ROOT)/Lib/Debug
RELEASE_TARGET_DIR = $(SERVER_ROOT)/Lib/Release

SRC = $(wildcard *.c)
OBJS = $(addprefix $(OBJ_DIR)/,$(subst .c,.o,$(SRC)))

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : %.c
	$(CXX) $(CPPFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)

