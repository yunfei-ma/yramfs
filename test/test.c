/*
 * INCLUDE FILES
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <linux/types.h>

/*
 * MACRO DEFINITIONS
 */
#define THREAD_NUM   6

#define TEST_CYCLE        5000

#define TEST_SEQUENCE_FILE_ID               0
#define TEST_CONCURRENCY_FILE_ID            1
#define TEST_CONCURRENCY_WRITE_FILE_ID      2

/*
  * STRUCTURE DEFINITIONS
  */
typedef struct {
    int      num;
    char     filename[255];
}threadParamType;

/*
 * LOCAL FUNCTION PROTOTYPES
 */
static void* write_file_simple_process(void*);
static int test_threads_sequence(const char *filename);
static int test_threads_concurrency(const char *filename);
static int test_thread_concurrency_write(const char *filename);

int main(int argc, char **args)
{
    int  ret;
    struct timeval timeBefore, timeAfer;
    __kernel_suseconds_t timeDiff;

    printf("input file is %s\n", args[1]);
    if (strlen((const char *)args[1]) > 200) {
        printf("file name is too long, limited to 250 charactores\n");
        return -1;
    }

    printf("---------------------\n");
    printf("start sequence test\n");
    printf("---------------------\n");
    gettimeofday(&timeBefore, 0);
    ret = test_threads_sequence((const char *)args[1]);
    gettimeofday(&timeAfer, 0);
    timeDiff = (timeAfer.tv_usec - timeBefore.tv_usec) / 1000 +
               (timeAfer.tv_sec - timeBefore.tv_sec) * 1000;
    printf("sequence test end. \n\tduration is %ld ms\n", timeDiff);

    printf("---------------------\n");
    printf("start concurrency open write test\n");
    printf("---------------------\n");
    gettimeofday(&timeBefore, 0);
    ret = test_threads_concurrency((const char *)args[1]);
    gettimeofday(&timeAfer, 0);
    timeDiff = (timeAfer.tv_usec - timeBefore.tv_usec) / 1000 +
               (timeAfer.tv_sec - timeBefore.tv_sec) * 1000;
    printf("concurrency open write test end. \n\tduration is %ld ms\n", timeDiff);

    printf("---------------------\n");
    printf("start concurrency write test\n");
    printf("---------------------\n");
    gettimeofday(&timeBefore, 0);
    ret = test_thread_concurrency_write((const char *)args[1]);
    gettimeofday(&timeAfer, 0);
    timeDiff = (timeAfer.tv_usec - timeBefore.tv_usec) / 1000 +
               (timeAfer.tv_sec - timeBefore.tv_sec) * 1000;
    printf("concurrency write test end. \n\tduration is %ld ms\n", timeDiff);
    exit(0);
}

/*
  * @brief simple open -> write -> close file
  */
static void *write_file_simple_process(void* ptr)
{
    FILE *pFile = NULL;
    char  buf[3];
    threadParamType  *param_ptr = (threadParamType*)ptr;

    pFile = fopen(param_ptr->filename, "a");
    if(NULL == pFile) {
        printf("A open file failed,%s\n", param_ptr->filename);
        return NULL;
    }

    if(5 == param_ptr->num) {
        sprintf(buf, "%1d\n", param_ptr->num);
        fwrite(buf, 1, 2, pFile);
    } else {
        sprintf(buf, "%1d", param_ptr->num);
        fwrite(buf, 1, 1, pFile);
    }
    fclose(pFile);

    return NULL;
}

/*
  * @brief open file write and wait to test multi-thread,
  */
static void *write_file_multi_thread_process(void *ptr)
{
    FILE *pFile = NULL;
    char  buf[3];
    threadParamType  *param_ptr = (threadParamType*)ptr;
    int   write_times = 3;

    pFile = fopen(param_ptr->filename, "a");
    if(NULL == pFile) {
        printf("A open file failed,%s\n", param_ptr->filename);
        return NULL;
    }

    while(write_times --) {
        if((5 == param_ptr->num) && (write_times == 0)) {
            sprintf(buf, "%1d\n", param_ptr->num);
            fwrite(buf, 1, 2, pFile);
        } else {
            sprintf(buf, "%1d", param_ptr->num);
            fwrite(buf, 1, 1, pFile);
        }
    }
    fclose(pFile);

    return NULL;

}

/*
  * this test threads one by one in sequence
  */
static int test_threads_sequence(const char *filename)
{
    pthread_t thread[THREAD_NUM];
    int i = 0, cycle = TEST_CYCLE;
    threadParamType  param;
    int  ret;

    sprintf(param.filename, "%s-%d", filename, TEST_SEQUENCE_FILE_ID);
    printf("writing to file:%s\n", param.filename);
    while(cycle --){
        /* create independent thread, each of this will execute print_msg */
        for(i = 0; i < THREAD_NUM; i ++) {
            param.num = i;
            ret = pthread_create(&thread[i], NULL, write_file_simple_process,
                                    (void*)&param);
            if(0 != ret){
                printf("thread(%d) creation failed: %d\n", i, ret);
                return ret;
            }
            pthread_join(thread[i], NULL);
        }
    }

    return ret;
}

/*
  * this test threads run concurrency
  */
static int test_threads_concurrency(const char *filename)
{
    pthread_t thread[THREAD_NUM];
    int i = 0, cycle = TEST_CYCLE;
    threadParamType  param[THREAD_NUM];
    int  ret;

    for (i = 0; i < THREAD_NUM; i ++) {
        sprintf(param[i].filename, "%s-%d", filename, TEST_CONCURRENCY_FILE_ID);
    }
    printf("writing to file:%s\n", param[0].filename);
    while(cycle --){
        /* create independent thread, each of this will execute print_msg */
        for(i = 0; i < THREAD_NUM; i ++) {
            param[i].num = i;
            ret = pthread_create(&thread[i], NULL, write_file_simple_process,
                                    (void*)&param[i]);
            if(0 != ret){
                printf("thread(%d) creation failed: %d\n", i, ret);
                return ret;
            }
        }

        for (i = 0; i < THREAD_NUM; i ++) {
            pthread_join(thread[i], NULL);
        }
    }

    return ret;

}

static int test_thread_concurrency_write(const char *filename)
{
    pthread_t thread[THREAD_NUM];
    int i = 0, cycle = TEST_CYCLE;
    threadParamType  param[THREAD_NUM];
    int  ret;

    for (i = 0; i < THREAD_NUM; i ++) {
        sprintf(param[i].filename, "%s-%d", filename,
                    TEST_CONCURRENCY_WRITE_FILE_ID);
    }
    cycle = 100;
    printf("writing to file:%s\n", param[0].filename);
    while(cycle --){
        /* create independent thread, each of this will execute print_msg */
        for(i = 0; i < THREAD_NUM; i ++) {
            param[i].num = i;
            ret = pthread_create(&thread[i], NULL,
                            write_file_multi_thread_process, (void*)&param[i]);
            if(0 != ret){
                printf("thread(%d) creation failed: %d\n", i, ret);
                return ret;
            }
        }

        for (i = 0; i < THREAD_NUM; i ++) {
            pthread_join(thread[i], NULL);
        }
    }

    return ret;
}
