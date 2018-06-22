/****************************************************************************
 * vmon_client_ep0_if.svh
 ****************************************************************************/

/**
 * Class: vmon_client_ep0_if
 * 
 * TODO: Add class documentation
 */
interface class vmon_client_ep0_if;
	
	pure virtual function void msg(string m);
	
	pure virtual function void tracepoint(int unsigned tp);
	
	pure virtual function void endtest(int status);
	
endclass


