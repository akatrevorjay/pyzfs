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
		#ifdef SWIG
		%feature("docstring") "Create a data stream that represents"
"<the current dataset>@tosnap into the file-like object writeTo, which must have"
"a file descriptor."
"  * Set fromsnap to the name of another snapshot (which precedes tosnap) to"
"    generate an incremental snapshot: one that includes only data updated"
"    between the time fromsnap was created and the time tosnap was created."
"  * Set verbose to get a printed elapsed-time/rate message when the data stream"
"    is done being created.  This is like zfs send -v."
"  * Set replicate to generate a replication stream: one that includes all the"
"    descendents of this dataset.  This is like zfs send -R."
"  * Set doall to include intermediate snapshots in incremental streams.  That"
"    is, if you have a filesystem with snapshots one, two, and three, and you"
"    call send(\"three\", myfile, fromsnap=\"one\", doall=True) you'll end up"
"    with all three snapshots on the system where you receive the stream;"
"    without it you'll end up with only one and three."
"  * Set fromorigin if the dataset is a clone to generate an incremental"
"    snapshot from its origin."
"  * Set dedup to activate deduplication on the generated stream."
"  * Set props to send properties along with the stream.  This is like"
"    'zfs send -p'."
"  * Set callable and callableArg if you're doing a recursive send and you want"
"    to put only certain datasets into the stream.  callable will be called"
"    like \"callable(ds, callableArg)\" must return True or False to signify"
"    whether ds should be included in the stream."
// FIXME: Does a recursive snapshot with callable set pass only snapshots to it?  Other datasets?
		#endif
		int send(char *tosnap, PyObject *writeTo, char *fromsnap = NULL, bool verbose = false, bool replicate = false, bool doall = false, bool fromorigin = false, bool dedup = false, bool props = false, PyObject *callable = NULL, PyObject *callableArg = NULL);
		#ifdef SWIG
		%feature("docstring") "Receive a data stream from the file-like object readFrom into the snapshot tosnap.\n"
"  * Set verbose to print an elapsed-time/rate message when the data stream has"
"    been completely received.  This is like zfs recv -v."
"  * Set isprefix if the destination is a prefix, not the exact filesystem.  This"
"    is like zfs recv -d."
"  * Set dryrun if nothing should actually be written to the filesystem, just"
"    check if it could be done.  This is like zfs recv -n."
"  * Set force if filesystems should be rolled back (or destroyed!) as necessary."
"    This is like zfs recv -F."
"  * Set canmountoff if the received filesystems should have \"canmount=off\" set"
"    on them."
		#endif
		int receive(const char *tosnap, PyObject *readFrom, bool verbose = false, bool isprefix = false, bool istail = false, bool dryrun = false, bool force = false, bool canmountoff = false);
		#ifdef SWIG
		%feature("docstring") "Destroy this filesystem.  Set defer if it's not required that it go away right now."
		#endif
		int destroy(bool defer = false);
		#ifdef SWIG
		%feature("docstring") "Take a snapshot of this dataset; call it \"snapname\" and set properties as specified in \"props\".  If recursive is set, make a snapshot of all descendents as well."
		#endif
		int snapshot(const char *snapname, bool recursive = false, PyObject *props = NULL);
		zfs& operator=(const zfs &);
		zfs(z*, libzfs_handle_t *, zfs_handle_t *);
	private:
		void init(z*, const libzfs_handle_t *, zfs_handle_t *);
		int raw_prop_get(zfs_prop_t, char *, size_t, my_zprop_t *, char *, size_t, boolean_t);
		libzfs_handle_t *m_handle;
		zfs_handle_t *m_openfs;
		z *m_parent;
		enum iter_type { filesystems, children, dependents, snapshots, root };
		int generic_iter(PyObject*, PyObject*, iter_type, bool);
		friend class z;
};

#endif
