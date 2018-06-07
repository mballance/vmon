
include $(SIMSCRIPTS_DIR)/mkfiles/plusargs.mk

BUILD_COMPILE_TARGETS += $(BUILD_DIR)/vmon_unit$(EXEEXT)
RUN_TARGETS += run_test
# MK_INCLUDES += $(GOOGLETEST_UVM)/src/rules_defs.mk
MK_INCLUDES += $(VMON)/src/client/vmon_client.mk
MK_INCLUDES += $(VMON)/src/client/sv/vmon_dpi_client.mk
MK_INCLUDES += $(VMON)/src/monitor/vmon_monitor.mk
MK_INCLUDES += $(VMON)/src/monitor/sv/vmon_monitor_dpi.mk
# MK_INCLUDES += $(VMON)/src/monitor/rules_defs.mk

DPI_OBJS_LIBS += libvmon_client.o libvmon_monitor.o libvmon_client_dpi.o

TB=sv_stub_hdl
TB_MODULES_HDL=sv_stub_hdl
TB_MODULES_HVL=sv_stub_hvl

ifeq (true,$(QUIET))
REDIRECT:= > run.log 2>&1
else
REDIRECT:= 2>&1 | tee run.log
endif

include $(SIMSCRIPTS_DIR)/mkfiles/common_sim.mk
