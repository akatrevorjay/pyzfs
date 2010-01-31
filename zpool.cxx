#include "zpool.h"

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
