include $(MYROOT)/src/make.dep

all: $(CMP_BIN_DIR)/shmload $(CMP_BIN_DIR)/shmread

FLAG=-DNEW_VERSION

OBJS1 = shmload.o shm_fmain.o shm_ftable.o
OBJS2 = shmread.o 

$(CMP_BIN_DIR)/shmload : $(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $(OBJS1) -L$(CMP_LIB_DIR) -lbase

$(CMP_BIN_DIR)/shmread : $(OBJS2)
	$(CC) $(LDFLAGS) -o $@ $(OBJS2) -L$(CMP_LIB_DIR) -lbase

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
