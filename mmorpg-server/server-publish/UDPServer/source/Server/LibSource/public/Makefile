include ${SERVER_ROOT}/MakeFile_Public

CPPFLAGS= -g -Wall $(PUBLIC_INC) -fPIC

TARGET	= $(DEBUG_TARGET)

DEBUG_TARGET = $(DEBUG_TARGET_DIR)/libpublic.a
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/libpublic.a

OBJ_DIR	= ../.objs

DEBUG_TARGET_DIR = $(SERVER_ROOT)/Lib/Debug
RELEASE_TARGET_DIR = $(SERVER_ROOT)/Lib/Release

all : $(TARGET)

$(TARGET) : $(OBJS)
	@if [ ! -e $(DEBUG_TARGET_DIR) ]; then \
		mkdir -p $(DEBUG_TARGET_DIR); \
	fi
	@if [ ! -e $(RELEASE_TARGET_DIR) ]; then \
		mkdir -p $(RELEASE_TARGET_DIR); \
	fi
	$(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : %.cpp
	@if [ ! -e $(OBJ_DIR) ]; then \
		mkdir $(OBJ_DIR); \
	fi
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean: 
	rm -f $(OBJS) $(TARGET)