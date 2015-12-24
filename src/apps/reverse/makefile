include $(MYROOT)/src/make.dep

LIBS = -L$(MYROOT)/lib -L$(APR_ROOT)/lib -L$(XML_ROOT)/lib -lapr-1 -lxml2 -lpkg -lbase -ltlv

DAEOBJ=main.o\
       rev_fun.o\
       rev_adp.o\
       rev_base.o\
       rev_var.o\
       adp_client.o\
       rev_kernel.o
MAINOBJS= $(MYROOT)/src/module/ap_global.o

all: reverse 

reverse: $(DAEOBJ)
	$(CXX) $(FLAG) $(CXXFLAG) -o $@ $(DAEOBJ) $(MAINOBJS) $(LIBS) $(APR_LIB)  $(ORACLE_LIB)
	mv -f $@ $(MYROOT)/bin

.SUFFIXES:.pcpp .pc .cpp .c

.pcpp.o:
	$(PROC) DEFINE=_LARGEFILE64_SOURCE INCLUDE=$(CMP_INCL_DIR) INCLUDE=$(APR_INCL_DIR) sqlcheck=SYNTAX userid=$(DBUSER)/$(DBPASS) DEF_SQLCODE=YES PARSE=NO code=cpp iname=$< oname=$*.cpp
	$(CXX) -D_LARGEFILE64_SOURCE $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(ORACLE_HOME)/include -g -c $*.cpp
	rm -f $*.cpp $*.lis

.pc.o:
	$(PROC) DEFINE=_LARGEFILE64_SOURCE INCLUDE=$(CMP_INCL_DIR) INCLUDE=$(APR_INCL_DIR) sqlcheck=SYNTAX userid=$(DBUSER)/$(DBPASS) DEF_SQLCODE=YES PARSE=NO iname=$< oname=$*.c
	$(CC) -D_LARGEFILE64_SOURCE $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(ORACLE_HOME)/include -g -c $*.c 
	rm -f $*.c $*.lis

.cpp.o:
	$(CXX) $(FLAG) $(CXXFLAG) $(INCPATH) -c $<

.c.o:
	$(CC) $(FLAG) $(CFLAG) $(INCPATH) -c $<

clean:
	rm -f *.o
	rm -f rev_fun.c
	rm -f rev_adp.cpp
	rm -f rev_base.c
	rm -f main.c
	rm -f rev_kernel.c
	rm -rf *.lis
	rm -f rev_adp.c
	rm -f tp*
