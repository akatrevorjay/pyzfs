#ifndef Z_H
#define Z_H

#include "core.h"
#include "zfs.h"
#include "zpool.h"

#ifdef DO_DEBUG
#define SPEW_BY_DEFAULT true
#else
#define SPEW_BY_DEFAULT false
#endif

class z
{
	public:
		#ifdef SWIG
		%feature("docstring") "Create an object that knows how to open ZFS datasets and pools.  Optionally, specify whether to print debugging information (the default depends on a compile-time setting)."
		#endif
		z(bool spew_error);
		#ifdef SWIG
		%feature("docstring") "Open a ZFS dataset called \"name\".  Optionally, specify the type of dataset to open: ZFS_TYPE_FILESYSTEM, ZFS_TYPE_SNAPSHOT, ZFS_TYPE_VOLUME, or some combination (using bitwise or, |, to combine them)."
		#endif
		zfs *open_fs(const char *name, int type = ZFS_TYPE_FILESYSTEM | ZFS_TYPE_SNAPSHOT | ZFS_TYPE_VOLUME);
		#ifdef SWIG
		%feature("docstring") "Open a zpool called \"name\"."
		#endif
		zpool *open_pool(const char *name);
		~z();
	private:
		void init(bool);
		libzfs_handle_t *m_handle;
		zfs_handle_t* raw_open_fs(const char *name, int);
		friend class zfs;
};

#endif
