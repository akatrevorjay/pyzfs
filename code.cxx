#include "code.h"
bool _prop_readonly(zfs_prop_t prop) { return zfs_prop_readonly(prop); }

z::z()
{
#ifdef DO_DEBUG
	init(true);
#else
	init(false);
#endif
}
z::z(bool spew_error)
{
	init(spew_error);
}
void z::init(bool spew_error)
{
	DEBUG(printf("Creating z instance at %p, spewerror is %s\n", this, (spew_error ? "true" : "false")););
	m_handle = libzfs_init();
	libzfs_print_on_error(m_handle, (boolean_t)spew_error);
}
zfs_handle_t* z::raw_open_fs(const char *name, zfs_type_t type)
{
	if (name == NULL) {
		throw Exception(PyExc_ValueError, "NULL passed as name");
	}
	DEBUG(printf("Opening raw fs for z instance at %p: %p, \"%p\", %d\n", this, m_handle, name, type););
	zfs_handle_t *fs = zfs_open(m_handle, name, type);
	if (libzfs_errno(m_handle) != 0)
	{
		fs = NULL;
		DEBUG(printf("Whoops, open_fs died\n"););
		std::string action = std::string(libzfs_error_action(m_handle));
		std::string desc = std::string(libzfs_error_description(m_handle));
		libzfs_fini(m_handle);
		m_handle = libzfs_init();
		throw Exception(PyExc_RuntimeError, action + ": " + desc);
	}
	return fs;
}

zfs *z::open_fs(const char *name)
{
	return open_fs(name, ZFS_TYPE_FILESYSTEM);
}
zfs *z::open_fs(const char *name, zfs_type_t type)
{
	DEBUG(printf("Opening cooked fs for z instance at %p: %p, \"%s\", %d\n", this, m_handle, name, type););
	zfs_handle_t *openfs = raw_open_fs(name, type);
	DEBUG(printf("Cooked open got %p\n", openfs););
	return new zfs(this, m_handle, openfs);
}
zpool *z::open_pool(const char *name)
{
	if (name == NULL) {
		throw Exception(PyExc_ValueError, "NULL passed as name");
	}
	zpool_handle_t *openpool = zpool_open(m_handle, name);
	if (libzfs_errno(m_handle) != 0)
	{
		DEBUG(printf("Whoops, open_pool died\n"););
		std::string action = std::string(libzfs_error_action(m_handle));
		std::string desc = std::string(libzfs_error_description(m_handle));
		libzfs_fini(m_handle);
		m_handle = libzfs_init();
		throw Exception(PyExc_RuntimeError, action + ": " + desc);
	}
	return new zpool(m_handle, openpool);
}
z::~z()
{
	libzfs_fini(m_handle);
}

void zfs::init(z *p, const libzfs_handle_t *h, zfs_handle_t *fs)
{
	DEBUG(printf("Creating zfs instance at %p with %p and %p\n", this, h, fs););
	m_parent = p;
	m_handle = (libzfs_handle_t*)h;
	m_openfs = fs;
	DEBUG(printf("Instantiated at %p: is %s and name is %s\n", this,  this->type_string(), this->name()););
}
zfs::zfs(const zfs &other)
{
	DEBUG(printf("zfs copycon from %p to %p\n", &other, this););
	*this = other;
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
// 	printf(" C++ is calling python with value %p\n");
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
		case root:		return zfs_iter_root(m_handle, my_eval, data); break;
	};
	return -1;
}
int zfs::iter_filesystems(PyObject *call, PyObject *data) { return generic_iter(call, data, filesystems, false); }
int zfs::iter_snapshots(PyObject *call, PyObject *data) { return generic_iter(call, data, snapshots, false); }
int zfs::iter_dependents(PyObject *call, PyObject *data, bool recurse) { return generic_iter(call, data, dependents, recurse); }
int zfs::iter_children(PyObject *call, PyObject *data) { return generic_iter(call, data, children, false); }
int zfs::iter_root(PyObject *call, PyObject *data) { return generic_iter(call, data, root, false); }


