%rename(assign) *::operator=;
#define _XOPEN_SOURCE_EXTENDED 1
%ignore zfs_prop_string_to_index(zfs_prop_t, const char *, uint64_t *);
%ignore zfs_prop_index_to_string(zfs_prop_t, uint64_t, const char **);
%ignore zfs_prop_setonce(zfs_prop_t);
%ignore zpool_prop_default_string(zpool_prop_t);
%ignore zpool_prop_default_numeric(zpool_prop_t);
%ignore zpool_prop_index_to_string(zpool_prop_t, uint64_t, const char **);
%ignore zpool_prop_string_to_index(zpool_prop_t, const char *, uint64_t *);

%inline %{
int my_eval(zfs_handle_t *child, void *data)
{
	DEBUG(printf("%s\n", zfs_get_name(child)););
	PyObject *dict = (PyObject*)data;
	PyObject *func = PyDict_GetItemString(dict, "function");
	PyObject *parent = PyDict_GetItemString(dict, "parent");
	z* m_parent = (z*)PyCObject_AsVoidPtr(parent);
	PyObject *handle = PyDict_GetItemString(dict, "zfs_handle");
	libzfs_handle_t *m_handle = (libzfs_handle_t*)PyCObject_AsVoidPtr(handle);
	PyObject *realdata = PyDict_GetItemString(dict, "data");
	PyObject *resultobj;
	
	zfs the_child = zfs(m_parent, m_handle, child);
	DEBUG(printf("Start function call:\n"););
	resultobj = SWIG_NewPointerObj(SWIG_as_voidptr(&the_child), SWIGTYPE_p_zfs, 0 );
	PyEval_CallObject(func, Py_BuildValue("(OO)", resultobj, realdata));
	int rval = (resultobj == Py_True);
	Py_XDECREF(resultobj);
	if (PyErr_Occurred() != NULL)
	{
		printf("An error occured from Python: ");
		PyErr_Print();
	}
	DEBUG(printf("Done calling function\n"););
	return rval;
}
%}

%include </usr/include/sys/fs/zfs.h>
%exception {
	try {
		$function
	}
	catch (Exception e) {
		PyErr_SetString(PyExc_RuntimeError,e.error());
		return NULL;
	}
}

%include "code.h"
