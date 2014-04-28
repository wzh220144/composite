#include <print.h>
#include <sqlite.h>

//#define VERBOSE 1

int callback(void * a, int count, char ** value, char **name) {	//callback function, used for ouput values in table
	int i;
	for(i=0; i<count; i++) {
		printc("%s %s\n", value[i], name[i]);
	}
	return 0;
}

void cos_init(void) {

	int i,m,n;
	sqlite3 *db = NULL;
	char *sql=NULL;
	char **dbResult;
	int nRow,nColumn;
	int index;
	int rc = sqlite_open("test.db", &db);	//open test.db
	char *output;
	int result;
	sqlite3_stmt **stmt;
	char **zTail;
	if(rc) {		//cannot open test.db
		printc("Cannot open database: ");
		sqlite_errmsg(db);		//output the error message
		sqlite_close(db);		//close test.db
		return ;
	}
	printc("Have opened sqlite file in composite successfully!!!\n");
	sql = "create table table_1( ID integer primary key autoincrement, Username nvarchar(32), PassWord nvarchar(32))";
	result = sqlite_exec( db, sql, 0, 0);	//execute the sql: create table_1
	if(result != SQLITE_OK ) {
		printc("Fail to create table_1: %d mesg: ", result);
		sqlite_cur_errmsg();		//output the error message from sqlite_exec
	}
	printc("Have created table_1!!!\n");
	sql="insert into table_1 values(1, 'wzh1', 'wzh1')";
	result = sqlite_exec( db, sql, 0, 0);	//execute the sql: insert value
	if(result != SQLITE_OK ) {
		printc("Fail to insert user wzh1: %d mesg: ", result);
		sqlite_cur_errmsg();		//output the error message from sqlite_exec
	}   
	printc("Have run inserted command!!!\n");
	sql="insert into table_1 values(2, 'wzh2', 'wzh2')";
	result = sqlite_exec( db, sql, 0, 0);	//execute the sql: insert value
	if(result != SQLITE_OK ) {
		printc("Fail to instert user wzh2: %d mesg: ", result);
		sqlite_cur_errmsg();		//output the error message from sqlite_exec
	}
	printc("Have run inserted command!!!\n");

	/*sql="insert into table_1 values(3,'hao1','hao1')";
	result = sqlite3_exec(db,sql,0,0,&errmsg);
	sql="insert into table_1 values(4,'hao2','hao2')";
	result = sqlite3_exec(db,sql,0,0,&errmsg);
	sql="insert into table_1 values(5,'hao3','hao3')";
	result = sqlite3_exec(db,sql,0,0,&errmsg);
	
	sql="insert into table_1 (Username,ID) values(?,?)";
	sqlite3_prepare_v2(db,sql,-1,&stmt,&zTail);
	char str[]="WZH3";
	int a = 6;
	sqlite3_bind_text(stmt,1,str,-1,SQLITE_STATIC);
	sqlite3_bind_int(stmt,2,a);
	result = sqlite3_step(stmt);
	if(result!=SQLITE_DONE)
		printc("%s",sqlite3_errmsg(db));
	sqlite3_reset(stmt);
	char str2[]="WZH4";
	int a2=7;
	sqlite3_bind_text(stmt,1,str2,-1,SQLITE_STATIC);
	sqlite3_bind_int(stmt,2,a2);
	result=sqlite3_step(stmt);
	printc("1111\n");

	if(result!=SQLITE_OK){
		printc("%s",sqlite3_errmsg(db));
	}
	
	printc("222222\n");
	sqlite3_finalize(stmt);
	printc("3333333\n");
//test for gettable function
	sql="select * from table_1";
	result = sqlite3_get_table(db,sql,&dbResult,&nRow,&nColumn,&errmsg);
	if(SQLITE_OK==result){
		index=nColumn;
		printc("have selected %d records:\n", nRow);
		for(m=0;m<nRow;m++){
			printc("the number %d records:\n",m+1);
			for(n=0;n<nColumn;n++){
				printc("Name is %s, value is %s\n", dbResult[n], dbResult[index]);
				++index;
			}

		}
	}
	else{
		printc("fail to use the gettable function\n");
	}
	
	printc("have inquiried the database without callback!!!\n");*/
	sql = "select * from table_1";
	result = sqlite_exec( db, sql, callback, NULL);	//execute the sql: ouput everything in table_1
	if(result != SQLITE_OK ) {
		printc("Fail to select: %d mesg: ", result);
		sqlite_cur_errmsg();
	}
	printc("Have print the message from select command\n");
	/*sql = "update table_1 set Username='update' where ID=2";
	result= sqlite3_exec(db,sql,callback,0,&errmsg);
	if(result != SQLITE_OK){
		printc("fail to update the table\n");
		sqlite3_free(errmsg);
	}
	sql="delete from table_1 where ID<3";
	result= sqlite3_exec(db,sql,callback,NULL,&errmsg);
	if(result!= SQLITE_OK){
		printc("fail to delete from the table\n");
	}
	
	printc("Have deleted the record ID less than 3!!!\n");
	sql = "select * from table_1";
        result = sqlite3_exec( db, sql, callback, NULL, &errmsg );
        if(result != SQLITE_OK ) {
                printc("Fail to select: %d mesg:%s\n", result, errmsg);
                sqlite3_free(errmsg);
        }
        printc("Have print the message from select command\n");*/
	sqlite_close(db);
	printc("Have closed sqlite3 file in composite successfully!!!\n");
	return;
}

