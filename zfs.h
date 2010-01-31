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
		zfs() {}
		zfs(const zfs&);
		~zfs();
		const char *name() const;
		zfs_type_t type() const;
		const char *type_string(void) const;
		int raw_prop_get(zfs_prop_t, char *, size_t, my_zprop_t *, char *, size_t, boolean_t);
		PyObject *prop_get_int(zfs_prop_t);
		void prop_set(char *prop, char *val);
		zfs& zfs::operator=(const zfs &);
		enum iter_type { filesystems, children, dependents, snapshots, root };
		int iter_filesystems(PyObject *, PyObject *);
		int iter_children(PyObject *, PyObject *);
		int iter_dependents(PyObject *, PyObject *, bool);
		int iter_snapshots(PyObject *, PyObject *);
		int iter_root(PyObject *, PyObject *);
		int send(char *snap, PyObject *writeTo, PyObject *kwargs);
		int send(char *fromsnap, char *tosnap, PyObject *writeTo, bool verbose = false, bool replicate = false, bool doall = false, bool fromorigin = false, bool dedup = false, bool props = false, PyObject *callable = NULL, PyObject *callableArg = NULL);
		int receive(const char *tosnap, PyObject *readFrom, PyObject *kwargs);
		int receive(const char *tosnap, PyObject *readFrom, bool verbose, bool isprefix, bool istail, bool dryrun, bool force, bool canmountoff);
		zfs(z*, libzfs_handle_t *, zfs_handle_t *);
	private:
		void init(z*, const libzfs_handle_t *, zfs_handle_t *);
		libzfs_handle_t *m_handle;
		zfs_handle_t *m_openfs;
		z *m_parent;
		int zfs::generic_iter(PyObject*, PyObject*, iter_type, bool);
		friend class z;
};

#endif
