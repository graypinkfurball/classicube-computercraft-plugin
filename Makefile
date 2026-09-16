export NAME := computercraft

export SOURCE_DIR := src
export MISC_DIR := misc
export MAKEFILE_DIR := misc/makefiles
export SCRIPT_DIR := misc/scripts
export CLASSICUBE_DIR := classicube
export OBJECT_DIR := obj
export DYNAMIC_DIR := dynamic

export RM := rm -f
export RMDIR := rm -rf
export MKDIR := mkdir -p
export GIT := git
export LUA := lua
export NM := nm
export NOOP := @:

PLATS := linux32 linux64 windows32 windows64
PLATGOALS := default linux windows all $(PLATS)

default linux: linux64
windows: windows64

SUBGOALS := $(filter-out $(PLATGOALS),$(MAKECMDGOALS))
LAST_SUBGOAL := $(word $(words $(SUBGOALS) _),_ $(SUBGOALS))

ifeq (clean,$(LAST_SUBGOAL))
  all: $(PLATS) ; $(RMDIR) $(OBJECT_DIR)
else
  all: $(PLATS)
endif

$(PLATS):
	@$(MAKE) --no-print-directory -f $(MAKEFILE_DIR)/build.mk "PLAT=$@" $(SUBGOALS)

ifeq (,$(filter $(PLATGOALS),$(MAKECMDGOALS)))
  build: default
  clean cleanobj: all
else
  build clean cleanobj: ; $(NOOP)
endif

.PHONY: $(PLATGOALS) build clean cleanobj

