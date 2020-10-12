//
//  yramfs_vector.h
//  ramfs
//
//  Created by Yunfei Ma on 12-3-25.
//  Copyright (c) 2012 yunfei. All rights reserved.
//

#ifndef ramfs_yramfs_vector_h
#define ramfs_yramfs_vector_h

typedef void (*free_func_t)(void *data);

typedef struct _yramfs_vector_t yramfs_vector_t;

/*
 * @brief this function create a vector object
 * @param ppVector   out put new created object
 *
 * @returns error codes
 */
int yramfs_vector_create(yramfs_vector_t **ppVector);

/*
 * @brief this function adds a data and a cresponding free handler
 *
 * @param pVector  a vector instance
 * @param data     custom data
 * @param free     free function
 *
 * returns error codes
 */
int yramfs_vector_add(yramfs_vector_t *pVector,
                      uint32_t data,
                      free_func_t free);

/*
 * @brief this function removes a data and call cresponding free handler
 *      automatically if it was registered
 *
 * @param pVector  a vector instance
 * @param data     custom data
 *
 * returns error codes
 */
int yramfs_vector_remove_data(yramfs_vector_t *pVector, uint32_t data);

/*
 * @brief this function removes all data and call cresponding free handler
 *      automatically if it was registered
 *
 * @param pVector  a vector instance
 *
 * returns error codes
 */
int yramfs_vector_remove_all(yramfs_vector_t *pVector);

/*
 * @brief this function caculates the count of vector
 *
 * @param pVector  a vector instance
 * @param pCount   out put count value
 *
 * returns error codes
 */
int yramfs_vector_count(yramfs_vector_t *pVector, uint32_t *pCount);

/*
 * @brief this function retreive data from vector by index
 *
 * @param pVector  a vector instance
 * @param index    data index stored in vector
 * @param pdata    output data
 *
 * returns error codes
 */
int yramfs_vector_get_at(yramfs_vector_t *pVector,
                         uint32_t index, uint32_t *pdata);


#endif
