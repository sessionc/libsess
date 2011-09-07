# 
# src/common/Makefile
#

ROOT := ../..
include $(ROOT)/Common.mk

all: $(BUILD_DIR)/st_node.o $(BUILD_DIR)/stack.o

$(BUILD_DIR)/%.o: %.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -c $*.c \
	  -o $(BUILD_DIR)/$*.o

example: $(BUILD_DIR)/st_node.o $(BUILD_DIR)/stack.o example.c
	$(CC) $(CFLAGS) example.c \
	  $(BUILD_DIR)/st_node.o \
	  $(BUILD_DIR)/stack.o \
	  -o $(BIN_DIR)/st_node

include $(ROOT)/Rules.mk
