include $(MYROOT)/src/make.dep

all: ch_miteno

MAINOBJS=$(MYROOT)/src/module/buffer_handle_miteno.o $(MYROOT)/src/module/ap_global.o\
         $(MYROOT)/src/module/ap_base.o $(MYROOT)/src/module/transctrl.o

OBJS=hookfuncs.o

ch_miteno: $(OBJS)
	$(CXX) -w -o $@  $(OBJS) $(MAINOBJS)  $(CMPLIBS) $(XML2_LIB) $(APR_LIB) $(ORACLE_LIB) 
	mv $@ $(MYROOT)/bin
	rm -f tp*


.SUFFIXES:.ecpp .ec .cpp .pc .pcpp

.pcpp.o:
	$(PROC) DEFINE=_LARGEFILE64_SOURCE INCLUDE=$(CMP_INCL_DIR) INCLUDE=$(APR_INCL_DIR) sqlcheck=SYNTAX userid=$(DBUSER)/$(DBPASS) DEF_SQLCODE=YES PARSE=NO code=cpp iname=$< oname=$*.cpp
	$(GCC) -D_LARGEFILE64_SOURCE $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(ORACLE_HOME)/include -g -c $*.cpp
	rm -f $*.cpp $*.lis

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
	rm -f tp*
