include ${SERVER_ROOT}/MakeFile_Public

CPPFLAGS= -g -Wall $(PUBLIC_INC) -fPIC

TARGET	= $(DEBUG_TARGET)

DEBUG_TARGET = $(DEBUG_TARGET_DIR)/libconnector.a
RELEASE_TARGET = $(RELEASE_TARGET_DIR)/libconnector.a

CONNECTOR_DIR = $(SERVER_ROOT)/Connector
CONNECTOR_INCLUDE = $(CONNECTOR_DIR)/include

OBJ_DIR	= ../.objs

DEBUG_TARGET_DIR = $(SERVER_ROOT)/Lib/Debug
RELEASE_TARGET_DIR = $(SERVER_ROOT)/Lib/Release

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $?

$(OBJ_DIR)/%.o : %.cpp
	@if [ ! -e $(OBJ_DIR) ]; then \
		mkdir $(OBJ_DIR); \
	fi
	$(CXX) $(CPPFLAGS) -I$(CONNECTOR_INCLUDE) -c $< -o $@

clean: 
	rm -f $(OBJS) $(TARGET)