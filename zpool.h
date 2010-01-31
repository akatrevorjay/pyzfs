#ifndef ZPOOL_H
#define ZPOOL_H

#include "core.h"
#include "z.h"

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
