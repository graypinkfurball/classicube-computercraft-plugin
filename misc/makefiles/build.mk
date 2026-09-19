ifneq (,$(filter %32,$(PLAT)))
  BASEPLAT := $(patsubst %32,%,$(PLAT))
  PLATBIT := 32
endif

ifneq (,$(filter %64,$(PLAT)))
  BASEPLAT := $(patsubst %64,%,$(PLAT))
  PLATBIT := 64
endif

OBJECT_DIR := $(OBJECT_DIR)/$(PLAT)

ifeq (linux,$(BASEPLAT))
  EXT := so
endif

ifeq (windows,$(BASEPLAT))
  EXT := dll
endif

ifeq (macos,$(BASEPLAT))
  EXT := dylib
endif

TARGET := $(NAME)_$(BASEPLAT)_$(PLATBIT).$(EXT)

SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCES))

CCFLAGS := -pipe -fPIC
CFLAGS := -I$(CLASSICUBE_DIR) -Wall -fvisibility=hidden -ffreestanding -fno-ident -O3
LDFLAGS := -nostdlib -s -shared

ifeq (linux32,$(PLAT))
  CCFLAGS += -m32
endif

ifeq (windows,$(BASEPLAT))
  LDFLAGS += -Wl,--entry=0 -lkernel32
  ifeq (32,$(PLATBIT))
    CC := x86_64-w64-mingw32-gcc
  endif
  ifeq (64,$(PLATBIT))
    CC := x86_64-w64-mingw32-gcc
  endif
  USE_DYNAMIC := 1
endif

# look into zig for macos and other platforms
#ifeq (macos,$(BASEPLAT))
  
#endif

build: $(CLASSICUBE_DIR) $(OBJECT_DIR)

ifeq (1,$(USE_DYNAMIC))
  include $(MAKEFILE_DIR)/dynamic.mk
endif

build: $(TARGET)

$(CLASSICUBE_DIR):
	$(GIT) clone --depth 1 https://github.com/ClassiCube/ClassiCube.git $(CLASSICUBE_DIR)

$(OBJECT_DIR):
	$(MKDIR) $(OBJECT_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(CCFLAGS) $(LDFLAGS)

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) -c $< -o $@ $(CCFLAGS) $(CFLAGS)

clean: cleanobj
	$(RM) $(TARGET)

cleanobj:
	$(RMDIR) $(OBJECT_DIR)

.PHONY: build clean cleanobj

