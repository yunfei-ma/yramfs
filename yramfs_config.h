#ifndef _yramfs_config_h
#define _yramfs_config_h

#define  YRAMFS_MAGIC                          0x20120321

#define  YRAMFS_CONFIG_CACHE_NUM_MAX           6


#define  YRAMFS_CONFIG_INODE_DATA_BLOCKS       256

#define  YRAMFS_CONFIG_MAX_FILESIZE            (YRAMFS_CONFIG_INODE_DATA_BLOCKS *\
		                                        YRAMFS_CONFIG_BLOCK_SIZE)

#define  YRAMFS_CONFIG_BLOCK_SIZE_BITS         32

#endif
