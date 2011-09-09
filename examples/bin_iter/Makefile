# 
# Makefile for building applications
#

ROOT := ../..
include $(ROOT)/Common.mk

OBJS = iter_Bob iter_Alice

all: $(OBJS)

%: %.c
	$(MKDIR) $(BIN_DIR)/bin_iter
	$(CC) $(CFLAGS) -o $(BIN_DIR)/bin_iter/$@ $*.c $(LD_FLAGS)
	cp *.spr $(BIN_DIR)/bin_iter

include $(ROOT)/Rules.mk
