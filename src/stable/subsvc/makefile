include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libstable.a

OBJS=sub_base.o sub_control.o sub_monitor.o

$(CMP_LIB_DIR)/libstable.a: $(OBJS)
	$(AR)  $@ $(OBJS)

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
