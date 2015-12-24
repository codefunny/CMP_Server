include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libsecu.a

OBJ = hsm30.o

$(CMP_LIB_DIR)/libsecu.a : $(OBJ)
	$(AR) $@ $(OBJ)

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -rf *.o
