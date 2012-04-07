/*
 * yramfs_file.h
 *
 *  Created on: Mar 23, 2012
 *      Author: yunfei
 */

#ifndef _yramfs_file_h
#define _yramfs_file_h



typedef struct _yramfs_file_info_t{
    struct dentry  *entry;
    unsigned char  *pContent;    // a buffer holds the content size
    uint32_t        contentSize; // indicates the buffer size of 'pContent'
}yramfs_file_info_t;


#ifndef _yramfs_file_c
extern const struct inode_operations yramfs_file_inode_operations;
extern const struct file_operations yramfs_file_operations;
extern const struct address_space_operations yramfs_file_addr_operations;
#endif

#endif /* yramfs_file_h_ */
