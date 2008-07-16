#ifndef _CODE_H
#define _CODE_H

#include <Python.h>
#include <libzfs.h>
#include <sys/fs/zfs.h>
#include <stdio.h>
#include <ucontext.h>
#include <string>

#ifdef OPENSOLARIS
typedef zprop_source_t my_zprop_t ;
#else
typedef zfs_source_t my_zprop_t ;
#endif
#ifdef DO_DEBUG
#define DEBUG(x) x
#else
#define DEBUG(x) /* x */
#endif

class zfs;
class zpool;

class Exception 
{
	public:
		Exception(std::string s, std::string text) 
			{ source = s; description = text; }
		std::string description, source;
		const char *error()
		{
			return (source + ": " + description).c_str();
		}
};

class z
{
	public:
		z();
		z(bool);
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
