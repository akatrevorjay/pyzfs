#! /usr/site/bin/python
import ctypes
from ctypes import cdll
lib = cdll.LoadLibrary("libzfs.so")
class z:
	def __init__(self, spew_error=False):
		global handle
		handle = lib.libzfs_init()
		if bool(spew_error):
			lib.libzfs_print_on_error(handle, spew_error)
	def open_fs(self, name, type):
		return zfs(name, type)

class zfs:
	class types:
		filesystem = 1
		snapshot = 2
		volume = 4
		pool = 8
		all = filesystem | snapshot | volume
	class props:
		type = 0
		creation = 1
		used = 2
		available = 3
		referenced = 4
		compressratio = 5
		mounted = 6
		origin = 7
		quota = 8
	MAXNAMELEN = 256
	def __init__(self, name, type):
		global handle
		self.fs = lib.zfs_open(handle, name, type)
	def name(self):
		return ctypes.string_at(lib.zfs_get_name(self.fs))
	def type_zfs_prop_t(self):
		return lib.zfs_get_type(self.fs)
	def type(self):
		return ctypes.string_at(lib.zfs_type_to_name(lib.zfs_get_type(self.fs)))
	def origin(self):
		the_origin = ctypes.create_string_buffer(zfs.MAXNAMELEN)
		lib.zfs_prop_get(self.fs, zfs.props.origin, ctypes.byref(the_origin), zfs.MAXNAMELEN, None, None, 0, ctypes.c_int(False))
		return the_origin.value
	def quota(self):
		return lib.zfs_prop_get_int(self.fs, zfs.props.quota)
