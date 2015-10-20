.SUFFIXES:

# Locate our submodules and figure out what the target library names should be.
RUNC := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
SUBMAKES := $(patsubst $(RUNC)/%,%,$(wildcard $(RUNC)/*/Makefile))
MODULES := $(dir $(SUBMAKES))
LIBS := $(join $(MODULES), lib$(MODULES:%/=%.a))
SRCS := $(foreach dir, $(MODULES),$(wildcard $(dir)*.c))
OBJS := $(subst /,/obj/,$(basename $(SRCS))).o
OUTLIB := librunc.a

all: $(OUTLIB)

$(OUTLIB): $(LIBS) $(OBJS)
	ar rcs $@ $(OBJS)

%.a:
	@$(MAKE) -s -C $(dir $@) -f Makefile

clean:
	-@rm -f $(OUTLIB)
	@for dir in $(MODULES); do \
		$(MAKE) -s -C $$dir -f Makefile clean; \
	done

.PHONY: clean all

