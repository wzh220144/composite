#include <print.h>
#include <sqlite.h>

//#define VERBOSE 1

int callback(void * a, int count, char ** value, char **name) {
	int i;
	for(i=0; i<count; i++) {
		printc("%s %s\n", value[i], name[i]);
	}
	return 0;
}

void cos_init(void) {

	int i;
	prints("sqlite tests\n");
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
	/*sql = "create table table_1( ID integer primary key autoincrement, Username nvarchar(32), PassWord nvarchar(32))";
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
	printc("Have print the message from select command\n");*/
	sqlite3_close(db);
	printc("Have closed sqlite3 file in composite successfully!!!\n");
	return;
}