int zfs::send(char *snap, PyObject *writeTo, PyObject *kwargs)
{
	DEBUG(printf("Entering short zfs::send\n"));
	typedef struct {
		char *name;
		bool *loc;
	} boolPropEntry;
	bool verbose = false, replicate = false, doall = false, fromorigin = false, dedup = false, props = false;
	boolPropEntry properties[] = {
		PROP_PAIR(verbose),
		PROP_PAIR(replicate),
		PROP_PAIR(doall),
		PROP_PAIR(fromorigin),
		PROP_PAIR(dedup),
		PROP_PAIR(props),
	};
	PyObject *callable = NULL, *callableArg = NULL;
	char *fromsnap = NULL;
	for (unsigned int i = 0; i < sizeof(properties) / sizeof(boolPropEntry); ++i)
	{
		if (PyMapping_HasKeyString(kwargs, properties[i].name))
		{
			PyObject *value = PyMapping_GetItemString(kwargs, properties[i].name);
			if (!PyBool_Check(value)) {
				throw Exception(PyExc_ValueError, std::string("Value for ") + properties[i].name + " must be boolean");
			}
			*(properties[i].loc) = (value == Py_True);
			if (*properties[i].loc)
				DEBUG(printf("Setting %s to True\n", properties[i].name));
			else
				DEBUG(printf("Setting %s to False\n", properties[i].name));
		}
	}
	DEBUG(printf("calling real send()\n"));
	return send(fromsnap, snap, writeTo, verbose, replicate, doall, fromorigin, dedup, props, callable, callableArg);
}

int zfs::send(char *fromsnap, char *tosnap, PyObject *writeTo, bool verbose, bool replicate, bool doall, bool fromorigin, bool dedup, bool props, PyObject *callable, PyObject *callableArg)
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

int zfs::receive(const char *tosnap, int fromFD, PyObject *kwargs)
{
	DEBUG(printf("Entering short zfs::receive\n"));
	typedef struct {
		char *name;
		bool *loc;
	} boolPropEntry;
	bool verbose = false, isprefix = false, istail = false, dryrun = false, force = false, canmountoff = false;
	boolPropEntry properties[] = {
		PROP_PAIR(verbose),
		PROP_PAIR(isprefix),
		PROP_PAIR(istail),
		PROP_PAIR(dryrun),
		PROP_PAIR(force),
		PROP_PAIR(canmountoff),
	};
	for (unsigned int i = 0; i < sizeof(properties) / sizeof(boolPropEntry); ++i)
	{
		if (PyMapping_HasKeyString(kwargs, properties[i].name))
		{
			PyObject *value = PyMapping_GetItemString(kwargs, properties[i].name);
			if (!PyBool_Check(value)) {
				throw Exception(PyExc_ValueError, std::string("Value for ") + properties[i].name + " must be boolean");
			}
			*(properties[i].loc) = (value == Py_True);
			if (*properties[i].loc)
				DEBUG(printf("Setting %s to True\n", properties[i].name));
			else
				DEBUG(printf("Setting %s to False\n", properties[i].name));
		}
	}
	DEBUG(printf("calling real receive()\n"));
	return receive(tosnap, fromFD, verbose, isprefix, istail, dryrun, force, canmountoff);
}

int zfs::receive(const char *tosnap, int fromFD, bool verbose, bool isprefix, bool istail, bool dryrun, bool force, bool canmountoff)
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
	rval = zfs_receive(m_handle, tosnap, flags, fromFD, NULL);
	return rval;
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


zpool::zpool(const zpool &other)
{
	m_handle = other.m_handle;
	m_openpool = zpool_open(m_handle, other.name());
}
zpool::zpool(libzfs_handle_t *h, zpool_handle_t *p)
{
	m_handle = h;
	m_openpool = p;
}
zpool::~zpool()
{
	if (m_openpool != NULL)
		zpool_close(m_openpool);
	m_openpool = NULL;
}
const char *zpool::name() const
{
	return zpool_get_name(m_openpool);
}
void zpool::scrub(const pool_scrub_type_t type) const
{
	zpool_scrub(m_openpool, type);
}
