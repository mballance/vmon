
VMON_MONITOR_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq (1,$(RULES))

VMON_MONITOR_SRC := $(notdir $(wildcard $(VMON_MONITOR_DIR)/*.c))

SRC_DIRS += $(VMON_MONITOR_DIR)

else

libvmon_monitor.o : $(VMON_MONITOR_SRC:.c=.o)
	$(Q)$(LD) -r -o $@ $(VMON_MONITOR_SRC:.c=.o)

endif
