import sys
if len(sys.argv) > 2:
	if sys.argv[2] == "debug":
		import pyzfs_debug
		pyzfs=pyzfs_debug
else:
	import pyzfs
fsname=str(sys.argv[1])
a=pyzfs.z(True) # argument is "should I spew debug"

def test_one(fsname):
	print " Python starting test one: open filesystems"
	for fs in [fsname, 'filesystemthatdoesntexist']:
		print "=================================="
		print " Python: Opening fs \"%s\"" % (fs)
		try:
			b=a.open_fs(fs, pyzfs.ZFS_TYPE_FILESYSTEM | pyzfs.ZFS_TYPE_SNAPSHOT | pyzfs.ZFS_TYPE_VOLUME)
			print " Python: %s is of type %s" % (b.name(), b.type_string())
			print " Python: Quota for %s is %s" % (b.name(), bytestonice(b.prop_get_int(pyzfs.ZFS_PROP_QUOTA)))
			del(b)
		except RuntimeError, e:
			print " Python error raised: %s" % e
def test_two(poolname):
	print " Python starting test two: open pools"
	for pool in [poolname, 'poolthatdoesntexist']:
		print "=================================="
		print " Python: Opening pool \"%s\"" % (pool)
		try:
			b=a.open_pool(pool)
			print " Python: opened pool \"%s\"" % (b.name())
			del(b)
		except RuntimeError, e:
			print " Python error raised: %s" % e
def bytestonice(size):
	units = ['B', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB', 'EiB', 'ZiB', 'YiB']
	unit = 0
	nicesize = float(size)
	if nicesize == -1.0:
		return "none"
	while (nicesize > 1024):
		unit = unit + 1
		nicesize = nicesize / 1024
	return "%.3f%s" % (nicesize, units[unit])
def fn(x, data):
	print "%s %s" % (x.name(), bytestonice(x.prop_get_int(pyzfs.ZFS_PROP_QUOTA)))

print " Python: Starting tests on %s" % (fsname)
test_one(fsname)
test_two(fsname)
fs = a.open_fs(fsname, pyzfs.ZFS_TYPE_FILESYSTEM | pyzfs.ZFS_TYPE_SNAPSHOT | pyzfs.ZFS_TYPE_VOLUME)
fs.iter_filesystems(fn, None)
print " Python: Done!"
