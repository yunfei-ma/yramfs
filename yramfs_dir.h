/*
 * yramfs_dir.h
 *
 *  Created on: Mar 23, 2012
 *      Author: yunfei
 */

#ifndef _yramfs_dir_h
#define _yramfs_dir_h
#include "yramfs_common.h"
#include "yramfs_vector.h"

typedef struct _yramfs_dir_info_t{
    struct dentry  *entry;
    unsigned char   ftype;
}yramfs_dir_info_t;

/*
 * @brief this function add a path to a dir's vector module
 *
 * @param pVector  pointer to an instance of yramfs_vector_t
 * @param path     new path to be added
 * @param type     type of this path, currently only file, folder is supported
 *
 * @returns error codes
 */
int yramfs_dir_add_path(struct inode *dir, struct dentry  *entry,
                                unsigned char type);

#endif /* yramfs_dir_h_ */
