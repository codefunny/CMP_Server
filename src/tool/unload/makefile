include $(MYROOT)/src/make.dep

all: unload

CFLAGS=-g
LDFLAGS=-g -ldl -lpthread
MYSQL_INCL_DIR=/usr/include/mysql
MYSQL_LIB_DIR=/usr/lib64/mysql
unload: unload.c
#	$(CC) $(FLAG) $(CFLAGS) $(LDFLAGS) -I$(MYSQL_INCL_DIR) -o $@ $? -L/home/cmp/packs/mysql-max-5.0.27-hpux11.23-ia64-64bit/lib -lmysqlclient 
	$(CC) $(FLAG) $(CFLAGS) $(LDFLAGS) -I$(MYSQL_INCL_DIR) -o $@ $? -L$(MYSQL_LIB_DIR) -lmysqlclient -lm 

	mv $@ $(CMP_BIN_DIR)

clean:
	rm -f unload.o
