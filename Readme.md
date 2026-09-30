Project steps

1. Create a dummy disk file which will be used for testing the api implementaion of File system working and buffer cache. 
	'''dd if=/dev/zero of=dummy_disk.img bs=4096 count=2500''' 
	dd linux command for copying raw bytes from one place to other.
	/dev/zero is a linux device that continously produce zeros
	of= produces out file 
	bs= block size
	cout = number of blocks.
	
	our dummy_disk.img will have 2500 blocks of 4096 block size each toatl - 10MB data.
	
	'''
	stat dummy_disk.img 
	
	  File: dummy_disk.img
	  size: 10240000        Blocks: 20000      IO Block: 4096   regular file
	Device: 252,1   Inode: 8522221     Links: 1
	Access: (0664/-rw-rw-r--)  Uid: ( 1000/ tarunya)   Gid: ( 1000/ tarunya)
	Access: 2026-09-30 12:58:24.757291575 +0400
	Modify: 2026-09-30 12:58:24.767291680 +0400
	Change: 2026-09-30 12:58:24.767291680 +0400
	 Birth: 2026-09-30 12:58:24.757291575 +0400
	 
	 '''

	
