include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libdac.a

OBJ = dac.o des.o checkdac.o

$(CMP_LIB_DIR)/libdac.a : $(OBJ)
	$(AR) $@ $(OBJ)

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -rf *.o
