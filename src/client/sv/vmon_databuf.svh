/****************************************************************************
 * vmon_databuf.svh
 ****************************************************************************/

/**
 * Class: vmon_databuf
 * 
 * TODO: Add class documentation
 */
class vmon_databuf;
	local chandle		m_hndl;

	function new(chandle hndl);
		m_hndl = hndl;
	endfunction
	
	function int unsigned sz();
		return _vmon_databuf_sz(m_hndl);
	endfunction
	
	function byte unsigned at(int unsigned i);
		return _vmon_databuf_at(m_hndl, i);
	endfunction

endclass


