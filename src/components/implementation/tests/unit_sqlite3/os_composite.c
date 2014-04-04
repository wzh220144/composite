/*
** 2014 March 29
**
** Copyright 2014 by Zhihua Wang, wzh22014@gmail.com
**
** Redistribution of this file is permitted under the GNU General
** Public License v2.
**
** In place of a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
*/

/*
** All the path must be absolute path.
** This composite vfs assumes that paths are like UNIX style. Specifically, that:
**
**   1. Path components are separated by a '/'. and 
**   2. Absolute paths begin with any character except '/'.
*/

#ifndef __SQLITE__
#define __SQLITE__

#include <cos_component.h>
#include <print.h>
#include <sched.h>
#include <cbuf.h>
#include <evt.h>
#include <torrent.h>
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>
#include <cos_types.h>
#include <time.h>
#include <timed_blk.h>
#include <evt.h>
#include <stdlib.h>

#define VERBOSE 1
#ifdef VERBOSE
#define printv(fmt,...) printc(fmt, ##__VA_ARGS__)
#define prints(fmt) printc(fmt)
#else
#define printv(fmt,...)
#define prints(fmt)
#endif

/*
** Size of the write buffer used by journal files in bytes.
*/
#ifndef SQLITE_COMVFS_BUFFERSZ
# define SQLITE_COMVFS_BUFFERSZ 8192
#endif

/*
** The maximum pathname length supported by this VFS.
*/
#define MAXPATHNAME 512

#define DEF_HASH_NUM 41		//the size of hash_node
typedef struct hash hash;
struct hash {
	hash *pre;
	hash *next;
	td_t fd;
	char *path;
};

hash *hash_node[DEF_HASH_NUM];

int find_hash_id(char *path, int len) {
	int ret=-1;
	int i;
	if( strlen(path) !=len )
		return ret;
	ret=1;
	for(i=0; i<len; i++) {
		ret*=path[i];
		ret%=DEF_HASH_NUM;
	}
	return ret;
}

int add_hash_node(td_t fd, char *path, int len) {
	prints("add_hash_node\n");
	int ret=-1;
	int id;
	printv("len: %d path: %d\n", len, strlen(path));
	if( len != strlen(path) )
		return ret;
	hash *thash = (hash *)malloc(sizeof(hash));
	thash->fd = fd;
	thash->path = malloc(len*sizeof(char));
	memcpy(thash->path, path, len);
	ret = find_hash_id(path, len);
	printv("ret: %d\n", ret);
	if(ret==-1)
		return ret;
	id = ret;
	if(hash_node[id]==NULL) {
		hash_node[id]=thash;
		thash->next = thash;
		thash->pre = thash;
	}
	else {
		thash->pre=hash_node[id];
		thash->next=hash_node[id]->next;
		hash_node[id]->next->pre=thash;
		hash_node[id]->next=thash;
	}
	printv("%d %d %d\n", hash_node[id], hash_node[id]->next, hash_node[id]->pre);
	printv("fd: %d\n", hash_node[id]->fd);
	return 0;
}

td_t find_hash_node(char *path, int len) {
	prints("find_hash_node\n");
	hash *thash;
	int ret=-1;
	int id;
	td_t fd=-1;
        if( len != strlen(path) )
                return fd;
        ret = find_hash_id(path, len);
	if(ret==-1)
		return fd;
	printv("%s\n", path);
        printv("ret: %d\n", ret);
	id=ret;
	thash = hash_node[id];
	while(thash!=NULL) {
		if(strcmp(thash->path, path)==0) {
			fd = thash->fd;
			break;
		}
		if(thash->next == hash_node[id])
			break;
	}
	return fd;
}

