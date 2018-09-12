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
	
	function byte unsigned get8();
		return _vmon_databuf_get8(m_hndl);
	endfunction
	
	function shortint unsigned get16();
		return _vmon_databuf_get16(m_hndl);
	endfunction
	
	function int unsigned get32();
		return _vmon_databuf_get32(m_hndl);
	endfunction

endclass

import "DPI-C" context function int unsigned _vmon_databuf_sz(chandle databuf);
	
import "DPI-C" context function byte unsigned _vmon_databuf_at(chandle databuf, int unsigned idx);

import "DPI-C" context function byte unsigned _vmon_databuf_get8(chandle databuf);

import "DPI-C" context function shortint unsigned _vmon_databuf_get16(chandle databuf);

import "DPI-C" context function int unsigned _vmon_databuf_get32(chandle databuf);


