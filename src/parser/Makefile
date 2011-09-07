# 
# src/parser/Makefile
#

ROOT := ../..
include $(ROOT)/Common.mk

OBJECTS = ScribbleProtocolParser.o ScribbleProtocolLexer.o st_node.o parser.o stack.o
OBJS    = $(addprefix $(BUILD_DIR)/,$(OBJECTS))

all: $(OBJS)

# External parser
parser: main.c $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/parser \
	  $(OBJS) main.c $(LD_FLAGS)

$(BUILD_DIR)/%.o: %.c $(INCLUDE_DIR)/%.h
	$(CC) $(CFLAGS) -I../common -I. -o $(BUILD_DIR)/$*.o -c $*.c

antlr: ScribbleProtocol.g
	antlr3 ScribbleProtocol.g
	$(CP) ScribbleProtocolParser.h $(INCLUDE_DIR) && $(RM) ScribbleProtocolParser.h
	$(CP) ScribbleProtocolLexer.h  $(INCLUDE_DIR) && $(RM) ScribbleProtocolLexer.h
	$(RM) ScribbleProtocol.tokens

clean-parser:
	$(RM) ScribbleProtocolParser.[ch] ScribbleProtocolLexer.[ch] ScribbleProtocol.tokens

include $(ROOT)/Rules.mk
