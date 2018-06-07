
VMON_CLIENT_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq (1,$(RULES))

VMON_CLIENT_SRC := $(notdir $(wildcard $(VMON_CLIENT_DIR)/*.cpp))

SRC_DIRS += $(VMON_CLIENT_DIR) $(VMON_CLIENT_DIR)/../common

else

libvmon_client.o : $(VMON_CLIENT_SRC:.cpp=.o)
	$(Q)$(LD) -r -o $@ $(VMON_CLIENT_SRC:.cpp=.o)

endif
