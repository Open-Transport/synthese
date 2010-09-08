
#LINKOPT	=	/usr/local/lib/libdbmalloc.a 
#LINKOPT = 	/usr/local/lib/cpl.a
CFLAGS	=	-g -Wall
#CFLAGS  =       -g -DUSE_CPL
INSTALL = /usr/bin/install
LD = /usr/bin/ld
#CC = g++

default:	all

all:	shpcreate shpadd shpdump shprewind dbfcreate dbfadd dbfdump shptest

shpopen.o:	shpopen.c shapefil.h
	$(CC) $(CFLAGS) -c shpopen.c

shptree.o:	shptree.c shapefil.h
	$(CC) $(CFLAGS) -c shptree.c

dbfopen.o:	dbfopen.c shapefil.h
	$(CC) $(CFLAGS) -c dbfopen.c

safileio.o:	safileio.c shapefil.h
	$(CC) $(CFLAGS) -c safileio.c

shpcreate:	shpcreate.c shpopen.o safileio.o 
	$(CC) $(CFLAGS) shpcreate.c shpopen.o safileio.o $(LINKOPT) -o shpcreate

shpadd:		shpadd.c shpopen.o safileio.o
	$(CC) $(CFLAGS) shpadd.c shpopen.o safileio.o $(LINKOPT) -o shpadd

shpdump:	shpdump.c shpopen.o safileio.o
	$(CC) $(CFLAGS) shpdump.c shpopen.o safileio.o $(LINKOPT) -o shpdump

shprewind:	shprewind.c shpopen.o safileio.o
	$(CC) $(CFLAGS) shprewind.c shpopen.o safileio.o $(LINKOPT) -o shprewind

dbfcreate:	dbfcreate.c dbfopen.o safileio.o
	$(CC) $(CFLAGS) dbfcreate.c dbfopen.o safileio.o $(LINKOPT) -o dbfcreate

dbfadd:		dbfadd.c dbfopen.o safileio.o
	$(CC) $(CFLAGS) dbfadd.c dbfopen.o safileio.o $(LINKOPT) -o dbfadd

dbfdump:	dbfdump.c dbfopen.o safileio.o
	$(CC) $(CFLAGS) dbfdump.c dbfopen.o safileio.o $(LINKOPT) -o dbfdump

shptest:	shptest.c shpopen.o safileio.o
	$(CC) $(CFLAGS) shptest.c shpopen.o safileio.o $(LINKOPT) -o shptest

shputils:	shputils.c shpopen.o safileio.o dbfopen.o 
	$(CC) $(CFLAGS) shputils.c shpopen.o safileio.o dbfopen.o  $(LINKOPT) -o shputils

shptreedump:	shptreedump.c shptree.o shpopen.o safileio.o
	$(CC) $(CFLAGS) shptreedump.c shptree.o shpopen.o safileio.o $(LINKOPT) \
		-o shptreedump

clean:
	rm -f *.o dbfdump dbfcreate dbfadd shpdump shpcreate shpadd shputils
	rm -f shptreedump
	rm -rf *.lo *.la .libs
	rm -f shptest

test:	test2 test3

#
#	Note this stream only works if example data is accessable.
#	Fetch ftp://gdal.velocet.ca/pub/outgoing/shape_eg_data.zip
#
test1:
	@./stream1.sh > s1.out
	@if test "`diff s1.out stream1.out`" = '' ; then \
	    echo "******* Stream 1 Succeeded *********"; \
	    rm s1.out; \
	else \
	    echo "******* Stream 1 Failed *********"; \
	    diff s1.out stream1.out; \
	fi

test2:
	@./stream2.sh > s2.out
	@if test "`diff s2.out stream2.out`" = '' ; then \
	    echo "******* Stream 2 Succeeded *********"; \
	    rm s2.out; \
	    rm test*.s??; \
	else \
	    echo "******* Stream 2 Failed *********"; \
	    diff s2.out stream2.out; \
	fi

test3:
	@./makeshape.sh > s3.out
	@if test "`diff s3.out stream3.out`" = '' ; then \
	    echo "******* Stream 3 Succeeded *********"; \
	    rm s3.out; \
	    rm test.*; \
	else \
	    echo "******* Stream 3 Failed *********"; \
	    diff s3.out stream3.out; \
	fi


