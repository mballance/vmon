
include $(SIMSCRIPTS_DIR)/mkfiles/plusargs.mk

BUILD_COMPILE_TARGETS += $(BUILD_DIR)/vmon_unit$(EXEEXT)
RUN_TARGETS += run_test
MK_INCLUDES += $(GOOGLETEST_UVM)/src/rules_defs.mk
MK_INCLUDES += $(VMON)/src/client/rules_defs.mk
MK_INCLUDES += $(VMON)/src/monitor/rules_defs.mk

SRC_DIRS += $(SIM_DIR)/src

VMON_UNIT_SRC := $(notdir $(wildcard $(SIM_DIR)/src/*.cpp))

VMON_UNIT_FLAGS += $(foreach a,$(call get_plusarg,gtest_filter,$(PLUSARGS)),--gtest_filter=$(a))

VMON_UNIT_OBJS += $(VMON_UNIT_SRC:.cpp=.o) $(GOOGLETEST_OBJ)
VMON_UNIT_OBJS += libvmon_client.o libvmon_monitor.o

ifeq (true,$(QUIET))
REDIRECT:= > run.log 2>&1
else
REDIRECT:= 2>&1 | tee run.log
endif

include $(SIMSCRIPTS_DIR)/mkfiles/common_sim.mk

run_test :
	$(Q)$(BUILD_DIR)/vmon_unit$(EXEEXT) $(VMON_UNIT_FLAGS) $(REDIRECT)

$(BUILD_DIR)/vmon_unit$(EXEEXT) : $(VMON_UNIT_OBJS)
	$(Q)$(CXX) -o $@ $(VMON_UNIT_OBJS) -lpthread
