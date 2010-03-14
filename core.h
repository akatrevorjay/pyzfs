#ifndef _CORE_H
#define _CORE_H

#include "config.h"

#include <Python.h>
#include <libzfs.h>
#include <sys/fs/zfs.h>
#include <stdio.h>
#include <ucontext.h>
#include <string>

#ifdef NEED_EXTERN_C
#define EXTERN extern "C++"
#else
#define EXTERN extern
#endif

#ifdef HAVE_ZFS_SOURCE_T
typedef zfs_source_t my_zprop_t ;
#else
typedef zprop_source_t my_zprop_t ;
#endif
#ifdef DO_DEBUG
#define DEBUG(priority, x) if (priority >= DO_DEBUG) { x; }
#else
#define DEBUG(priority, x) /* x */
#endif

class zfs;
class zpool;

class Exception
{
	public:
		Exception(PyObject *type, std::string text) 
			{ m_type = type; m_text = (char *)text.c_str(); }
		Exception(PyObject *type, const char *text) 
			{ m_type = type; m_text = (char *)text; }
		char *m_text;
		PyObject *m_type;
};

#endif
