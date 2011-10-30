# 
# src/connmgr/Makefile
#

ROOT := ../..
include $(ROOT)/Common.mk

OBJECTS = parser.o stack.o connmgr.o
OBJS    = $(addprefix $(BUILD_DIR)/,$(OBJECTS))

all: $(OBJS)

connmgr: connmgr.c $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/connmgr \
	  $(OBJS) connmgr.c $(LD_FLAGS)

$(BUILD_DIR)/%.o: %.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -I../common -I. -o $(BUILD_DIR)/$*.o -c $*.c

include $(ROOT)/Rules.mk
