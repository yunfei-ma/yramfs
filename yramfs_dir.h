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

typedef struct _yramfs_dir_private_t{
    yramfs_vector_t     *pVectorEntries;
}yramfs_dir_private_t;


#endif /* yramfs_dir_h_ */