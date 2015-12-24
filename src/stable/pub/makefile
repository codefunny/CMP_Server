include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libbase.a

OBJS = udo_prt2file.o udo.o pub_base.o pub_cfg.o pub_math.o shm_base.o \
	easysock.o

$(CMP_LIB_DIR)/libbase.a: $(OBJS)
	$(AR)  $@ $(OBJS)
	@echo "Make [$@] Success!"

.c.o: 
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
