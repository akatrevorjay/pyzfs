#include "zfs.h"
bool _prop_readonly(zfs_prop_t prop) { return zfs_prop_readonly(prop); }

void zfs::init(z *p, const libzfs_handle_t *h, zfs_handle_t *fs)
{
	DEBUG(printf("Creating zfs instance at %p with %p and %p\n", this, h, fs););
	m_parent = p;
	m_handle = (libzfs_handle_t*)h;
	m_openfs = fs;
	DEBUG(printf("Instantiated at %p: is %s and name is %s\n", this,  this->type_string(), this->name()););
}

zfs& zfs::operator=(const zfs &other)
{
	DEBUG(printf("zfs operator= from %p to %p\n", &other, this););
	printstack(fileno(stdout));
	init(other.m_parent,
			other.m_handle,
			other.m_parent->raw_open_fs(other.name(), other.type()));
	return *this;
}
zfs::zfs(z *p, libzfs_handle_t *h, zfs_handle_t *fs)
{
	DEBUG(printf("zfs plain create at %p\n", this);)
	init(p, h, fs);
}
const char *zfs::name(void) const
{
	const char *temp;
	DEBUG(printf("Getting name from %p: ", this);)
	temp = zfs_get_name(m_openfs);
	DEBUG(printf("Got \"%s\"\n", temp);)
	return temp;
}
zfs_type_t zfs::type(void) const
{
	DEBUG(printf("Getting type from %p\n", this);)
	return zfs_get_type(m_openfs);
}
const char *zfs::type_string(void) const
{
	return zfs_type_to_name(this->type());
}
int zfs::raw_prop_get(zfs_prop_t whatprop, char *output, size_t outlen, my_zprop_t *sourcetype, char *statbuf, size_t statlen, boolean_t literal)
{
	int error;
	error = zfs_prop_get(m_openfs, whatprop, output, outlen, sourcetype, statbuf, statlen, literal);
	return error;
}
PyObject *zfs::prop_get_int(zfs_prop_t prop)
{
	return PyLong_FromUnsignedLongLong((unsigned long long)zfs_prop_get_int(m_openfs, prop));
}
void zfs::prop_set(char *prop, char *val) {
	zfs_prop_set(m_openfs, prop, val);
}

EXTERN int my_eval(zfs_handle_t *child, void *data);

int zfs::generic_iter(PyObject *call, PyObject *data, iter_type type, bool recurse)
// Note: recurse is only used for the "dependents" mode
{
	DEBUG(printf("generic_iter at %p, Function is at %p, data is %p\n", this, call, data););
	PyObject *foo = Py_BuildValue("{sOsOsO}",
					"function", call,
					"parent", PyCObject_FromVoidPtr(m_parent, NULL),
					"zfs_handle", PyCObject_FromVoidPtr(m_handle, NULL));
	if (data != NULL)
		PyDict_SetItemString(foo, "data", data);
	switch(type)
	{
		case filesystems:	return zfs_iter_filesystems(m_openfs, my_eval, foo); break;
		case snapshots:		return zfs_iter_snapshots(m_openfs, my_eval, foo); break;
		case dependents:	return zfs_iter_dependents(m_openfs, (boolean_t)recurse, my_eval, foo); break;
		case children:		return zfs_iter_children(m_openfs, my_eval, foo); break;
		case root:		return zfs_iter_root(m_handle, my_eval, foo); break;
	};
	return -1;
}
int zfs::iter_filesystems(PyObject *call, PyObject *data) { return generic_iter(call, data, filesystems, false); }
int zfs::iter_snapshots(PyObject *call, PyObject *data) { return generic_iter(call, data, snapshots, false); }
int zfs::iter_dependents(PyObject *call, PyObject *data, bool recurse) { return generic_iter(call, data, dependents, recurse); }
int zfs::iter_children(PyObject *call, PyObject *data) { return generic_iter(call, data, children, false); }
int zfs::iter_root(PyObject *call, PyObject *data) { return generic_iter(call, data, root, false); }

int zfs::send(char *tosnap, PyObject *writeTo, char *fromsnap, bool verbose, bool replicate, bool doall, bool fromorigin, bool dedup, bool props, PyObject *callable, PyObject *callableArg)
{
	if (tosnap == NULL)
		throw Exception(PyExc_ValueError, "tosnap may not be NULL");
	int rval = -1;
	PyObject *foo = Py_BuildValue("{sOsO}",
					"parent", PyCObject_FromVoidPtr(m_parent, NULL),
					"zfs_handle", PyCObject_FromVoidPtr(m_handle, NULL));
	if (PyErr_Occurred()) {
		DEBUG(printf("Couldn't build dictionary object\n"));
		return -1;
	}
	if (callable != NULL) 
		PyDict_SetItemString(foo, "function", callable);
	if (callableArg != NULL)
		PyDict_SetItemString(foo, "data", callableArg);
	int outfd = PyObject_AsFileDescriptor(writeTo);
	DEBUG(printf("Got FD %d from file object %p\n", outfd, writeTo));
	if (outfd == -1) {
		throw Exception(PyExc_TypeError, "Couldn't get file descriptor from writeTo");
	}
	#ifdef HAVE_SENDFLAGS_T
	sendflags_t flags;
	flags.verbose = verbose;
	flags.replicate = replicate;
	flags.doall = doall;
	flags.fromorigin = fromorigin;
	flags.dedup = dedup;
	flags.props = props;
	rval = zfs_send(m_openfs, fromsnap, tosnap, flags, outfd, my_eval, foo);
	#else
	rval = zfs_send(m_openfs, fromsnap, tosnap, (boolean_t)replicate, (boolean_t)doall, (boolean_t)fromorigin, (boolean_t)verbose, outfd);
	#endif
	
	DEBUG(printf("zfs_send returned %d\n", rval));
	if (rval != 0) {
		throw Exception(PyExc_RuntimeError, std::string(libzfs_error_action(m_handle)) + ": " + libzfs_error_description(m_handle));
	}
	return rval;
}

int zfs::receive(const char *tosnap, PyObject *readFrom, bool verbose, bool isprefix, bool istail, bool dryrun, bool force, bool canmountoff)
{
	int rval = -1;
	recvflags_t flags;
	flags.verbose = verbose;
	flags.isprefix = isprefix;
	#ifdef HAVE_RECVFLAGS_T_ISTAIL
	flags.istail = istail;
	#endif
	flags.dryrun = dryrun;
	flags.force = force;
	flags.canmountoff = canmountoff;
	
	int fromFD = PyObject_AsFileDescriptor(readFrom);
	DEBUG(printf("Got FD %d from file object %p\n", fromFD, readFrom));
	if (fromFD == -1) {
		throw Exception(PyExc_TypeError, "Couldn't get file descriptor from readFrom");
	}
	rval = zfs_receive(m_handle, tosnap, flags, fromFD, NULL);
	if (rval != 0) {
		throw Exception(PyExc_RuntimeError, std::string(libzfs_error_action(m_handle)) + ": " + libzfs_error_description(m_handle));
	}
	return rval;
}

int zfs::destroy(bool defer)
{
	return zfs_destroy(m_openfs, (boolean_t)defer);
}
zfs::~zfs()
{
	DEBUG(printf("Destroying zfs instance at %p\n", this););
	if (m_openfs != NULL)
	{
		DEBUG(printf("Closing handle at %p\n", m_openfs););
		zfs_close(m_openfs);
		DEBUG(printf("Closed handle, %p\n", m_openfs););
	}
	else
		return;
	m_openfs = NULL;
	DEBUG(printf("Done destroying %p\n", this););
}
