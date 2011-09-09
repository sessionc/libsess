# 
# Makefile for building example applications
#

ROOT := ../..
include $(ROOT)/Common.mk

OBJS := iter_Alice iter_Bob iter_Charlie

all: $(OBJS)

%: %.c
	$(MKDIR) -p $(BIN_DIR)/multiparty_iter
	$(CC) $(CFLAGS) -o $(BIN_DIR)/multiparty_iter/$@ $*.c $(LD_FLAGS)
	cp *.spr $(BIN_DIR)/multiparty_iter

include $(ROOT)/Rules.mk
