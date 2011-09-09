# 
# Makefile for building applications
#

ROOT := ../..
include $(ROOT)/Common.mk

OBJS = sendrecv

all: $(OBJS)

%: %.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $*.c $(LD_FLAGS)

include $(ROOT)/Rules.mk
