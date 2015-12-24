include $(MYROOT)/src/make.dep

all: ch_nac 

LCNTOBJS=ch_nac.o $(MYROOT)/src/module/ap_base.o

ch_nac: $(LCNTOBJS)
	$(CXX) $(LDFLAGS) -o $@ $(LCNTOBJS)  $(CMPLIBS) -ldl $(XML2_LIB) $(APR_LIB)
	mv $@ $(HOME)/bin

.SUFFIXES:.ecpp .ec .cpp .pc

.pc.o:
	$(PROC) DEFINE=_LARGEFILE64_SOURCE INCLUDE=$(CMP_INCL_DIR) INCLUDE=$(APR_INCL_DIR) sqlcheck=SYNTAX userid=$(DBUSER)/$(DBPASS) DEF_SQLCODE=YES PARSE=NO iname=$< oname=$*.c
	$(CC) -D_LARGEFILE64_SOURCE $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(ORACLE_HOME)/include -g -c $*.c
	rm -f $*.c $*.lis

.ecpp.o:
	$(EC) $(CFLAGS) $(FLAG) $(INCPATH) $? -c
	rm $*.C

.ec.o:
	$(EC) $(CFLAGS) $(FLAG) $(INCPATH) $? -c
	rm $*.c

.cpp.o:
	$(CXX) $(CFLAGS) $(FLAG) -I. $(INCPATH) -g -c $<

.c.o:
	$(CXX) $(CFLAGS) $(FLAG) -I. $(INCPATH) -g -c $<

clean:
	rm -f *.o
