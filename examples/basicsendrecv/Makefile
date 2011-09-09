# 
# Makefile
#

ROOT := ../..
include $(ROOT)/Common.mk

all: basicsendrecv_alice

%: %.c
	$(MKDIR) $(BIN_DIR)/examples/basicsendrecv
	$(CC) $(CFLAGS) -o $(BIN_DIR)/examples/basicsendrecv/$@ $*.c $(LD_FLAGS)
	$(CP) *.spr $(BIN_DIR)/examples/basicsendrecv

include $(ROOT)/Rules.mk