int delete_hash_node(td_t fd, char *path, int len) {
	prints("delete_hash_node\n");
	hash *thash;
        int ret=-1;
        int id;
        if( len != strlen(path) )
                return ret;
        ret = find_hash_id(path, len);
	printv("%s\n", path);
	printv("ret: %d\n", ret);
        if(ret==-1)
                return ret;
	id = ret;
	ret = -1;
	thash = hash_node[id];
	while(thash!=NULL) {
		if(thash->fd == fd) {
			if(thash->next == thash) {
				hash_node[id] = NULL;
				free(thash);
			}
			else {
				thash->pre->next=thash->next;
				thash->next->pre=thash->pre;
				free(thash);
			}
			ret=0;
		}
		if(thash->next == hash_node[id])
			break;
	}
	return ret;
}

/*
** When using this VFS, the sqlite3_file* handles that SQLite uses are
** actually pointers to instances of type ComFile.
*/
typedef struct ComFile ComFile;
struct ComFile {
	sqlite3_file base;	/* Base class. Must be first. */
	spdid_t pid;		/* spd id */
	td_t fd;		/* Usually it is file descriptor, but in 
				composite, it is torrent id, which is like 
				socket id*/
	long evtid;		/* event id */
	char *aBuffer;		/* Pointer to malloc'd buffer */
	int nBuffer;		/* Valid bytes of data in zBuffer */
	sqlite3_int64 iBufferOfst;	/* Offset in file of zBuffer[0] */
	//sqlite3_int64 file_size;	/* Size of file */
};


/*
** Write directly to the file passed as the first argument. Even if the
** file has a write-buffer (ComFile.aBuffer), ignore it.
*/
static int comDirectWrite(
  ComFile *p,                    /* File handle */
  const void *zBuf,               /* Buffer containing data to write */
  int iAmt,                       /* Size of data to write in bytes */
  sqlite_int64 iOfst              /* File offset to write to */
){
	prints("comDirectWrite\n");
	char bfst[21];		/* Return the string of iOfst */
	int rfst;		/* Return value from twmeta() */
	int rwrite;		/* Return value from twrite_pack() */
	char flags[10]="offset";
	int rc = SQLITE_OK;

	sprintf(bfst, "%lld", iOfst);	/* transfor iOfst to char[] */

	rfst = twmeta(cos_spd_id(), p->fd, flags, strlen(flags), bfst, strlen(bfst));
	if( rfst == -1 ){
		rc = SQLITE_IOERR_WRITE;
	}
	if(rc == SQLITE_OK) {
		rwrite = twrite_pack(cos_spd_id(), p->fd, (char *)zBuf, iAmt);	//cos_spd_id() might be changed to p->pid
		if( rwrite == -1 ){
			rc = SQLITE_IOERR_WRITE;
		}
	}
	printv("rc: %d\n", rc);
	return rc;
}

/*
** Flush the contents of the ComFile.aBuffer buffer to disk. This is a
** no-op if this particular file does not have a buffer (i.e. it is not
** a journal file) or if the buffer is currently empty.
*/
static int comFlushBuffer(ComFile *p){
	prints("comFlushBuffer\n");
	int rc = SQLITE_OK;
	if( p->nBuffer ){
		rc = comDirectWrite(p, p->aBuffer, p->nBuffer, p->iBufferOfst);
		/* update ComFile struct */
		p->iBufferOfst += p->nBuffer;
 		p->nBuffer = 0;
	}
	printv("rc: %d\n", rc);
	return rc;
}

/*
** Close a file.
*/
static int comClose(sqlite3_file *pFile){
	prints("comClose\n");
	int rc;
	ComFile *p = (ComFile*)pFile;
	rc = comFlushBuffer(p);
	/* update ComFile struct */
	sqlite3_free(p->aBuffer);
	p->iBufferOfst += 0;
	p->nBuffer = 0;
	trelease(cos_spd_id(), p->fd);
	printv("rc: %d\n", rc);
	return rc;
}

