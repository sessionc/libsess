# 
# src/connmgr/Makefile
#

ROOT := ../..
include $(ROOT)/Common.mk

OBJECTS = parser.o stack.o connmgr.o
OBJS    = $(addprefix $(BUILD_DIR)/,$(OBJECTS))

all: $(OBJS)

connmgr: main.c $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/connmgr \
	  $(OBJS) main.c $(LD_FLAGS) $(DEBUG)

$(BUILD_DIR)/%.o: %.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -I../common -I. -o $(BUILD_DIR)/$*.o -c $*.c

include $(ROOT)/Rules.mk
