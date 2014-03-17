#include <cos_component.h>
#include <stdio.h>
#include <sqlite3.h>

/* TODO: added micro benchmark */ 

void
cos_init(void *args)
{
/* use posix functions or system calls ?*/ 
	sqlite3 *db;
	fprintf(stdout, "Fuck!!!\n");
	int rc = sqlite3_open(":memory", &db);	
        return;
}
