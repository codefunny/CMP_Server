include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libqclean.a $(CMP_BIN_DIR)/qclean

OBJqclean = qclean.o qcleanfun.o
OBJreqclean = reqcleanque.o qcleanfun.o
OBJdtest = dtest.o qcleanfun.o
OBJtest2 = test2.o
OBJtest3 = test3.o


$(CMP_LIB_DIR)/libqclean.a: $(OBJreqclean)
	$(AR) $@ $(OBJreqclean)

$(CMP_BIN_DIR)/qclean: $(OBJqclean)
	$(CC) -o $@ $(OBJqclean) $(CMPLIBS)

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f $(ALL) *.o
