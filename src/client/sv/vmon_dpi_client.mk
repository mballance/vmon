
VMON_CLIENT_SV_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

VMON := $(abspath $(VMON_CLIENT_SV_DIR)/../../..)
export VMON

ifneq (1,$(RULES))

VMON_CLIENT_SV_SRC := $(notdir $(wildcard $(VMON_CLIENT_SV_DIR)/*.cpp))

SRC_DIRS += $(VMON_CLIENT_SV_DIR)

else

libvmon_client_dpi.o : $(VMON_CLIENT_SV_SRC:.cpp=.o)
	$(Q)$(LD) -r -o $@ $(VMON_CLIENT_SV_SRC:.cpp=.o)

endif
