include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libstable.a

FLAG = -DDEBUG
LEX     = lex 
LEXFLAG = -DUSE_FLEX
#LEX     = lex 
YY      = yacc
OBJS = chk_api.o y.tab.o lex.yy.o

$(CMP_LIB_DIR)/libstable.a: $(OBJS)
	$(AR)  $@ $(OBJS)

y.tab.o:chk.y
	$(YY) -d chk.y
	$(CC) $(CFLAGS) -c y.tab.c

lex.yy.o:chk.l
	$(LEX) chk.l
	$(CC) $(CFLAGS) $(LEXFLAG) -c lex.yy.c

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
	rm -f y.tab.c
	rm -f y.tab.h
	rm -f lex.yy.c
