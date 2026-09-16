CLASSICUBE := $(CLASSICUBE_DIR)/ClassiCube
CC_SYMBOLS := $(DYNAMIC_DIR)/cc_symbols.txt
DYN_HEADER := $(DYNAMIC_DIR)/dynamic.h

DYN_PLAT_DIR := $(DYNAMIC_DIR)/$(PLAT)
OBJ_SYMBOLS := $(DYN_PLAT_DIR)/obj_symbols.txt
DYN_SOURCE := $(DYN_PLAT_DIR)/dynamic.c
DYN_OBJECT := $(patsubst %.c,%.o,$(DYN_SOURCE))

MISCDYN_DIR := $(MISC_DIR)/dynamic
DYN_LOADER := $(MISCDYN_DIR)/loader.c
DYNGEN_H := $(MISCDYN_DIR)/dyngen_h.lua
DYNGEN_C := $(MISCDYN_DIR)/dyngen_c.lua

CFLAGS += -I$(DYNAMIC_DIR) -I$(DYN_PLAT_DIR) -DHAS_DYNAMIC_H

build: $(DYN_PLAT_DIR) $(DYN_HEADER) $(DYN_OBJECT) $(TARGET)

$(TARGET): $(DYN_OBJECT)

$(DYN_PLAT_DIR):
	$(MKDIR) $(DYN_PLAT_DIR)

$(DYN_HEADER): $(CC_SYMBOLS)
	$(LUA) $(DYNGEN_H) $(CC_SYMBOLS) > $(DYN_HEADER)

$(CC_SYMBOLS): $(CLASSICUBE)
	$(NM) -jgDU $(CLASSICUBE) > $(CC_SYMBOLS)

$(CLASSICUBE):
	$(MAKE) -C $(CLASSICUBE_DIR) linux

$(DYN_OBJECT): $(DYN_SOURCE) $(DYN_LOADER)
	$(CC) -c $(DYN_LOADER) -o $@ -I$(DYN_PLAT_DIR) $(CCFLAGS) $(CFLAGS)

$(DYN_SOURCE): $(OBJ_SYMBOLS)
	$(LUA) $(DYNGEN_C) $(CC_SYMBOLS) $(OBJ_SYMBOLS) > $(DYN_SOURCE)

$(OBJ_SYMBOLS): $(OBJECTS)
	$(NM) -jgu $^ > $(OBJ_SYMBOLS)

cleandyn:
	$(RMDIR) $(DYNAMIC_DIR)

cleandynobj:
	$(RMDIR) $(DYN_PLAT_DIR)

clean: cleandyn
cleanobj: cleandynobj

.PHONY: cleandyn cleandynobj

