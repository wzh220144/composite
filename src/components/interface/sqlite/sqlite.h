#ifndef SQLITE_H
#define SQLITE_H

#include <sqlite3.h>
#include <torrent.h>

typedef struct hash hash;
struct hash {
        hash *pre;
        hash *next;
        td_t fd;
        char *path;
};

void sqlite_init(void);
sqlite3_vfs *sqlite3_comvfs(void);

#endif	/* ifndef SQLITE_H */