/*
** Read data from a file.
*/
static int comRead(
  sqlite3_file *pFile, 
  void *zBuf, 
  int iAmt, 
  sqlite_int64 iOfst
){
	prints("comRead\n");
	char bfst[21];		/* Return the string of iOfst */
	ComFile *p = (ComFile*)pFile;
	off_t rfst;                     /* Return value from lseek() */
	int rread;                      /* Return value from read() */
	int rc;                         /* Return code from comFlushBuffer() */
	char flags[10]="offset";	


	sprintf(bfst, "%lld", iOfst);	/* transfor iOfst to char[] */

	/* Flush any data in the write buffer to disk in case this operation
	** is trying to read data the file-region currently cached in the buffer.
	** It would be possible to detect this case and possibly save an 
	** unnecessary write here, but in practice SQLite will rarely read from
	** a journal file when there is data cached in the write-buffer.
	*/
	rc = comFlushBuffer(p);
	printv("rc: %d\n", rc);
	if( rc!=SQLITE_OK )
		return rc;

	printv("fd: %d\n", p->fd);

	rfst = twmeta(p->pid, p->fd, flags, strlen(flags), bfst, strlen(bfst));
	printv("rfst: %d\n", rfst);
	if( rfst == -1 )
		return SQLITE_IOERR_READ;
	rread = tread_pack(cos_spd_id(), p->fd, (char *)zBuf, iAmt);
	printv("rread: %d\n", rread);
	if( rread != -1 )
		return SQLITE_OK;
	else
		return SQLITE_IOERR_READ;
}

/*
** Write data to a crash-file.
*/
static int comWrite(
  sqlite3_file *pFile, 
  const void *zBuf, 
  int iAmt, 
  sqlite_int64 iOfst
){
	prints("comWrite\n");
	ComFile *p = (ComFile*)pFile;
	int rc;

	if( p->aBuffer ){
		char *z = (char *)zBuf;		/* Pointer to remaining data to write */
		int n = iAmt;			/* Number of bytes at z */
		sqlite3_int64 i = iOfst;	/* File offset to write to */

		while( n>0 ){
			int nCopy;		/* Number of bytes to copy into buffer */

			/* If the buffer is full, or if this data is not being written directly
			** following the data already buffered, flush the buffer. Flushing
			** the buffer is a no-op if it is empty.  
			*/
			if( p->nBuffer==SQLITE_COMVFS_BUFFERSZ || p->iBufferOfst+p->nBuffer!=i ){
				rc = comFlushBuffer(p);
				printv("rc: %d\n", rc);
				if( rc!=SQLITE_OK ){
					return rc;
				}
			}
			assert( p->nBuffer==0 || p->iBufferOfst+p->nBuffer==i );
			p->iBufferOfst = i - p->nBuffer;

			/* Copy as much data as possible into the buffer. */
			nCopy = SQLITE_COMVFS_BUFFERSZ - p->nBuffer;
			if( nCopy>n ){
				nCopy = n;
			}
			memcpy(&p->aBuffer[p->nBuffer], z, nCopy);
			p->nBuffer += nCopy;
		
			n -= nCopy;
			i += nCopy;
			z += nCopy;
		}
	}
	else{
    		rc = comDirectWrite(p, zBuf, iAmt, iOfst);
		printv("rc: %d\n", rc);
		return rc;
	}
	prints("rc: 0\n");
	return SQLITE_OK;
}

/*
** Truncate a file. This is a no-op for this VFS (see header comments at
** the top of the file).
*/
static int comTruncate(sqlite3_file *pFile, sqlite_int64 size){
	prints("comTruncate\n");
#if 0
  if( ftruncate(((ComFile *)pFile)->fd, size) ) return SQLITE_IOERR_TRUNCATE;
#endif
	
	prints("rc: 0\n");
	return SQLITE_OK;
}

/*
** Sync the contents of the file to the persistent media.
** composite does not support system call like fsync, so all return
** value will be comFlushBuffer return value
*/
static int comSync(sqlite3_file *pFile, int flags){
	prints("comSync\n");
	ComFile *p = (ComFile*)pFile;
	int rc;

	rc = comFlushBuffer(p);
	//rc = fsync(p->fd);
	printv("rc: %d\n", rc);
	return rc;
}

