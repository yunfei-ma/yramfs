#ifndef _yramfs_super_h
#define _yramfs_super_h

#define YRAMFS_ROOT_INODE_NUMBER     1

typedef struct yramfs_sb_info {
	mode_t       mode;		    /* Mount mode for root directory */
    uint32_t     nodeNum;       /* total number of inode */
}yramfs_sb_info;


/*
 * @brief this file initialiazes the super block on mounting time
 *      with 's_op' and 's_root' initialized
 *
 * @param sb    super_block instance to be initialized
 * @param data  
 * @param silent
 */
int yramfs_fill_super(struct super_block *sb, void *data, int silent);
#endif
