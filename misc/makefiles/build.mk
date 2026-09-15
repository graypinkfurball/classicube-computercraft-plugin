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

TARGET := $(NAME)_$(BASEPLAT)_$(PLATBIT).$(EXT)

SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCES))

CCFLAGS := -pipe -fPIC
CFLAGS := -I$(CLASSICUBE_DIR) -Wall -fvisibility=hidden -ffreestanding -fno-ident -O3 -g
LDFLAGS := -nostdlib -s -shared

ifeq (linux32,$(PLAT))
  CCFLAGS += -m32
endif

ifeq (windows32,$(PLAT))
  CC := x86_64-w64-mingw32-gcc
endif

ifeq (windows64,$(PLAT))
  CC := x86_64-w64-mingw32-gcc
endif

ifeq (windows,$(BASEPLAT))
  LDFLAGS += -Wl,--entry=0 -lkernel32
  USE_DYNAMIC := 1
endif

build: $(CLASSICUBE_DIR) $(OBJECT_DIR)

ifeq (1,$(USE_DYNAMIC))
  include $(MAKEFILE_DIR)/dynamic.mk
else
  build: $(TARGET)
endif

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

