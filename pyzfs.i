%module pyzfs
%{
#include "code.h"
int Z = 0x5a; // blargh SWIG parses zfs.h wrong
%}

%include common.i
