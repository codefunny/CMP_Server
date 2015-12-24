include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libstable.a

OBJS = svc_base.o svc_control.o

$(CMP_LIB_DIR)/libstable.a: $(OBJS)
	$(AR)  $@ $(OBJS)

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
