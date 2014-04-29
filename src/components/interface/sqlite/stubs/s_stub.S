/**
 * Copyright 2011 by Gabriel Parmer, gparmer@gwu.edu
 *
 * Redistribution of this file is permitted under the GNU General
 * Public License v2.
 */

#include <cos_asm_server_stub.h>

.text
cos_asm_server_stub(sqlite_close)
cos_asm_server_stub(sqlite_open)	
cos_asm_server_stub(sqlite_errmsg)
cos_asm_server_stub(sqlite_exec)
cos_asm_server_stub(sqlite_cur_errmsg)
cos_asm_server_stub(sqlite_free_cur_errmsg)
cos_asm_server_stub(sqlite_prepare_v2)
cos_asm_server_stub(sqlite_bind_text)
cos_asm_server_stub(sqlite_bind_int)
cos_asm_server_stub(sqlite_step)
cos_asm_server_stub(sqlite_reset)
cos_asm_server_stub(sqlite_finalize)
cos_asm_server_stub(sqlite_get_table)
cos_asm_server_stub(sqlite_cur_db)
cos_asm_server_stub(sqlite_clear_cur_db)
cos_asm_server_stub(sqlite_clear_cur_result)
cos_asm_server_stub(sqlite_get_cur_result)
#cos_asm_server_stub(sqlite_init)
#cos_asm_server_stub(sqlite3_comvfs)
#cos_asm_server_stub(comDirectWrite)
#cos_asm_server_stub(comFlushBuffer)
#cos_asm_server_stub(comClose)
#cos_asm_server_stub(comTruncate)
#cos_asm_server_stub(comSync)
#cos_asm_server_stub(comFileSize)
#cos_asm_server_stub(comRead)
#cos_asm_server_stub(comWrite)
#cos_asm_server_stub(comLock)
#cos_asm_server_stub(comUnlock)
#cos_asm_server_stub(comCheckReservedLock)
#cos_asm_server_stub(comFileControl)
#cos_asm_server_stub(comSectorSize)
#cos_asm_server_stub(comDeviceCharacteristics)
#cos_asm_server_stub(comOpen)
#cos_asm_server_stub(comDelete)
#cos_asm_server_stub(comAccess)
#cos_asm_server_stub(comFullPathname)
#cos_asm_server_stub(comcomDlOpen)
#cos_asm_server_stub(comDlError)
#cos_asm_server_stub(comDlSym)
#cos_asm_server_stub(comDlClose)
#cos_asm_server_stub(comRandomness)
#cos_asm_server_stub(comSleep)
#cos_asm_server_stub(comCurrentTime)
