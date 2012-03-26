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
    char            name[YRAMFS_MAX_PATH_LEN];
    unsigned char   ftype;
    ino_t           ino; // inode serial number
}_yramfs_dir_info_t;

#endif /* yramfs_dir_h_ */