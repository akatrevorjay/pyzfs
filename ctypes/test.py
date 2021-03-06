#! /usr/site/bin/python
import sys
import pyzfs
fsname=str(sys.argv[1])
a=pyzfs.z(True) # argument is "should I spew debug"

def test_one(fsname):
	print " Python starting test one: open filesystems"
	for fs in [fsname, 'rpool', 'filesystemthatdoesntexist']:
		print "=================================="
		print " Python: Opening fs \"%s\"" % (fs)
		try:
			print dir(pyzfs.zfs.types)
			b=a.open_fs(fs, pyzfs.zfs.types.filesystem | pyzfs.zfs.types.snapshot | pyzfs.zfs.types.volume)
			print " Python: %s is of type %s" % (b.name(), b.type())
			print " Python: Quota for %s is %s" % (b.name(), bytestonice(b.quota()))
			print " Python: Origin for %s is %s" % (b.name(), b.origin())
			print " Python: Mounted for %s is %s" % (b.name(), b.mounted())
			print " Python: Compressratio for %s is %s" % (b.name(), b.compressratio())
			print " Python: Referenced for %s is %s" % (b.name(), bytestonice(b.referenced()))
			print " Python: Available for %s is %s" % (b.name(), bytestonice(b.available()))
			print " Python: Used for %s is %s" % (b.name(), bytestonice(b.used()))
			print " Python: Creation time for %s is %s" % (b.name(), b.creation())
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
	print dir(x)
	print dir(data)
	print x.__class__
	print data.__class__
	print "%s %s" % (x.name(), bytestonice(x.quota()))

print " Python: Starting tests on %s" % (fsname)
#test_one(fsname)
#test_two(fsname)
fs = a.open_fs(fsname, pyzfs.zfs.types.filesystem | pyzfs.zfs.types.snapshot | pyzfs.zfs.types.volume)
pool = a.open_pool(fsname)
print pool.status()
#a= fs.iter_children(fn)
print " Python: Done!"
