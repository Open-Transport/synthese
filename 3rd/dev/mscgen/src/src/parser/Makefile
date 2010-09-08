# Makefile for msc parser library

YACC=bison
LEX=flex

LDFLAGS+=
CFLAGS+=-O2 -I. -Wall
OBJS  =language.tab.o lex.yy.o msc.o	

parser.a: $(OBJS)
	$(AR) cr $@ $^

language.tab.c language.tab.h: language.y
	$(YACC) -d $^

lex.yy.c: language.l
	$(LEX) $^
	
parsertest: parsertest.o parser.a
	$(CC) $(LDFLAGS) -o $@ $^
	
	
test: parsertest
	cat testinput0.msc | ./parsertest
	cat testinput1.msc | ./parsertest	
	cat testinput2.msc | ./parsertest
	
clean: FORCE
	rm -f parser.a language.tab.c lex.yy.c parsertest.o language.output $(OBJS)
	
distclean: clean
	rm -f parsertest parsertest.exe
	
FORCE:
# DO NOT DELETE