/*
** Write the size of the file in bytes to *pSize.
*/
static int comFileSize(sqlite3_file *pFile, sqlite_int64 *pSize){
	prints("comFileSize\n");
	ComFile *p = (ComFile*)pFile;
	int rc;				/* Return code from tremta() */
	int i;				/* Iterator variable */
	/* Flush the contents of the buffer to disk. As with the flush in the
	** comRead() method, it would be possible to avoid this and save a write
	** here and there. But in practice this comes up so infrequently it is
	** not worth the trouble.
	*/
	rc = comFlushBuffer(p);
	printv("rc: %d\n", rc);
	if( rc!=SQLITE_OK ){
		return rc;
	}
	rc = tsize(cos_spd_id(), p->fd);
	if( rc==-1) {
		return SQLITE_IOERR;
	}
	*pSize = rc;
	printv("file_size: %d\n", rc);
	return SQLITE_OK;
}

/*
** Locking functions. The xLock() and xUnlock() methods are both no-ops.
** The xCheckReservedLock() always indicates that no other process holds
** a reserved lock on the database file. This ensures that if a hot-journal
** file is found in the file-system it is rolled back.
*/
static int comLock(sqlite3_file *pFile, int eLock){
	prints("comLock\n");
	prints("rc: 0\n");
	return SQLITE_OK;
}
static int comUnlock(sqlite3_file *pFile, int eLock){
	prints("comUnlock\n");
	prints("rc: 0\n");
	return SQLITE_OK;
}
static int comCheckReservedLock(sqlite3_file *pFile, int *pResOut){
	prints("comCheckReservedLock\n");
	prints("rc: 0\n");
	*pResOut = 0;
	return SQLITE_OK;
}

/*
** No xFileControl() verbs are implemented by this VFS.
*/
static int comFileControl(sqlite3_file *pFile, int op, void *pArg){
	prints("comFileControl\n");
	prints("rc: 0\n");
	return SQLITE_OK;
}

/*
** The xSectorSize() and xDeviceCharacteristics() methods. These two
** may return special values allowing SQLite to optimize file-system 
** access to some extent. But it is also safe to simply return 0.
*/
static int comSectorSize(sqlite3_file *pFile){
	prints("comSectorSize\n");
	prints("rc: 0\n");
	return 0;
}
static int comDeviceCharacteristics(sqlite3_file *pFile){
	prints("comDeviceCharacteristics\n");
	prints("rc: 0\n");
	return 0;
}

