#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <semanage/handle.h>
#include <semanage/semanage.h>

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

typedef enum { SH_NULL, SH_HANDLE, SH_CONNECT, SH_TRANS } level_t;

void setup_handle(level_t level);
void cleanup_handle(level_t level);
void setup_handle_invalid_store(level_t level);

void helper_handle_create(void);
void helper_handle_destroy(void);
void helper_connect(void);
void helper_disconnect(void);
void helper_begin_transaction(void);
void helper_commit(void);

int context_compare(semanage_context_t *con, const char *str);

int create_test_store(void);
int write_test_policy(unsigned char *data, unsigned int data_len);
int write_test_policy_src(unsigned char *data, unsigned int data_len);
int destroy_test_store(void);
void enable_test_store(void);
void disable_test_store(void);

// forks, runs code and checks signal returned by child process
#define CU_ASSERT_SIGNAL(CODE,SIGNAL) \
    do { \
        pid_t pid = fork(); \
        if (pid == 0) { \
            CODE; \
            exit(0); \
        } else { \
            int stat_val; \
            wait(&stat_val); \
            CU_ASSERT_FALSE(WIFEXITED(stat_val)); \
            CU_ASSERT_EQUAL(WTERMSIG(stat_val), SIGNAL); \
        } \
    } while(0)

#define CU_ASSERT_CONTEXT(CON1,CON2,EQUAL) \
    do { \
        char *str; \
        char *str2; \
        CU_ASSERT_PTR_NOT_NULL(CON1); \
        CU_ASSERT_PTR_NOT_NULL(CON2); \
        CU_ASSERT(semanage_context_to_string(sh, CON1, &str) >= 0); \
        CU_ASSERT(semanage_context_to_string(sh, CON2, &str2) >= 0); \
        if (EQUAL) {\
            CU_ASSERT_STRING_EQUAL(str, str2); \
        } else {\
            CU_ASSERT_STRING_NOT_EQUAL(str, str2); \
        } \
    } while(0)

#define CU_ASSERT_CONTEXT_EQUAL(CON1,CON2) CU_ASSERT_CONTEXT(CON1,CON2,1)
#define CU_ASSERT_CONTEXT_NOT_EQUAL(CON1,CON2) CU_ASSERT_CONTEXT(CON1,CON2,0)

#define I_NULL  -1
#define I_FIRST  0
#define I_SECOND 1
#define I_THIRD  2

#endif
