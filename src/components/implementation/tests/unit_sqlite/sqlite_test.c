#include <print.h>
#include <sqlite.h>
#include <sqlite3.h>

//#define VERBOSE 1
#ifdef VERBOSE
#define printv(fmt,...) printc(fmt, ##__VA_ARGS__)
#define prints(fmt) printc(fmt)
#else
#define printv(fmt,...)
#define prints(fmt)
#endif


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
		printc("%s %s\n", value[i], name[i]);
	}
	return 0;
}

void cos_init(void) {
	sqlite_init();
	int i;
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
	printc("Have opened sqlite file in composite successfully!!!\n");
	sql = "create table table_1( ID integer primary key autoincrement, Username nvarchar(32), PassWord nvarchar(32))";
	result = sqlite3_exec( db, sql, 0, 0, &errmsg);
	if(result != SQLITE_OK ) {
		printc("Fail to create table_1: %d mesg:%s\n", result, errmsg);
		sqlite3_free(errmsg);
	}
	printc("Have created table_1!!!\n");
	sql="insert into table_1 values(1, 'wzh1', 'wzh1')";
	result = sqlite3_exec( db, sql, 0, 0, &errmsg );
	if(result != SQLITE_OK ) {
		printc("Fail to insert user wzh1: %d mesg:%s\n", result, errmsg);
		sqlite3_free(errmsg);
	}
	printc("Have run inserted command!!!\n");
	sql="insert into table_1 values(2, 'wzh2', 'wzh2')";
	result = sqlite3_exec( db, sql, 0, 0, &errmsg );
	if(result != SQLITE_OK ) {
		printc("Fail to inster user wzh2: %d mesg:%s\n", result, errmsg);
		sqlite3_free(errmsg);
	}
	printc("Have run inserted command!!!\n");
	sql = "select * from table_1";
	result = sqlite3_exec( db, sql, callback, NULL, &errmsg );
	if(result != SQLITE_OK ) {
		printc("Fail to select: %d mesg:%s\n", result, errmsg);
		sqlite3_free(errmsg);
	}
	printc("Have print the message from select command\n");
	sqlite3_close(db);
	printc("Have closed sqlite3 file in composite successfully!!!\n");
	return;
}

