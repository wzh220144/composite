#ifndef SQLITE_H
#define SQLITE_H

#include <torrent.h>
#include <sqlite3.h>

//#define VERBOSE 1
#ifdef VERBOSE
#define printv(fmt,...) printc(fmt, ##__VA_ARGS__)
#define prints(fmt) printc(fmt)
#else
#define printv(fmt,...)
#define prints(fmt)
#endif

int sqlite3_open(const char *filename, sqlite3 **ppDb);
int sqlite3_close(sqlite3*);
const char *sqlite3_errmsg(sqlite3*);
/*void sqlite_init();
sqlite3_vfs *sqlite3_comvfs();
static int comDirectWrite(ComFile *p, const void *zBuf, int iAmt, sqlite_int64 iOfst);
static int comFlushBuffer(ComFile *p);
static int comClose(sqlite3_file *pFile);
static int comTruncate(sqlite3_file *pFile, sqlite_int64 size);
static int comSync(sqlite3_file *pFile, int flags);
static int comFileSize(sqlite3_file *pFile, sqlite_int64 *pSize);
static int comRead(sqlite3_file *pFile, void *zBuf, int iAmt, sqlite_int64 iOfst);
static int comWrite(sqlite3_file *pFile, const void *zBuf, int iAmt, sqlite_int64 iOfst);
static int comLock(sqlite3_file *pFile, int eLock);
static int comUnlock(sqlite3_file *pFile, int eLock);
static int comCheckReservedLock(sqlite3_file *pFile, int *pResOut);
static int comFileControl(sqlite3_file *pFile, int op, void *pArg);
static int comSectorSize(sqlite3_file *pFile);
static int comDeviceCharacteristics(sqlite3_file *pFile);
static int comOpen(sqlite3_vfs *pVfs, const char *zName, sqlite3_file *pFile, int flags, int *pOutFlags);
static int comDelete(sqlite3_vfs *pVfs, const char *zPath, int dirSync);
static int comAccess(sqlite3_vfs *pVfs, const char *zPath, int flags, int *pResOut);
static int comFullPathname(sqlite3_vfs *pVfs, const char *zPath, int nPathOut, char *zPathOut);
static void *comDlOpen(sqlite3_vfs *pVfs, const char *zPath);
static void comDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg);
static void (*comDlSym(sqlite3_vfs *pVfs, void *pH, const char *z))(void);
static void comDlClose(sqlite3_vfs *pVfs, void *pHandle);
static int comRandomness(sqlite3_vfs *pVfs, int nByte, char *zByte);
static int comSleep(sqlite3_vfs *pVfs, int nMicro);
static int comCurrentTime(sqlite3_vfs *pVfs, double *pTime);
*/
#endif	/* ifndef SQLITE_H */