/*
** Open a file handle. ***File must be absolute path****.
*/
static int comOpen(
  sqlite3_vfs *pVfs,              /* VFS */
  const char *zName,              /* File to open, or 0 for a temp file */
  sqlite3_file *pFile,            /* Pointer to ComFile struct to populate */
  int flags,                      /* Input SQLITE_OPEN_XXX flags */
  int *pOutFlags                  /* Output SQLITE_OPEN_XXX flags (or NULL) */
){
	prints("comOpen\n");
  static const sqlite3_io_methods comio = {
    1,                            /* iVersion */
    comClose,                    /* xClose */
    comRead,                     /* xRead */
    comWrite,                    /* xWrite */
    comTruncate,                 /* xTruncate */
    comSync,                     /* xSync */
    comFileSize,                 /* xFileSize */
    comLock,                     /* xLock */
    comUnlock,                   /* xUnlock */
    comCheckReservedLock,        /* xCheckReservedLock */
    comFileControl,              /* xFileControl */
    comSectorSize,               /* xSectorSize */
    comDeviceCharacteristics     /* xDeviceCharacteristics */
  };
	long evt = evt_split(cos_spd_id(), 0, 0);
	ComFile *p = (ComFile*)pFile; /* Populate this structure */
	int oflags = 0;                 /* flags to pass to tsplit() call */
	char *aBuf = 0;

	if( zName==0 ){		//do not support temp file
		printv("rc: %d\n", SQLITE_IOERR);
		return SQLITE_IOERR;
	}

	if( flags&SQLITE_OPEN_MAIN_JOURNAL ){
		aBuf = (char *)sqlite3_malloc(SQLITE_COMVFS_BUFFERSZ);
		if( !aBuf ){
			printv("rc: %d\n", SQLITE_NOMEM);
			return SQLITE_NOMEM;
		}
	}

	/*if( flags&SQLITE_OPEN_EXCLUSIVE )
		oflags |= O_EXCL;
	if( flags&SQLITE_OPEN_CREATE )
		oflags |= O_CREAT;
	if( flags&SQLITE_OPEN_READONLY )
		oflags |= O_RDONLY;
	if( flags&SQLITE_OPEN_READWRITE )
		oflags |= O_RDWR;*/
	oflags |= TOR_ALL;	//do not support unix file flags, this is composite torrent flags

	memset(p, 0, sizeof(ComFile));
	p->pid = cos_spd_id();
	if(zName[0]=='/') {
		printv("rc: %d\n", SQLITE_CANTOPEN);
		return SQLITE_CANTOPEN;
	}
	p->evtid = evt;
	p->fd = tsplit(p->pid, td_root, zName, strlen(zName), TOR_ALL, p->evtid);
	add_hash_node(p->fd, zName, strlen(zName));
	printv("fd: %d\n", p->fd);
	if( p->fd == -1 ){
		sqlite3_free(aBuf);
		printv("rc: %d\n", SQLITE_CANTOPEN);
		return SQLITE_CANTOPEN;
	}
	p->aBuffer = aBuf;
	p->nBuffer = 0;
	p->iBufferOfst = 0;

	if( pOutFlags ){
		*pOutFlags = TOR_ALL;
	}
	p->base.pMethods = &comio;
	printv("rc: %d\n", SQLITE_OK);
	return SQLITE_OK;
}

/*
** Delete the file identified by argument zPath. If the dirSync parameter
** is non-zero, then ensure the file-system modification to delete the
** file has been synced to disk before returning.
*/
static int comDelete(sqlite3_vfs *pVfs, const char *zPath, int dirSync){
	prints("comDelete\n");
	int rc;                         /* Return code */
	td_t fd = find_hash_node(zPath, strlen(zPath));
	printv("fd: %d\n", fd);
	rc = tmerge(cos_spd_id(), td_root, td_null, zPath, strlen(zPath));
	if( (rc == -1) && (errno == ENOENT) ) {
		printv("rc: %d\n", (rc==-1)&&(errno==ENOENT));
		return SQLITE_OK;
	}
	delete_hash_node(fd, zPath, strlen(zPath));

	//do not need dirSync, becasue the file is in cache, do not need to be synced
	//if( rc==0 && dirSync ){
	//	int dfd;                      /* File descriptor open on directory */
	//	int i;                        /* Iterator variable */
	//	char zDir[MAXPATHNAME+1];     /* Name of directory containing file zPath */

		/* Figure out the directory name from the path of the file deleted. */
	//	sqlite3_snprintf(MAXPATHNAME, zDir, "%s", zPath);
	//	zDir[MAXPATHNAME] = '\0';
	//	for(i=strlen(zDir); i>1 && zDir[i]!='/'; i--);
	//	zDir[i] = '\0';

    		/* Open a file-descriptor on the directory. Sync. Close. */
    	//	dfd = open(zDir, O_RDONLY, 0);
    	//	if( dfd<0 ) {
	//		rc = -1;
    	//	}
	//	else {
	//		rc = fsync(dfd);
	//		close(dfd);
	//	}
	//}
	printv("rc: %d\n", (rc==1) ? SQLITE_IOERR_DELETE: SQLITE_OK);
  	return (rc==-1 ? SQLITE_IOERR_DELETE : SQLITE_OK);
}

#ifndef F_OK
# define F_OK 0
#endif
#ifndef R_OK
# define R_OK 4
#endif
#ifndef W_OK
# define W_OK 2
#endif

