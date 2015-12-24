include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libbase.a

OBJS = tool_ascii.o tool_base.o tool_datetime.o log4c.o

$(CMP_LIB_DIR)/libbase.a: $(OBJS)
	$(AR)  $@ $(OBJS)
	@echo "Make [$@] Success!"

.c.o: 
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
