##===- examples/SessionTypeChecker/Makefile ----------------*- Makefile -*-===##
# 
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
# 
##===----------------------------------------------------------------------===##

SESSCC_PATH      := ..
SESSCC_INC_DIR   := $(SESSCC_PATH)/include
SESSCC_BUILD_DIR := $(SESSCC_PATH)/build

CLANG_LEVEL := ../..
LIBRARYNAME = SessionTypeChecker 

# If we don't need RTTI or EH, there's no reason to export anything
# from the plugin.
ifneq ($(REQUIRES_RTTI), 1)
ifneq ($(REQUIRES_EH), 1)
#EXPORTED_SYMBOL_FILE = $(PROJ_SRC_DIR)/SessionTypeChecker.exports
endif
endif

LINK_LIBS_IN_SHARED = 0
SHARED_LIBRARY = 1

include $(CLANG_LEVEL)/Makefile

CXX += -I$(SESSCC_INC_DIR)
CXXFLAGS += $(SESSCC_BUILD_DIR)/st_node.o $(SESSCC_BUILD_DIR)/parser.o $(SESSCC_BUILD_DIR)/ScribbleProtocolParser.o $(SESSCC_BUILD_DIR)/ScribbleProtocolLexer.o $(SESSCC_BUILD_DIR)/stack.o
LDFLAGS += -lantlr3c


ifeq ($(OS),Darwin)
  LDFLAGS=-Wl,-undefined,dynamic_lookup
endif
