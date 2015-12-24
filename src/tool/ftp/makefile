include $(MYROOT)/src/make.dep
EXECFILE=myftpd
TESTFILE=myput myget
all: $(EXECFILE) $(TESTFILE)

INCPATH=-I$(MYROOT)/include
#LIBS=-L$(CMP_ROOT)/lib -lcommon -lbase
LIBS=-L$(MYROOT)/lib -lbase 
LINKFLAGS=
FLAG=-g 
OBJS=myftpd.o

$(EXECFILE): $(OBJS) myput myget
	$(CXX) $(FLAG) $(CXXFLAG) -o $@ $(OBJS) $(LIBS) $(LINKFLAGS)
	mv $(EXECFILE) $(MYROOT)/bin
	@rm -f *.o

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) -D DEBUG $(FLAG) $(CXXFLAG) $(INCPATH) -c $<

.c.o:
	$(CXX) -D DEBUG $(FLAG) $(CFLAG) $(INCPATH) -c $<

myput: myput.o myftpcli.o
	$(CXX) $(FLAG) $(CXXFLAG) -o $@ myput.o myftpcli.o $(LIBS) $(LINKFLAGS)
#	mv $@ $(MYROOT)/bin

myget: myget.o myftpcli.o
	$(CXX) $(FLAG) $(CXXFLAG) -o $@ myget.o myftpcli.o $(LIBS) $(LINKFLAGS)
#	mv $@ $(MYROOT)/bin

clean:
	rm -f $(EXECFILE) $(TESTFILE) *.o

