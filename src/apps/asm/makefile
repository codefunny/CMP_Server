include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libcustom.a

OBJS = asm_interface.o asm_fun.o

$(CMP_LIB_DIR)/libcustom.a: $(OBJS)
	ar rv $@ $(OBJS)

.SUFFIXES:.pc .cpp .sqc  .ec

.pc.o:
	$(PROC) DEFINE=_LARGEFILE64_SOURCE INCLUDE=$(CMP_INCL_DIR) INCLUDE=$(APR_INCL_DIR) sqlcheck=SYNTAX userid=$(DBUSER)/$(DBPASS) DEF_SQLCODE=YES PARSE=NO iname=$*.pc oname=$*.c
	$(CC) -D_LARGEFILE64_SOURCE $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(ORACLE_HOME)/include -g -c $*.c
	rm -f $*.c $*.lis tp*

.sqc.o:
	db2 connect to $(DBNAME) user $(DBUSER) using $(DBPASS);
	db2 prep $*.sqc bindfile using $(BNDDIR)/$*.bnd datetime ISO
	db2 bind $(BNDDIR)/$*.bnd datetime ISO blocking all grant public
	db2 grant execute on package $* to public;
	$(CXX) -c $(CFLAGS) $(FLAG) $(INCPATH) $(DB2_INCL)  $*.c
	rm -f $*.c $*.csc

.c.o: 
	$(CC) $(CFLAGS) $(FLAG) $(INCPATH) -c $<
.ec.o:
	$(EC)  $(INCPATH) -g -c $<
	rm $*.c

clean:
	rm -rf *.o tp*
