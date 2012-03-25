/*
 * yramfs_file.h
 *
 *  Created on: Mar 23, 2012
 *      Author: yunfei
 *
 *  This file contains some common public definitions
 */

#ifndef _yramfs_common_h
#define _yramfs_common_h
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pagemap.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/blkdev.h>
#include <linux/slab.h>
#include <linux/cramfs_fs_sb.h>
#include <linux/buffer_head.h>
#include <linux/vfs.h>
#include <linux/mutex.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/magic.h>
#include <linux/kernel.h>

#include <asm/atomic.h>

#include <asm/uaccess.h>

#include "yramfs_config.h"
#include "yramfs_utils.h"



#endif /* yramfs_common_h_ */
