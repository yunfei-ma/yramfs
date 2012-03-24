#ifndef _yramfs_super_h
#define _yramfs_super_h

#define YRAMFS_ROOT_INODE_NUMBER     1

typedef struct yramfs_sb_info {
	mode_t       mode;		    /* Mount mode for root directory */
    uint32_t     nodeNum;       /* total number of inode */
}yramfs_sb_info;

#endif
