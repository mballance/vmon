/****************************************************************************
 * vmon_client_ep_if.svh
 ****************************************************************************/

/**
 * Class: vmon_client_ep_if
 * 
 * TODO: Add class documentation
 */
interface class vmon_client_ep_if;

	pure virtual function void process_msg(byte unsigned ep, vmon_databuf data);

endclass


