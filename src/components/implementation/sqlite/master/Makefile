C_OBJS=os_composite.o
ASM_OBJS=
COMPONENT=sql.o
INTERFACES=sqlite
DEPENDENCIES=sched printc timed_blk cbufp cbuf_c torrent evt mem_mgr_large valloc lock
OBJLIBS += $(POSIX_LIB)
IF_LIB=./cos_libccv.a $(DIETDIR)/bin-i386/libm.a ./libsqlite3.a

pre: first all
first:
	$(info | [CP] copying cos_libccv.a from libccv)
	@cp $(CCVOBJ) .
	$(info | [CP] copying libsqlite3.a from ./components/lib)
	@cp $(SQLITEOBJ) .

include ../../Makefile.subsubdir