# -----------------------------------------------------------------------------
# The following is contributed by Jan-Oliver Wagner, and should allow for
# creating shared libraries on most platforms with gcc, and libtool installed.

SHPLIB_VERSION=1.2.9
LIBSHP_VERSION=1.0.1 # still once to be changed manually (see for 1:1:0), sorry

lib:
	/bin/sh ./libtool --mode=compile gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1  -I. -I. -I/usr/local/include    -g -O2 -c shpopen.c
	gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1 -I. -I. -I/usr/local/include -g -O2 -c  -fPIC -DPIC shpopen.c -o .libs/shpopen.lo
	gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1 -I. -I. -I/usr/local/include -g -O2 -c shpopen.c -o shpopen.o >/dev/null 2>&1
	mv -f .libs/shpopen.lo shpopen.lo
	/bin/sh ./libtool --mode=compile gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1  -I. -I. -I/usr/local/include    -g -O2 -c shptree.c
	rm -f .libs/shptree.lo
	gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1 -I. -I. -I/usr/local/include -g -O2 -c  -fPIC -DPIC shptree.c -o .libs/shptree.lo
	gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1 -I. -I. -I/usr/local/include -g -O2 -c shptree.c -o shptree.o >/dev/null 2>&1
	mv -f .libs/shptree.lo shptree.lo
	/bin/sh ./libtool --mode=compile gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1  -I. -I. -I/usr/local/include    -g -O2 -c dbfopen.c
	rm -f .libs/dbfopen.lo
	gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1 -I. -I. -I/usr/local/include -g -O2 -c  -fPIC -DPIC dbfopen.c -o .libs/dbfopen.lo
	gcc -DPACKAGE=\"libshp\" -DVERSION=\"$(SHPLIB_VERSION)\" -DSTDC_HEADERS=1 -I. -I. -I/usr/local/include -g -O2 -c dbfopen.c -o dbfopen.o >/dev/null 2>&1
	mv -f .libs/dbfopen.lo dbfopen.lo
	/bin/sh ./libtool --mode=link gcc  -g -O2  -o libshp.la -rpath /usr/local/lib -version-info 1:1:0 shpopen.lo shptree.lo dbfopen.lo  
	rm -fr .libs/libshp.la .libs/libshp.* .libs/libshp.*
	rm -fr .libs/libshp.lax
	mkdir .libs/libshp.lax
	$(LD) -G -h libshp.so.1 -o .libs/libshp.so.$(LIBSHP_VERSION)  shpopen.lo shptree.lo dbfopen.lo  -lc

	(cd .libs && rm -f libshp.so.1 && ln -s libshp.so.$(LIBSHP_VERSION) libshp.so.1)
	(cd .libs && rm -f libshp.so && ln -s libshp.so.$(LIBSHP_VERSION) libshp.so)
	ar cru .libs/libshp.a  shpopen.o shptree.o dbfopen.o 
	ranlib .libs/libshp.a
	rm -fr .libs/libshp.lax
	(cd .libs && rm -f libshp.la && ln -s ../libshp.la libshp.la)

lib_install:
	cp .libs/libshp.la .libs/libshp.lai
	/bin/sh ./mkinstalldirs /usr/local/lib
	/bin/sh ./libtool  --mode=install $(INSTALL) -c libshp.la /usr/local/lib/libshp.la
	$(INSTALL) -c .libs/libshp.so.$(LIBSHP_VERSION) /usr/local/lib/libshp.so.$(LIBSHP_VERSION)
	(cd /usr/local/lib && rm -f libshp.so.1 && ln -s libshp.so.$(LIBSHP_VERSION) libshp.so.1)
	(cd /usr/local/lib && rm -f libshp.so && ln -s libshp.so.$(LIBSHP_VERSION) libshp.so)
	chmod +x /usr/local/lib/libshp.so.$(LIBSHP_VERSION)
	$(INSTALL) -c .libs/libshp.la /usr/local/lib/libshp.la
	$(INSTALL) -c .libs/libshp.a /usr/local/lib/libshp.a
	ranlib /usr/local/lib/libshp.a
	chmod 644 /usr/local/lib/libshp.a
	/bin/sh ./mkinstalldirs /usr/local/include/libshp
	$(INSTALL) -c -m 644 shapefil.h /usr/local/include/libshp/shapefil.h
