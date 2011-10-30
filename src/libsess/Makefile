# 
# Makefile for building libsess
#

ROOT := ../..
include $(ROOT)/Common.mk

OBJS = $(addprefix $(BUILD_DIR)/,st_node.o parser.o stack.o ScribbleProtocolParser.o ScribbleProtocolLexer.o libsess.o connmgr.o)

all: libsess

libsess:%: $(BUILD_DIR)/%.a
	$(CP) $(BUILD_DIR)/$@.a $(LIB_DIR)/$@.a

$(BUILD_DIR)/%.a: $(OBJS)
	$(RM) $(BUILD_DIR)/$*.a
	$(AR) $(ARFLAGS) $@ $(OBJS)

#
# --- Rules for building the objects ---
#

$(BUILD_DIR)/st_node.o:
	$(MAKE) --directory=$(SRC_DIR)/common

$(BUILD_DIR)/stack.o:
	$(MAKE) --directory=$(SRC_DIR)/common

$(BUILD_DIR)/parser.o:
	$(MAKE) --directory=$(SRC_DIR)/parser

$(BUILD_DIR)/ScribbleProtocolParser.o:
	$(MAKE) --directory=$(SRC_DIR)/parser

$(BUILD_DIR)/ScribbleProtocolLexer.o:
	$(MAKE) --directory=$(SRC_DIR)/parser

$(BUILD_DIR)/connmgr.o:
	$(MAKE) --directory=$(SRC_DIR)/connmgr

$(BUILD_DIR)/libsess.o: libsess.c
	$(CC) $(CFLAGS) \
	  -c libsess.c \
	  -o $(BUILD_DIR)/libsess.o


include $(ROOT)/Rules.mk
