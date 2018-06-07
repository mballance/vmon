
if test "x$SIMSCRIPTS_PROJECT_ENV" = "xtrue"; then
	export VMON=`dirname $SIMSCRIPTS_DIR`
	export VMON=`dirname $VMON`
fi

export GOOGLETEST_UVM=${VMON}/ve/googletest_uvm
export SV_BFMS=${VMON}/subprojects/sv_bfms
export STD_PROTOCOL_IF=${VMON}/subprojects/std_protocol_if
export WB_SYS_IP=${VMON}/subprojects/wb_sys_ip