/*
** Query the file-system to see if the named file exists, is readable or
** is both readable and writable.
** However, for composite, there does not exist system call like access.
** So, this routine will always return true.
** We must ensure the named file exists or has such abbribute.
*/
static int comAccess(
  sqlite3_vfs *pVfs, 
  const char *zPath, 
  int flags, 
  int *pResOut
){
	prints("comAccess\n");
	int rc;				/* access() return code */
	int eAccess = F_OK;		/* Second argument to access() */

	//assert( flags==SQLITE_ACCESS_EXISTS	/* access(zPath, F_OK) */
	//|| flags==SQLITE_ACCESS_READ		/* access(zPath, R_OK) */
	//|| flags==SQLITE_ACCESS_READWRITE	/* access(zPath, R_OK|W_OK) */
	/*);

	if( flags==SQLITE_ACCESS_READWRITE )
		eAccess = R_OK|W_OK;
	if( flags==SQLITE_ACCESS_READ )
		eAccess = R_OK;

	rc = access(zPath, eAccess);
	*pResOut = (rc==0);*/
	printv("rc: %d\n", SQLITE_OK);
	return SQLITE_OK;
}

/*
** Argument zPath points to a nul-terminated string containing a file path.
** zPath must be an absolute path
**
** This function assumes that paths are UNIX style. Specifically, that:
**
**   1. Path components are separated by a '/'. and 
**   2. Absolute paths begin with any character except.
*/
static int comFullPathname(
  sqlite3_vfs *pVfs,              /* VFS */
  const char *zPath,              /* Input path (possibly a relative path) */
  int nPathOut,                   /* Size of output buffer in bytes */
  char *zPathOut                  /* Pointer to output buffer */
){
	prints("comFullPathname\n");
	if( zPath[0]=='/' ){
		printv("rc: %d\n", SQLITE_IOERR);
		return SQLITE_IOERR;
	}

	sqlite3_snprintf(nPathOut, zPathOut, "%s", zPath);
	zPathOut[nPathOut-1] = '\0';
	printv("rc: %d\n", SQLITE_OK);
	return SQLITE_OK;
}

/*
** The following four VFS methods:
**
**   xDlOpen
**   xDlError
**   xDlSym
**   xDlClose
**
** are supposed to implement the functionality needed by SQLite to load
** extensions compiled as shared objects. This VFS for composite does not support
** this functionality, so the following functions are no-ops.
*/
static void *comDlOpen(sqlite3_vfs *pVfs, const char *zPath) {
	prints("comDlOpen\n");
	return 0;
}
static void comDlError(sqlite3_vfs *pVfs, int nByte, char *zErrMsg){
	prints("comDlError\n");
	sqlite3_snprintf(nByte, zErrMsg, "Loadable extensions are not supported");
	zErrMsg[nByte-1] = '\0';
}
static void (*comDlSym(sqlite3_vfs *pVfs, void *pH, const char *z))(void){
	prints("comDlSym\n");
	return 0;
}
static void comDlClose(sqlite3_vfs *pVfs, void *pHandle){
	prints("comDlClose\n");
	return;
}

/*
** Parameter zByte points to a buffer nByte bytes in size. Populate this
** buffer with pseudo-random data.
*/
static int comRandomness(sqlite3_vfs *pVfs, int nByte, char *zByte){
	prints("comRandomness\n");
	return SQLITE_OK;
}

/*
** Sleep for at least nMicro microseconds. Return the (approximate) number 
** of microseconds slept for.
*/
static int comSleep(sqlite3_vfs *pVfs, int nMicro){
	prints("comSleep\n");
	timed_event_block(cos_spd_id(), nMicro);
	return nMicro;
}

/*
** Set *pTime to the current UTC time expressed as a Julian day. Return
** SQLITE_OK if successful, or an error code otherwise.
**
**   http://en.wikipedia.org/wiki/Julian_day
**
** This implementation is not very good. The current time is rounded to
** an integer number of seconds. Also, assuming time_t is a signed 32-bit 
** value, it will stop working some time in the year 2038 AD (the so-called
** "year 2038" problem that afflicts systems that store time this way). 
*/
static int comCurrentTime(sqlite3_vfs *pVfs, double *pTime){
	prints("comCurrentTime\n");
	time_t t = time(0);
	*pTime = t/86400.0 + 2440587.5; 
	printv("rc: %d\n", SQLITE_OK);
	return SQLITE_OK;
}

