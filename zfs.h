#ifndef ZFS_H
#define ZFS_H
#include "core.h"
class z;
#include "z.h"

// libzfs.h has this, and we can use it nearly verbatim!
extern "C" boolean_t zfs_prop_readonly(zfs_prop_t prop);
bool _prop_readonly(zfs_prop_t prop);

#define PROP_PAIR(x) {#x, &x}

class zfs
{
	public:
		~zfs();
		#ifdef SWIG
		%feature("docstring") "Get the name of the current dataset object."
		#endif
		const char *name() const;
		#ifdef SWIG
		%feature("docstring") "Get the (enum-based) type of the current dataset object."
		#endif
		zfs_type_t type() const;
		#ifdef SWIG
		%feature("docstring") "Get the (string-based) type of the current dataset object."
		#endif
		const char *type_string(void) const;
		#ifdef SWIG
		%feature("docstring") "Retrieve the value of the builtin property \"prop\" as an integer.  Will return incorrect results for string-based properties (e.g., mountpoint)."
		#endif
		PyObject *prop_get_int(zfs_prop_t prop);
		#ifdef SWIG
		%feature("docstring") "Set the value of \"prop\" to \"value\"."
		#endif
		void prop_set(char *prop, char *val);
		#ifdef SWIG
		%feature("docstring") "Walk over the filesystems that are descendents of this one; for each filesystem \"fs\", execute call(fs, data)."
		#endif
		int iter_filesystems(PyObject *call, PyObject *data);
		#ifdef SWIG
		%feature("docstring") "Walk over the children of this dataset; for each dataset \"ds\", execute call(ds, data)."
		#endif
		int iter_children(PyObject *call, PyObject *data);
		#ifdef SWIG
		%feature("docstring") "Walk over the datasets that depend on this dataset; for each dataset \"ds\", execute call(ds, data).  If recurse is True, allow circular dependencies."
		#endif
		int iter_dependents(PyObject *call, PyObject *data, bool recurse);
		#ifdef SWIG
		%feature("docstring") "Walk over the snapshots of this dataset; for each snapshot \"snap\", execute call(snap, data)."
		#endif
		int iter_snapshots(PyObject *call, PyObject *data);
		#ifdef SWIG
		%feature("docstring") "Walk over the descendents of this dataset; for each dataset \"ds\", execute call(ds, data)."
		#endif
		int iter_root(PyObject *call, PyObject *data);
		int send(char *tosnap, PyObject *writeTo, char *fromsnap = NULL, bool verbose = false, bool replicate = false, bool doall = false, bool fromorigin = false, bool dedup = false, bool props = false, PyObject *callable = NULL, PyObject *callableArg = NULL);
		
		int receive(const char *tosnap, PyObject *readFrom, bool verbose = false, bool isprefix = false, bool istail = false, bool dryrun = false, bool force = false, bool canmountoff = false);
		zfs& zfs::operator=(const zfs &);
		zfs(z*, libzfs_handle_t *, zfs_handle_t *);
	private:
		void init(z*, const libzfs_handle_t *, zfs_handle_t *);
		int raw_prop_get(zfs_prop_t, char *, size_t, my_zprop_t *, char *, size_t, boolean_t);
		libzfs_handle_t *m_handle;
		zfs_handle_t *m_openfs;
		z *m_parent;
		enum iter_type { filesystems, children, dependents, snapshots, root };
		int zfs::generic_iter(PyObject*, PyObject*, iter_type, bool);
		friend class z;
};

#endif
