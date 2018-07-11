
VMON_MKFILES_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
VMON_DIR := $(abspath $(VMON_MKFILES_DIR)/..)

VMON := $(VMON_DIR)
export VMON

ifneq (1,$(RULES))

VMON_JAR = $(VMON_DIR)/lib/libvmon.jar
VMON_DEPS = $(STD_PROTOCOL_IF_JARS)
VMON_JARS = $(VMON_JAR) $(VMON_DEPS)

else # Rules


endif

