#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fts.h>
#include <stdlib.h>
#include <stdarg.h>

#include <semanage/handle.h>
#include <semanage/semanage.h>

#include <CUnit/Basic.h>

#include "functions.h"

int result;
int test_store_enabled = 0;

semanage_handle_t *sh = NULL;

void stderr_callback(void *varg, semanage_handle_t *sh, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    fprintf(stderr, "\n");

    va_end(args);
}

int create_test_store() {
    FILE *fptr;

    if (mkdir("test-policy", 0700) < 0)
        return -1;

    if (mkdir("test-policy/store", 0700) < 0)
        return -1;

    if (mkdir("test-policy/store/active", 0700) < 0)
        return -1;

    if (mkdir("test-policy/store/active/modules", 0700) < 0)
        return -1;

    if (mkdir("test-policy/etc", 0700) < 0)
        return -1;

    if (mkdir("test-policy/etc/selinux", 0700) < 0)
        return -1;

    fptr = fopen("test-policy/etc/selinux/semanage.conf", "w+");

    if (!fptr)
        return -1;

    fclose(fptr);

    enable_test_store();
    return 0;
}

void disable_test_store(void) {
    test_store_enabled = 0;
}

void enable_test_store(void) {
    test_store_enabled = 1;
}

int write_test_policy(unsigned char *data, unsigned int data_len) {
    FILE *fptr = fopen("test-policy/store/active/policy.kern", "wb+");

    if (!fptr) {
        perror("fopen");
        return -1;
    }

    if (fwrite(data, data_len, 1, fptr) != 1) {
        perror("fwrite");
        return -1;
    }

    fclose(fptr);

    return 0;
}

int write_test_policy_src(unsigned char *data, unsigned int data_len) {
    if (mkdir("test-policy/store/active/modules/100", 0700) < 0)
        return -1;

    if (mkdir("test-policy/store/active/modules/100/base", 0700) < 0)
        return -1;

    FILE *fptr = fopen("test-policy/store/active/modules/100/base/cil", "w+");

    if (!fptr) {
        perror("fopen");
        return -1;
    }

    if (fwrite(data, data_len, 1, fptr) != 1) {
        perror("fwrite");
        return -1;
    }

    fclose(fptr);

    fptr = fopen("test-policy/store/active/modules/100/base/lang_ext", "w+");

    if (!fptr) {
        perror("fopen");
        return -1;
    }

    if (fwrite("cil", sizeof "cil", 1, fptr) != 1) {
        perror("fwrite");
        return -1;
    }

    fclose(fptr);

    return 0;
}

int destroy_test_store() {
    FTS *ftsp = NULL;
    FTSENT *curr = NULL;
    int ret = 0;

    disable_test_store();

    char *files[] = { "test-policy", NULL };

    ftsp = fts_open(files, FTS_NOCHDIR | FTS_PHYSICAL | FTS_XDEV, NULL);

    if (!ftsp)
        return -1;

    while ((curr = fts_read(ftsp))) {
        switch (curr->fts_info) {
        case FTS_DP:
        case FTS_F:
        case FTS_SL:
        case FTS_SLNONE:
        case FTS_DEFAULT:
            if (remove(curr->fts_accpath) < 0)
                ret = -1;
        default:
            break;
        }
    }

    fts_close(ftsp);

    return ret;
}

void helper_handle_create(void) {
    /* some testsuites uses custom semanage store, some uses
     * /var/lib/selinux/targeted/ */

    if (test_store_enabled)
        semanage_set_root("test-policy");

    sh = semanage_handle_create();
    CU_ASSERT_PTR_NOT_NULL(sh);

    semanage_msg_set_callback(sh, stderr_callback, NULL);

    if (test_store_enabled) {
        semanage_set_create_store(sh, 1);
        semanage_set_reload(sh, 0);
        semanage_set_store_root(sh, "");
        semanage_select_store(sh, "store", SEMANAGE_CON_DIRECT);
    }
}

void helper_handle_destroy(void) {
    semanage_handle_destroy(sh);
}

void helper_connect(void) {
    CU_ASSERT(semanage_connect(sh) >= 0);
}

void helper_disconnect(void) {
    CU_ASSERT(semanage_disconnect(sh) >= 0);
}

void helper_begin_transaction(void) {
    CU_ASSERT(semanage_begin_transaction(sh) >= 0);
}

void helper_commit(void) {
    CU_ASSERT(semanage_commit(sh) >= 0);
}

void setup_handle(level_t level) {
    if (level >= SH_NULL)
        sh = NULL;

    if (level >= SH_HANDLE)
        helper_handle_create();

    if (level >= SH_CONNECT)
        helper_connect();

    if (level >= SH_TRANS)
        helper_begin_transaction();
}

void cleanup_handle(level_t level) {
    if (level >= SH_TRANS)
        helper_commit();

    if (level >= SH_CONNECT)
        helper_disconnect();

    if (level >= SH_HANDLE)
        helper_handle_destroy();

    if (level >= SH_NULL)
        sh = NULL;
}

void setup_handle_invalid_store(level_t level) {
    CU_ASSERT(level >= SH_HANDLE);

    helper_handle_create();
    
    semanage_select_store(sh, "", SEMANAGE_CON_INVALID);

    if (level >= SH_CONNECT)
        helper_connect();

    if (level >= SH_TRANS)
        helper_begin_transaction();
}

int context_compare(semanage_context_t *con, const char *str) {
    char *con_str = NULL;
    int rc;

    CU_ASSERT_PTR_NOT_NULL_FATAL(con);

    CU_ASSERT(semanage_context_to_string(sh, con, &con_str) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(con_str);

    rc = strcmp(con_str, str);

    free(con_str);
    return rc;
}
