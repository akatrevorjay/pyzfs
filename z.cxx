#include "config.h"
#include "z.h"

z::z(bool spew_error = SPEW_BY_DEFAULT)
{
	init(spew_error);
}
void z::init(bool spew_error)
{
	DEBUG(0, printf("Creating z instance at %p, spewerror is %s\n", this, (spew_error ? "true" : "false")););
	m_handle = libzfs_init();
	libzfs_print_on_error(m_handle, (boolean_t)spew_error);
}
zfs_handle_t* z::raw_open_fs(const char *name, int type)
{
	if (name == NULL) {
		throw Exception(PyExc_ValueError, "NULL passed as name");
	}
	DEBUG(0, printf("Opening raw fs for z instance at %p: %p, \"%p\", %d\n", this, m_handle, name, type););
	zfs_handle_t *fs = zfs_open(m_handle, name, type);
	if (libzfs_errno(m_handle) != 0)
	{
		fs = NULL;
		DEBUG(10, printf("Whoops, open_fs died\n"););
		std::string action = std::string(libzfs_error_action(m_handle));
		std::string desc = std::string(libzfs_error_description(m_handle));
		libzfs_fini(m_handle);
		m_handle = libzfs_init();
		throw Exception(PyExc_RuntimeError, action + ": " + desc);
	}
	return fs;
}

zfs *z::open_fs(const char *name, int type)
{
	DEBUG(0, printf("Opening cooked fs for z instance at %p: %p, \"%s\", %d\n", this, m_handle, name, type););
	zfs_handle_t *openfs = raw_open_fs(name, type);
	DEBUG(5, printf("Cooked open got %p\n", openfs););
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
		DEBUG(30, printf("Whoops, open_pool died\n"););
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
