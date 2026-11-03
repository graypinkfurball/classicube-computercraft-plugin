NAME := computercraft

GIT := git
MKDIR := mkdir -p
RMDIR := rm -rf

SOURCE_DIR := src
OBJECT_DIR := obj
CLASSICUBE_DIR := classicube

SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst $(SOURCE_DIR)/%.c,$(OBJECT_DIR)/%.o,$(SOURCES))
TARGET := $(NAME)_linux_64.so

CFLAGS := -pipe -ffreestanding -fno-ident -fPIC -fvisibility=hidden -Wall -O3 -I$(CLASSICUBE_DIR)
LDFLAGS := -pipe -nostdlib -fPIC -s -shared

build: $(CLASSICUBE_DIR) $(OBJECT_DIR) $(TARGET)

$(CLASSICUBE_DIR):
	$(GIT) clone --depth 1 https://github.com/ClassiCube/ClassiCube.git $(CLASSICUBE_DIR)

$(OBJECT_DIR):
	$(MKDIR) $(OBJECT_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

cleandist: clean
	$(RMDIR) $(CLASSICUBE_DIR)

clean: cleanobj
	$(RM) $(TARGET)

cleanobj:
	$(RMDIR) $(OBJECT_DIR)

.PHONY: build cleandist clean cleanobj

