ROOT := ../..
include $(ROOT)/Common.mk

all: alice bob

alice: alice.c
	$(CC) $(CFLAGS) alice.c -o $(BIN_DIR)/alice $(LD_FLAGS)
	cp Workflow_Alice.spr $(BIN_DIR)

bob: bob.c
	$(CC) $(CFLAGS) bob.c -o $(BIN_DIR)/bob $(LD_FLAGS)
	cp Workflow_Bob.spr $(BIN_DIR)

include $(ROOT)/Rules.mk
