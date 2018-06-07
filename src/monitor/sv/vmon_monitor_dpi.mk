
VMON_MONITOR_SRC_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq (1,$(RULES))

VMON_SRC := $(notdir $(wildcard $(VMON_MONITOR_SRC_DIR)/*.c))
SRC_DIRS += $(VMON_MONITOR_SRC_DIR)

DPI_OBJS_LIBS += libvmon_monitor_sv.o

else # Rules

libvmon_monitor_sv.o : $(VMON_SRC:.c=.o)
	$(Q)$(LD) -r -o $@ $^

endif
