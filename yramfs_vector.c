#include "yramfs_common.h"
#include "yramfs_vector.h"

/*
 * @brief this function create a vector object
 * @param ppVector   out put new created object
 *
 * @returns error codes
 */
int yramfs_vector_create(yramfs_vector_t **ppVector)
{
    yramfs_vector_t    *pVector = NULL;

    pVector = (yramfs_vector_t*)kmalloc(sizeof(yramfs_vector_t),
                                             GFP_KERNEL);
    if (NULL == pVector) {
        DBG_PRINT("malloc failed\n");
        return ENOMEM;
    }

    pVector->head = NULL;
    pVector->tail = NULL;
    pVector->count = 0;
    *ppVector = pVector;

    return 0;
}

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
                     free_func_t free)
{
    yramfs_vector_link_t  *plink = NULL;

    plink = (yramfs_vector_link_t*)kmalloc(sizeof(yramfs_vector_link_t),
                                           GFP_KERNEL);
    if(NULL == plink) {
        return ENOMEM;
    }

    plink->next = NULL;
    plink->data = data;
    plink->free = free;

    // add first plink
    if((pVector->head == pVector->tail)
               && (pVector->head == NULL)) {
        pVector->head = plink;
        pVector->tail = plink;
        pVector->count = 1;
        return 0;
    }

    // add plink to tail, and update new tail
    pVector->tail->next = plink;
    pVector->tail = plink;
    pVector->count ++;
    return 0;
}

/*
 * @brief this function removes a data and call cresponding free handler
 *      automatically if it was registered
 *
 * @param pVector  a vector instance
 * @param data     custom data
 *
 * returns error codes
 */
int yramfs_vector_remove_data(yramfs_vector_t *pVector, uint32_t data)
{
   yramfs_vector_link_t *plink = NULL, *plinkPre = NULL;

   if (NULL == pVector) {
        return -1;
   }

    plink = pVector->head;
    while(plink){
        // find the link
        if (data == plink->data){
            // update head if link is head
            if(plink == pVector->head) {
                pVector->head = plink->next;
            }
            // update tail if the link is tail
            if (plink == pVector->tail) {
                pVector->tail = plinkPre;
            }
            // update previous link if the link has previous
            if (plinkPre) {
                plinkPre->next = plink->next;
            }
            // free the link
            if (plink->free) {
                plink->free((void*)plink->data);
            }
            kfree(plink);
            pVector->count --;
        } else {
            // update previous link
            plinkPre = plink;
        }
        plink = plink->next;
    }

    return 0;
}

/*
 * @brief this function removes all data and call cresponding free handler
 *      automatically if it was registered
 *
 * @param pVector  a vector instance
 *
 * returns error codes
 */
int yramfs_vector_remove_all(yramfs_vector_t *pVector)
{
    yramfs_vector_link_t *plink = NULL, *plinkTmp = NULL;

    if (NULL == pVector) {
        return -1;
    }

    plink = pVector->head;

    while (plink) {
        plinkTmp = plink;
        plink = plink->next;
        if (plinkTmp->free) {
            plinkTmp->free((void*)plinkTmp->data);
        }
        kfree(plink);
    }

    pVector->count = 0;
    return 0;
}

/*
 * @brief this function caculates the count of vector
 *
 * @param pVector  a vector instance
 * @param pCount   out put count value
 *
 * returns error codes
 */
int yramfs_vector_count(yramfs_vector_t *pVector, uint32_t *pCount)
{
    if (NULL == pVector || (NULL == pCount)) {
        return -1;
    }

    *pCount = pVector->count;
    return 0;
}

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
                         uint32_t index, uint32_t *pdata)
{
    int i;
    yramfs_vector_link_t *plink = NULL;

    if (NULL == pVector) {
        return -1;
    }

    if (pVector->count <= index) {
        return -1;
    }

    for (i = 0, plink = pVector->head; i < index; i ++) {
        plink = plink->next;
    }
    if (plink) {
        *pdata = plink->data;
    }

    return 0;
}

/*
int test_vector()
{
    yramfs_vector_t *aVector = NULL;
    uint32_t value = 0;
    int      index = 9;
    int      ret;

    printf("start test ...\n");
    ret = yramfs_vector_create(&aVector);
    if (0 != ret) {
        printf("\t-- create vector failed%d\n", ret);
        return -1;
    }
    printf ("add value...\n");
    while (value <= 9) {
        yramfs_vector_add(aVector, value++, NULL);
    }
    printf("\n");

    printf("dump...\n");
    yramfs_vector_count(aVector, &index);
    while (index >= 0) {
        ret = yramfs_vector_get_at(aVector, index, &value);
        printf("\t--get value %d(%d), ret%d\n", value, index, ret);
        index --;
    }

    printf("remove first and dump...\n");
    yramfs_vector_remove_data(aVector, 0);
    yramfs_vector_count(aVector, &index);
    while (index >= 0) {
        ret = yramfs_vector_get_at(aVector, index, &value);
        printf("\t--get value %d(%d) ret%d\n", value, index, ret);
        index --;
    }

    printf("remove last and dump...\n");
    yramfs_vector_remove_data(aVector, 9);
    yramfs_vector_count(aVector, &index);
    while (index > 0) {
        ret = yramfs_vector_get_at(aVector, index, &value);
        printf("\t--get value %d(%d), ret%d\n", value, index, ret);
        index --;
    }

    printf("remove middle and dump...\n");
    yramfs_vector_remove_data(aVector, 4);
    yramfs_vector_count(aVector, &index);
    while (index > 0) {
        ret = yramfs_vector_get_at(aVector, index, &value);
        printf("\t--get value %d(%d), ret%d\n", value, index, ret);
        index --;
    }

    printf("remove all\n");
    ret = yramfs_vector_remove_all(aVector);
    if (0 != ret) {
        printf("remove all failed%d\n", ret);
        return -1;
    }
    ret = yramfs_vector_count(aVector, &index);
    printf("get count %d, ret%d\n", index, ret);

    return 0;
}
*/
