/*
 * yramfs_super.h
 *
 *  Created on: Mar 23, 2012
 *      Author: yunfei
 */

#ifndef _yramfs_super_h
#define _yramfs_super_h

#define YRAMFS_ROOT_INODE_NUMBER     0

typedef struct _yramfs_sb_info {
    uint32_t     nodeSerialNum;         /* total number of inode */
}yramfs_sb_info_t;


/*
 * @brief this file initialiazes the super block on mounting time
 *      with 's_op' and 's_root' initialized
 *
 * @param sb     super_block instance to be initialized
 * @param data   arbitrary mount options, usually comes as an ASCII string
 * @param silent whether or not to be silent on error
 *
 * @returns  error codes
 */
int yramfs_fill_super(struct super_block *sb, void *data, int silent);
#endif
