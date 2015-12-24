include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libcustom.a

OBJS = log_udo.o

$(CMP_LIB_DIR)/libcustom.a: $(OBJS) 
	$(AR)  $@ $(OBJS)

.SUFFIXES:.ec .pc .c

.pc.o:
	proc DEFINE=_LARGEFILE64_SOURCE INCLUDE=$(CMP_INCL_DIR) INCLUDE=$(APR_INCL_DIR) sqlcheck=SYNTAX userid=$(DBUSER)/$(DBPASS) DEF_SQLCODE=YES PARSE=NO iname=$*.pc oname=$*.c
	$(CC) -D_LARGEFILE64_SOURCE $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(ORACLE_HOME)/include -g -c $*.c
	rm -f $*.c $*.lis 

.ec.o:
	$(EC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<
	rm -f $*.c

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
