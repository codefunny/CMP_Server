include $(MYROOT)/src/make.dep

LIBS=-L$(CMP_LIB_DIR)  -lstable -lbase -lsecu  -L/usr/lib -lpthread  -lfl

all: monitor

OBJS=config.o monitor.o dbfun.o

monitor: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) $(APR_LIB)  $(ORACLE_LIB)
	mv -f monitor $(MYROOT)/bin

.SUFFIXES:.pc .c

.pc.o:
	$(PROC) THREADS=NO DEFINE=_LARGEFILE64_SOURCE INCLUDE=$(CMP_INCL_DIR) INCLUDE=$(APR_INCL_DIR) sqlcheck=SYNTAX userid=$(DBUSER)/$(DBPASS) DEF_SQLCODE=YES PARSE=NO iname=$< oname=$*.c
	$(CC) -D_LARGEFILE64_SOURCE $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(ORACLE_HOME)/include -g -c $*.c 
	rm -f $*.c $*.lis

.c.o:
	$(CC) $(FLAG) $(CFLAG) $(INCPATH) -c $<

clean:
	rm -f *.o
	rm -f dbfun.c
	rm -f dbfun.lis
	rm -f tp*
