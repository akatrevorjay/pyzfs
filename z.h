#ifndef Z_H
#define Z_H

#include "core.h"
#include "zfs.h"
#include "zpool.h"

class z
{
	public:
		z();
		z(bool);
		zfs *open_fs(const char *name);
		zfs *open_fs(const char *name, int);
		zpool *open_pool(const char *name);
		~z();
	private:
		void init(bool);
		libzfs_handle_t *m_handle;
		zfs_handle_t* raw_open_fs(const char *name, int);
		friend class zfs;
};

#endif
