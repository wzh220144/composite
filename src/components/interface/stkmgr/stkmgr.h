/**
 * Copyright 2009 by Andrew Sweeney, ajs86@gwu.edu
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#ifndef _STKMGR_H_
#define _STKMGR_H_


/** 
 * In the future we may want to change this too
 * cos_asm_server_stub_spdid
 */
//spdid_t spdid
vaddr_t stkmgr_get_stack(spdid_t d_spdid, vaddr_t d_addr);
void * stkmgr_grant_stack(spdid_t d_spdid);
void stkmgr_return_stack(spdid_t s_spdid, void *addr);
#endif




