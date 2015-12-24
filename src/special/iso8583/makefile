include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/lib8583.a

OBJ = liso8583.o

$(CMP_LIB_DIR)/lib8583.a : $(OBJ)
	$(AR) $@ $(OBJ)

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -rf *.o