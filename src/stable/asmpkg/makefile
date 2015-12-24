include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libcustom.a

OBJS = asm_base.o asm_api.o asm_systemfun.o

$(CMP_LIB_DIR)/libcustom.a: $(OBJS)
	$(AR)  $@ $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
