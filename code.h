#ifndef _CODE_H
#define _CODE_H

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
#define DEBUG(x) x
#else
#define DEBUG(x) /* x */
#endif

#define PROP_PAIR(x) {#x, &x}

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

class z
{
	public:
		z();
		z(bool);
		zfs *open_fs(const char *name);
		zfs *open_fs(const char *name, zfs_type_t);
		zpool *open_pool(const char *name);
		~z();
	private:
		void init(bool);
		libzfs_handle_t *m_handle;
		zfs_handle_t* raw_open_fs(const char *name, zfs_type_t);
		friend class zfs;
};
class zfs
{
	public:
		zfs() {}
		zfs(const zfs&);
		~zfs();
		const char *name() const;
		zfs_type_t type() const;
		const char *type_string(void) const;
		int raw_prop_get(zfs_prop_t, char *, size_t, my_zprop_t *, char *, size_t, boolean_t);
		PyObject *prop_get_int(zfs_prop_t);
		zfs& zfs::operator=(const zfs &);
		enum iter_type { filesystems, children, dependents, snapshots, root };
		int iter_filesystems(PyObject *, PyObject *);
		int iter_children(PyObject *, PyObject *);
		int iter_dependents(PyObject *, PyObject *, bool);
		int iter_snapshots(PyObject *, PyObject *);
		int iter_root(PyObject *, PyObject *);
		int send(char *snap, PyObject *writeTo, PyObject *kwargs);
		int send(char *fromsnap, char *tosnap, PyObject *writeTo, bool verbose = false, bool replicate = false, bool doall = false, bool fromorigin = false, bool dedup = false, bool props = false, PyObject *callable = NULL, PyObject *callableArg = NULL);
		int receive(const char *tosnap, int fromFD, PyObject *kwargs);
		int receive(const char *tosnap, int fromFD, bool verbose, bool isprefix, bool istail, bool dryrun, bool force, bool canmountoff);
		zfs(z*, libzfs_handle_t *, zfs_handle_t *);
	private:
		void init(z*, const libzfs_handle_t *, zfs_handle_t *);
		libzfs_handle_t *m_handle;
		zfs_handle_t *m_openfs;
		z *m_parent;
		int zfs::generic_iter(PyObject*, PyObject*, iter_type, bool);
		friend class z;
};
class zpool
{
	public:
		zpool(const zpool&);
		~zpool();
		const char *name() const;
		void scrub(const pool_scrub_type_t) const;
	private:
		zpool(libzfs_handle_t *, zpool_handle_t *);
		libzfs_handle_t *m_handle;
		zpool_handle_t *m_openpool;
		friend class z;
};
#endif
