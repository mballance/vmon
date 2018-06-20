
VMON_CLIENT_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

ifneq (1,$(RULES))

VMON_CLIENT_SRC := $(notdir $(wildcard $(VMON_CLIENT_DIR)/*.cpp))

SRC_DIRS += $(VMON_CLIENT_DIR) $(VMON_CLIENT_DIR)/../common

VMON_CHISEL_MON_LIB := vmon_chisel_mon_lib.jar
VMON_CHISEL_MON_LIBS := $(WB_CHISEL_MON_LIB) $(STD_PROTOCOL_IF_LIB) $(CHISELLIB_JAR)
VMON_CHISEL_MON_SRC := \
	$(wildcard $(VMON_CLIENT_DIR)/chisel/vmon/*.scala)

else

libvmon_client.o : $(VMON_CLIENT_SRC:.cpp=.o)
	$(Q)$(LD) -r -o $@ $(VMON_CLIENT_SRC:.cpp=.o)

$(VMON_CHISEL_MON_LIB) : $(VMON_CHISEL_MON_SRC) $(STD_PROTOCOL_IF_LIB) $(CHISELLIB_JAR)
	$(Q)$(DO_CHISELC)

endif
