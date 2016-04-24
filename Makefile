default: all

DIRS:=kernel libc loader

all: $(DIRS)
.PHONY: all $(DIRS)
$(DIRS):
	$(MAKE) -C $@

CLEANDIRS:=$(DIRS:%=clean-%)
clean: $(CLEANDIRS)
.PHONY: clean $(CLEANDIRS)
$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean

