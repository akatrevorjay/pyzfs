OBJECTS=pyzfs_wrap.o code.o
DEBUG_OBJECTS=pyzfs_debug_wrap.do code.do
all: pyzfs pyzfs_debug test
INCLUDES=-I/usr/include/python2.4
LIBS=-lpthread -lzfs -lpython2.4
EXTRADEFS=-DSWIG
COMPILER=SunStudio
OPENSOLARIS=yes
MYPOOL=data

ifeq ($(COMPILER),SunStudio)
	CFLAGS=
	CC=/opt/SunStudioExpress/bin/cc
	CXX=/opt/SunStudioExpress/bin/CC
	SHARED=-G
	LDFLAGS+=-L/opt/SunStudioExpress/lib -R/opt/SunStudioExpress/lib -lCstd
	INCLUDES+=-I/opt/SunStudioExpress/include
	CFILT=/opt/SunStudioExpress/bin/c++filt
else
ifeq ($(COMPILER),gcc)
	CFLAGS=-Wall
	CC=gcc
	CXX=g++
	SHARED=-shared
	CFILT=gc++filt
endif
endif
ifdef OPENSOLARIS
	EXTRADEFS+=-DOPENSOLARIS
endif
CFLAGS+=-g

pyzfs: _pyzfs.so 
pyzfs_debug: _pyzfs_debug.so
test: test.py
	/usr/bin/python test.py $(MYPOOL)
#	/usr/bin/python test.py $(MYPOOL) debug

_pyzfs.so: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(SHARED) $(LIBS) $(OBJECTS) -o $@
_pyzfs_debug.so: $(DEBUG_OBJECTS)
	$(CXX) $(LDFLAGS) $(SHARED) $(LIBS) $(DEBUG_OBJECTS) -o $@
%.o: %.c
	$(CC) -c -fPIC $(INCLUDES) -DSWIG $<
%.o: %.cxx
	$(CXX) $(CFLAGS) -c -fPIC $(INCLUDES) $(EXTRADEFS) $<
%.do: %.c
	$(CC) -c -fPIC $(INCLUDES) -DSWIG $< -o $@
%.do: %.cxx
	$(CXX) $(CFLAGS) -c -fPIC $(INCLUDES) -DDO_DEBUG $(EXTRADEFS) $< -o $@

pyzfs_wrap.cxx: pyzfs.i
	swig -Werror -c++ -classic -python -shadow pyzfs.i
pyzfs_debug_wrap.cxx: pyzfs_debug.i
	swig -Werror -c++ -classic -python -shadow pyzfs_debug.i
code.o: code.h
clean:
	rm -f $(OBJECTS) $(DEBUG_OBJECTS) pyzfs_wrap.cxx _pyzfs.so pyzfs_debug_wrap.cxx _pyzfs_debug.so *.pyc pyzfs.py pyzfs_debug.py
.PHONY: clean all pyzfs test
