# CORSS_COMPILE ?=mips-linux-
# CROSS_TARGET  ?=mips-linux-

# CC=$(CROSS_COMPILE)gcc
# CXX=$(CROSS_COMPILE)g++
# LD=$(CROSS_COMPILE)ld
# AR=$(CROSS_COMPILE)ar
# RANLIB=$(CROSS_COMPILE)ranlib
# STRIP=$(CROSS_COMPILE)strip

CC		?=gcc
CXX		?=g++
LD		?=COMPILER_DEFINES
AR		?=ar
STD		?= gnu99
SHELL	?= /bin/sh

# TARGET_CONFIG = CC=$(CROSS_COMPILE)gcc \
# 	CXX=$(CROSS_COMPILE)g++ \
# 	LD=$(CROSS_COMPILE)gcc \
# 	AR=$(CROSS_COMPILE)ar \
# 	RANLIB=$(CROSS_COMPILE)ranlib

APP_DIRECTORY=.
INCLUDES= \
    -I. \
	-I./logutil	\
	-I./util \
	-I./mq_interface \
	-I./upnp_pf_interface \
	-I./llist \
	\

DEFINES = \
   $(COMPILER_DEFINES) \
#    -DDISABLE_LOG
  
COMPILER_FLAGS ?= \
   -Wall \
   -ggdb \
   -Os   \
   -O0   \
   -std=$(STD) \
   -funroll-loops

APPLICATION_FILES = \
	$(APP_DIRECTORY)/routerupnp.c 	\
	$(APP_DIRECTORY)/mq_interface/mq_posix_interface.c	\
	$(APP_DIRECTORY)/upnp_pf_interface/upnp_pf_interface.c \
	$(APP_DIRECTORY)/upnp_pf_interface/upnp_pf_errcode.c \
	$(APP_DIRECTORY)/util/util.c \
	$(APP_DIRECTORY)/util/netutil/netutil.c \
	$(APP_DIRECTORY)/llist/list.c

# -MMD and -MF generates Makefile dependencies while at the same time compiling.
# -MP notes to add a dummy 'build' rule for each header file.  This 
# prevent a problem where a removed header file will generate an error because a
# dependency references it but it can't be found anymore.
DEPENDENCY_FLAGS ?= -MMD -MP -MF $(@D)/$*.d 

CFLAGS += $(INCLUDES) $(DEFINES) $(COMPILER_FLAGS) $(DEPENDENCY_FLAGS)
LDFLAGS += 
LDLIBS += -lminiupnpc -lrt -lpthread

DOC_DIR=$(APP_DIRECTORY)/doc
OUTPUT_DIR=$(APP_DIRECTORY)/build
BIN_DIR=$(OUTPUT_DIR)/bin
# Build a list of object files from the source file list, but all objects
# live in the $(OUTPUT_DIR) above.  The list of object files
# created assumes that the file part of the filepath is unique
# (i.e. the bar.c of foo/bar.c is unique across all sub-directories included).
SOURCES_OBJECTS = $(shell echo $(APPLICATION_FILES) | xargs -n1 echo | sed -e 's^.*/\(.*\)\.c^$(OUTPUT_DIR)/\1\.o^')
TARGET_FILE = $(BIN_DIR)/routerupnp

.PHONY: all
all: directories $(TARGET_FILE)

ifneq ($(MAKECMDGOALS),clean)
-include $(SOURCES_OBJECTS:.o=.d)
endif

$(OUTPUT_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(OUTPUT_DIR)/$(@F)

$(TARGET_FILE): $(SOURCES_OBJECTS)
	$(CC) $^ $(LDFLAGS) $(LDLIBS) -o $(TARGET_FILE)
	@echo -e '\n$@ Build success'

.PHONY: clean
clean:
	-rm -rf $(OUTPUT_DIR)
	-rm -rf $(DOC_DIR)

.PHONY: directories
directories:
	mkdir -p $(BIN_DIR)

.PHONY: doc
doc:
	mkdir -p $(DOC_DIR)
	doxygen Doxyfile

# To facilitate generating all output files in a single output directory, we
# must create separate .o and .d rules for all the different sub-directories
# used by the source files.
# If additional directories are added that are not already in the
# $(APPLICATION_FILES) above, new rules will have to be created below.

# Object File rules

VPATH += $(dir $(APPLICATION_FILES))