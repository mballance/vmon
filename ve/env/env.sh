
if test "x$SIMSCRIPTS_PROJECT_ENV" = "xtrue"; then
	export VMON=`dirname $SIMSCRIPTS_DIR`
	export VMON=`dirname $VMON`
fi

export GOOGLETEST_UVM=${VMON}/ve/googletest_uvm
