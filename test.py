# -*- coding: utf-8 -*-
import sys
import time
import unittest
import tempfile

Debug = False
if Debug:
	import pyzfs_debug
	pyzfs=pyzfs_debug
else:
	import pyzfs

class FsTest(unittest.TestCase):
	existing_fs = None
	existing_snap = None
	def setUp(self):
		self.z = pyzfs.z(False) # argument is "should I spew debug"
		self.outfile = tempfile.NamedTemporaryFile()
	def testOpenNull(self):
		self.assertRaises(ValueError, self.z.open_fs, None)
	def testOpenExisting(self):
		self.z.open_fs(self.existing_fs)
	def testOpenNonExisting(self):
		self.assertRaises(RuntimeError, self.z.open_fs, "filesystemthatdoesntexist")
	def testGetType(self):
		fs = self.z.open_fs(self.existing_fs)
		self.assert_(fs.type_string() in ["filesystem", "snapshot", "volume"])
	def testGetQuota(self):
		fs = self.z.open_fs(self.existing_fs)
		fs.prop_get_int(pyzfs.ZFS_PROP_QUOTA)
	def testIter(self):
		# Something that we'll pass back and forth to C; does it make the trip?
		magic = 'magic'
		def fn(x, data):
			s = data[0]
			s.assert_(data[1] == magic)
		fs = self.z.open_fs(self.existing_fs)
		fs.iter_filesystems(fn, [self, magic])
	def testSendNull(self):
		fs = self.z.open_fs(self.existing_fs)
		self.assertRaises(ValueError, fs.send, None, 0, {})
	def testSendBadArg(self):
		fs = self.z.open_fs(self.existing_fs)
		self.assertRaises(ValueError, fs.send, None, 0, {"verbose": "foo"})
	def testSendGoodArg(self):
		fs = self.z.open_fs(self.existing_fs)
		fs.send(self.existing_snap, self.outfile, {})
		self.assert_(self.outfile.tell() != 0)
		self.outfile.seek(0)

class PoolTest(unittest.TestCase):
	existing_pool = None
	def setUp(self):
		self.z = pyzfs.z(False)
	def testOpenNull(self):
		self.assertRaises(ValueError, self.z.open_pool, None)
	def testOpenExisting(self):
		pool = self.z.open_pool(self.existing_pool)
	def testOpenNonExisting(self):
		self.assertRaises(RuntimeError, self.z.open_pool, 'poolthatdoesntexist')



#print " Python: Trying a zfs send from %s" % (snapname)
#fs = a.open_fs("rpool/tiny", pyzfs.ZFS_TYPE_FILESYSTEM)
#output = open("/tmp/tiny-b", "w")
#try:
#  fs.send(None, "now", [], True, False, False, False, False, False, None, None)
#except:
#  print " Python: Got an exception!"
#output.close()



if __name__ == "__main__":
	FsTest.existing_fs = "rpool/tiny"
	FsTest.existing_snap = "now"
	PoolTest.existing_pool = "huge"
	for testCase in [FsTest]: #, PoolTest]:
		suite = unittest.TestLoader().loadTestsFromTestCase(testCase)
		unittest.TextTestRunner(verbosity=2).run(suite)
