include $(MYROOT)/src/make.dep

all: soaptest 


LCNTOBJS=soapC.o stdsoap2.o soapClient.o posServiceClient.o test.o

soaptest: $(LCNTOBJS)
	$(CXX) $(LDFLAGS) -o $@ $(LCNTOBJS) 

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
	$(CXX) $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(GSOAP_HOME)/include -g -c $<

.c.o:
	$(CXX) $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(GSOAP_HOME)/include -g -c $<

clean:
	rm -f *.o 