/*
** This function returns a pointer to the VFS implemented in this file.
** To make the VFS available to SQLite:
**
**   sqlite3_vfs_register(sqlite3_comvfs(), 0);
*/
sqlite3_vfs *sqlite3_comvfs(void){
	prints("sqlite3_comvfs\n");
  static sqlite3_vfs comvfs = {
    1,				/* iVersion */
    sizeof(ComFile),		/* szOsFile */
    MAXPATHNAME,		/* mxPathname */
    0,				/* pNext */
    "com",			/* zName */
    0,				/* pAppData */
    comOpen,			/* xOpen */
    comDelete,			/* xDelete */
    comAccess,			/* xAccess */
    comFullPathname,		/* xFullPathname */
    comDlOpen,			/* xDlOpen */
    comDlError,			/* xDlError */
    comDlSym,			/* xDlSym */
    comDlClose,			/* xDlClose */
    comRandomness,		/* xRandomness */
    comSleep,			/* xSleep */
    comCurrentTime,		/* xCurrentTime */
  };
  return &comvfs;
}


int sqlite3_os_init(void){
	prints("sqlite3_os_init\n");
	sqlite3_vfs_register(sqlite3_comvfs(), 0);
	printv("rc: %d\n", SQLITE_OK);
	return SQLITE_OK; 
}

/*
** Shutdown the operating system interface.
**
** May need to do some clean up.
**
** This routine is a no-op for composite
*/
int sqlite3_os_end(void){
	prints("sqlite3_os_end\n");
	printv("rc: %d\n", SQLITE_OK);
	return SQLITE_OK; 
}

int callback(void * a, int count, char ** value, char **name) {
	int i;
	for(i=0; i<count; i++) {
		printf("%s %s\n", value[i], name[i]);
	}
	return 0;
}

void cos_init(void) {

	int i;
	for(i=0; i<DEF_HASH_NUM; i++)
		hash_node[i]=NULL;
	prints("cos_init\n");
	sqlite3 *db = NULL;
	char *sql=NULL;
	int rc = sqlite3_open("test.db", &db);
	char *errmsg=0;
	char *output;
	int result;
	if(rc) {
		printc("Cannot open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return ;
	}
	printc("Fuck!!!\nHave opened sqlite file in composite successfully!!!\n");
	sql = "create table table_1( ID integer primary key autoincrement, Username nvarchar(32), PassWord nvarchar(32))";
	result = sqlite3_exec( db, sql, 0, 0, &errmsg);
	if(result != SQLITE_OK ) {
		printc("Fail to create table_1: %d mesg:%s\n", result, errmsg);
		sqlite3_free(errmsg);
	}
	sql="insert into table_1 values('wzh1', 'wzh1')";
	result = sqlite3_exec( db, sql, 0, 0, &errmsg );
	if(result != SQLITE_OK ) {
		printc("Fail to insert user wzh1: %d mesg:%s\n", result, errmsg);
		sqlite3_free(errmsg);
	}
	sql="insert into table_1 values('wzh2', 'wzh2')";
	result = sqlite3_exec( db, sql, 0, 0, &errmsg );
	if(result != SQLITE_OK ) {
		printc("Fail to inster user wzh2: %d mesg:%s\n", result, errmsg);
		sqlite3_free(errmsg);
	}
	sql = "select * from table_1";
	result = sqlite3_exec( db, sql, callback, NULL, &errmsg );
	if(result != SQLITE_OK ) {
		printc("Fail to select: %d mesg:%s\n", result, errmsg);
		sqlite3_free(errmsg);
	}
	sqlite3_close(db);
	printc("Have closed sqlite3 file in composite successfully!!!\n");
	return;
}

#endif
