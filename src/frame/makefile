include $(MYROOT)/src/make.dep

FLAG=-I$(TUX_INCL_DIR)
ECXX=esql 


all: $(CMP_LIB_DIR)/libbase.a apgetshm cmdsvr cmp_cmd nlmanager module

BASEOBJ=ap_base.o
CMDOBJ=ap_cmdsvr.o
CMPCMDOBJ=cmp_cmd.o
ANYOBJ=ap_manager.o
HANDLEOBJ=ap_global.o buffer_handle.o longconnect_handle.o longconnect_queue_handle.o buffer_handle_miteno.o\
          udo_handle.o queue_handle.o ap_base.o\
          8583_handle.o  transctrl.o udp_handle.o
#tuxfml32_handle.o mq_handle.o 

LIBS=-L$(CMP_LIB_DIR) -lbase $(APR_LIB) $(XML2_LIB) 
#LIBS=-L$(CMP_LIB_DIR) -lbase $(APR_LIB) $(XML2_LIB) -L/usr/lib -lpthread

$(CMP_LIB_DIR)/libbase.a: $(BASEOBJ)
	$(AR)  $@ $(BASEOBJ)

cmdsvr: $(CMDOBJ) $(BASEOBJ)
	$(CXX) $(LDFLAGS) -o $@ $(CMDOBJ) $(BASEOBJ) $(LIBS)
	mv $@ $(MYROOT)/bin

apgetshm:  apgetshm.o $(BASEOBJ)
#	$(CXX) -o $@ apgetshm.o $(BASEOBJ) $(LIBS)
	$(CXX) $(LDFLAGS) -o $@ apgetshm.o $(BASEOBJ) $(LIBS)
	mv $@ $(MYROOT)/bin

cmp_cmd: $(CMPCMDOBJ)
	$(CXX) $(LDFLAGS) -o $@ $(CMPCMDOBJ)  $(LIBS)
	mv $@ $(MYROOT)/bin

nlmanager: $(ANYOBJ)
	$(CXX) $(LDFLAGS) -o $@ $(ANYOBJ) $(BASEOBJ) $(LIBS)
	mv $@ $(MYROOT)/bin

module: $(HANDLEOBJ)
	mv $(HANDLEOBJ) $(MYROOT)/src/module

mq_handle.o:mq_handle.cpp
	$(CXX) -I$(MQ_INCL_DIR) $(CFLAGS) $(INCPATH) -g -c $<

.c.o:
	$(CXX) $(FLAG) $(CFLAGS) $(INCPATH) -g -c $<

.SUFFIXES:.cpp .pc .ec
.pc.o:
	$(PROC) DEFINE=_LARGEFILE64_SOURCE INCLUDE=$(CMP_INCL_DIR) INCLUDE=$(APR_INCL_DIR) sqlcheck=SYNTAX userid=$(DBUSER)/$(DBPASS) DEF_SQLCODE=YES PARSE=NO iname=$*.pc oname=$*.c
	$(CC) -D_LARGEFILE64_SOURCE -DSQLNOTFOUND=1403 $(CFLAGS) $(FLAG) -I. $(INCPATH) -I$(ORACLE_HOME)/include -g -c $*.c
	rm -f $*.c $*.lis 

.cpp.o:
	$(CXX) $(FLAG) $(CFLAGS) $(INCPATH) -g -c $<

.ec.o:
	$(EC)  $(INCPATH) -g -c $<
clean:
	rm -f *.o

