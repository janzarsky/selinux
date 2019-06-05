#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <CUnit/Basic.h>

#include "test_fcontext.h"
#include "test_fcontext_policy.h"
#include "functions.h"

/* TODO: test function type_str
 * TODO: test function fcontext_print
 * TODO: function fcontext_parse, error gotos
 * TODO: test function validate_handler
 */

extern semanage_handle_t *sh;

int get_type(char *t) {
    if (strcmp(t, "--") == 0)
        return SEMANAGE_FCONTEXT_ALL;
    else if (strcmp(t, "-f") == 0)
        return SEMANAGE_FCONTEXT_REG;
    else if (strcmp(t, "-d") == 0)
        return SEMANAGE_FCONTEXT_DIR;
    else if (strcmp(t, "-c") == 0)
        return SEMANAGE_FCONTEXT_CHAR;
    else if (strcmp(t, "-b") == 0)
        return SEMANAGE_FCONTEXT_BLOCK;
    else if (strcmp(t, "-s") == 0)
        return SEMANAGE_FCONTEXT_SOCK;
    else if (strcmp(t, "-l") == 0)
        return SEMANAGE_FCONTEXT_LINK;
    else if (strcmp(t, "-p") == 0)
        return SEMANAGE_FCONTEXT_PIPE;
    else
        return -1;
}

int write_file_contexts(const char *data, unsigned int data_len) {
    FILE *fptr = fopen("test_root/test_store/active/file_contexts", "w+");

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

int fcontext_test_init(void) {
    if (create_test_store() < 0) {
        fprintf(stderr, "Could not create test store\n");
        return -1;
    }

    if (write_test_policy(FCONTEXT_POLICY, FCONTEXT_POLICY_LEN) < 0) {
        fprintf(stderr, "Could not write test policy\n");
        return -1;
    }

    if (write_file_contexts(FCONTEXTS, FCONTEXTS_LEN) < 0) {
        fprintf(stderr, "Could not write file contexts\n");
        return -1;
    }

	return 0;
}

int fcontext_test_cleanup(void) {
	return destroy_test_store();
}

int fcontext_add_tests(CU_pSuite suite) {
    CU_add_test(suite, "test_fcontext_compare",     test_fcontext_compare);
    CU_add_test(suite, "test_fcontext_compare2",    test_fcontext_compare2);
    CU_add_test(suite, "test_fcontext_key_create",  test_fcontext_key_create);
    CU_add_test(suite, "test_fcontext_key_extract", test_fcontext_key_extract);
    CU_add_test(suite, "test_fcontext_get_set_expr",test_fcontext_get_set_expr);
    CU_add_test(suite, "test_fcontext_get_set_type",test_fcontext_get_set_type);
    CU_add_test(suite, "test_fcontext_get_type_str",test_fcontext_get_type_str);
    CU_add_test(suite, "test_fcontext_get_set_con", test_fcontext_get_set_con);
    CU_add_test(suite, "test_fcontext_create",      test_fcontext_create);
    CU_add_test(suite, "test_fcontext_clone",       test_fcontext_clone);

    CU_add_test(suite, "test_fcontext_query",       test_fcontext_query);
    CU_add_test(suite, "test_fcontext_exists",      test_fcontext_exists);
    CU_add_test(suite, "test_fcontext_count",       test_fcontext_count);
    CU_add_test(suite, "test_fcontext_iterate",     test_fcontext_iterate);
    CU_add_test(suite, "test_fcontext_list",        test_fcontext_list);
    
    CU_add_test(suite, "test_fcontext_modify_del_local",
                test_fcontext_modify_del_local);
    CU_add_test(suite, "test_fcontext_query_local", test_fcontext_query_local);
    CU_add_test(suite, "test_fcontext_exists_local",test_fcontext_exists_local);
    CU_add_test(suite, "test_fcontext_count_local", test_fcontext_count_local);
    CU_add_test(suite, "test_fcontext_iterate_local",
                test_fcontext_iterate_local);
    CU_add_test(suite, "test_fcontext_list_local",  test_fcontext_list_local);

	return 0;
}

// Helpers

semanage_fcontext_t *get_fcontext_new() {
    int result;
    semanage_fcontext_t *fcontext;

    result = semanage_fcontext_create(sh, &fcontext);

    CU_ASSERT_FATAL(result >= 0);

    return fcontext;
}

semanage_fcontext_t *get_fcontext_nth(int index) {
    semanage_fcontext_t **records;
    semanage_fcontext_t *fcontext;
    unsigned int count;

    if (index == I_NULL)
        return NULL;

    CU_ASSERT_FATAL(semanage_fcontext_list(sh, &records, &count) >= 0);
    CU_ASSERT_FATAL(count >= index + 1);

    fcontext = records[index];

    for (unsigned int i = 0; i < count; i++) {
        if (i != index)
            semanage_fcontext_free(records[i]);
    }

    return fcontext;
}

semanage_fcontext_key_t *get_fcontext_key_nth(int index) {
    semanage_fcontext_key_t *key;
    semanage_fcontext_t *fcontext;

    if (index == I_NULL)
        return NULL;

    fcontext = get_fcontext_nth(index);

    CU_ASSERT_FATAL(semanage_fcontext_key_extract(sh, fcontext, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

void add_local_fcontext(int fcontext_index) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_key_t *key = NULL;

    CU_ASSERT_FATAL(fcontext_index != I_NULL);

    fcontext = get_fcontext_nth(fcontext_index);

    CU_ASSERT_FATAL(semanage_fcontext_key_extract(sh, fcontext, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    CU_ASSERT_FATAL(semanage_fcontext_modify_local(sh, key, fcontext) >= 0);
}

void delete_local_fcontext(int fcontext_index) {
    semanage_fcontext_key_t *key = NULL;

    CU_ASSERT_FATAL(fcontext_index != I_NULL);

    key = get_fcontext_key_nth(fcontext_index);

    CU_ASSERT_FATAL(semanage_fcontext_del_local(sh, key) >= 0);
}

semanage_fcontext_key_t *get_fcontext_key_from_str(const char *str, int type) {
    semanage_fcontext_key_t *key;
    int result;

    if (str == NULL)
        return NULL;

    result = semanage_fcontext_key_create(sh, str, type, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

// Function semanage_fcontext_compare

void test_fcontext_compare(void) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_key_t *key1;
    semanage_fcontext_key_t *key2;
    semanage_fcontext_key_t *key3;

    // setup
    setup_handle(SH_CONNECT);

    fcontext = get_fcontext_nth(I_FIRST);

    key1 = get_fcontext_key_nth(I_FIRST);
    key2 = get_fcontext_key_nth(I_SECOND);
    key3 = get_fcontext_key_nth(I_THIRD);

    // test
    CU_ASSERT(semanage_fcontext_compare(fcontext, key1) == 0);
    CU_ASSERT(semanage_fcontext_compare(fcontext, key2) < 0);
    CU_ASSERT(semanage_fcontext_compare(fcontext, key3) > 0);

    // cleanup
    semanage_fcontext_free(fcontext);
    semanage_fcontext_key_free(key1);
    semanage_fcontext_key_free(key2);
    semanage_fcontext_key_free(key3);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_fcontext_compare2
// TODO: test same expr, different type
// TODO: test function semanage_fcontext_compare2_qsort

void test_fcontext_compare2(void) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_t *fcontext1;
    semanage_fcontext_t *fcontext2;
    semanage_fcontext_t *fcontext3;

    // setup
    setup_handle(SH_CONNECT);

    fcontext = get_fcontext_nth(I_FIRST);
    fcontext1 = get_fcontext_nth(I_FIRST);
    fcontext2 = get_fcontext_nth(I_SECOND);
    fcontext3 = get_fcontext_nth(I_THIRD);

    // test
    CU_ASSERT(semanage_fcontext_compare2(fcontext, fcontext1) == 0);
    CU_ASSERT(semanage_fcontext_compare2(fcontext, fcontext2) < 0);
    CU_ASSERT(semanage_fcontext_compare2(fcontext, fcontext3) > 0);

    // cleanup
    semanage_fcontext_free(fcontext);
    semanage_fcontext_free(fcontext1);
    semanage_fcontext_free(fcontext2); 
    semanage_fcontext_free(fcontext3); 
    cleanup_handle(SH_CONNECT);
}

// Function semanage_fcontext_key_create

void test_fcontext_key_create(void) {
    semanage_fcontext_key_t *key = NULL;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_fcontext_key_create(sh, "", SEMANAGE_FCONTEXT_ALL,
                                           &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_fcontext_key_free(key);

    key = NULL;

    CU_ASSERT(semanage_fcontext_key_create(sh, "testfcontext",
                                           SEMANAGE_FCONTEXT_ALL, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_fcontext_key_free(key);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_fcontext_key_extract
// TODO: make semanage_fcontext_key_create return error

void test_fcontext_key_extract(void) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_key_t *key;
    
    // setup
    setup_handle(SH_CONNECT);

    fcontext = get_fcontext_nth(I_FIRST);

    // test
    CU_ASSERT(semanage_fcontext_key_extract(sh, fcontext, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    // cleanup
    semanage_fcontext_key_free(key);
    semanage_fcontext_free(fcontext);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_fcontext_get_expr, semanage_fcontext_set_expr

void test_fcontext_get_set_expr(void) {
    semanage_fcontext_t *fcontext;
    const char *expr = NULL;
    char *expr_exp = "/asdf";
    
    // setup
    setup_handle(SH_CONNECT);

    fcontext = get_fcontext_nth(I_FIRST);

    // test
    CU_ASSERT(semanage_fcontext_set_expr(sh, fcontext, expr_exp) >= 0);

    expr = semanage_fcontext_get_expr(fcontext);

    CU_ASSERT_PTR_NOT_NULL(expr);
    CU_ASSERT_STRING_EQUAL(expr, expr_exp);

    // cleanup
    semanage_fcontext_free(fcontext);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_fcontext_get_type, semanage_fcontext_set_type

void test_fcontext_get_set_type(void) {
    semanage_fcontext_t *fcontext;
    int type_exp = SEMANAGE_FCONTEXT_SOCK;
    int type;
    
    // setup
    setup_handle(SH_CONNECT);

    fcontext = get_fcontext_nth(I_FIRST);

    // test
    semanage_fcontext_set_type(fcontext, type_exp);

    type = semanage_fcontext_get_type(fcontext);

    CU_ASSERT(type == type_exp);

    // cleanup
    semanage_fcontext_free(fcontext);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_fcontext_get_type_str

void helper_fcontext_get_type_str(int type, const char *exp_str) {
    CU_ASSERT_STRING_EQUAL(semanage_fcontext_get_type_str(type), exp_str);
}

void test_fcontext_get_type_str(void) {
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_ALL,   "all files");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_REG,   "regular file");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_DIR,   "directory");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_CHAR,  "character device");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_BLOCK, "block device");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_SOCK,  "socket");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_LINK,  "symbolic link");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_PIPE,  "named pipe");

    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_ALL - 1,  "????");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_PIPE + 1, "????");
}

// Function semanage_fcontext_get_con, semanage_fcontext_set_con

void helper_fcontext_get_set_con(level_t level, int fcontext_index,
                                 const char *con_str) {
    semanage_fcontext_t *fcontext;
    semanage_context_t *con = NULL;
    semanage_context_t *new_con = NULL;
    
    setup_handle(level);

    fcontext = get_fcontext_nth(fcontext_index);

    if (con_str != NULL) {
        CU_ASSERT(semanage_context_from_string(sh, con_str, &con) >= 0);
        CU_ASSERT_PTR_NOT_NULL(con);
    }
    else {
        con = NULL;
    }

    // set con
    CU_ASSERT(semanage_fcontext_set_con(sh, fcontext, con) >= 0);

    // get con
    new_con = semanage_fcontext_get_con(fcontext);

    if (con_str != NULL) {
        CU_ASSERT_CONTEXT_EQUAL(con, new_con);
    }
    else {
        CU_ASSERT_PTR_NULL(new_con);
    }

    semanage_fcontext_free(fcontext);
    cleanup_handle(level);
}

void test_fcontext_get_set_con(void) {
    helper_fcontext_get_set_con(SH_CONNECT, I_FIRST, NULL);

    helper_fcontext_get_set_con(SH_CONNECT, I_FIRST, "user_u:role_r:type_t:s0");
    helper_fcontext_get_set_con(SH_CONNECT, I_SECOND,
                                "user_u:role_r:type_t:s0");

    helper_fcontext_get_set_con(SH_TRANS, I_FIRST, NULL);

    helper_fcontext_get_set_con(SH_TRANS, I_FIRST,  "user_u:role_r:type_t:s0");
    helper_fcontext_get_set_con(SH_TRANS, I_SECOND, "user_u:role_r:type_t:s0");
}

// Function semanage_fcontext_create

void helper_fcontext_create(level_t level) {
    semanage_fcontext_t *fcontext;
    
    setup_handle(level);

    CU_ASSERT(semanage_fcontext_create(sh, &fcontext) >= 0);

    CU_ASSERT_PTR_NULL(semanage_fcontext_get_expr(fcontext));
    CU_ASSERT(semanage_fcontext_get_type(fcontext) == SEMANAGE_FCONTEXT_ALL);
    CU_ASSERT_PTR_NULL(semanage_fcontext_get_con(fcontext));

    semanage_fcontext_free(fcontext);
    cleanup_handle(level);
}

void test_fcontext_create(void) {
    helper_fcontext_create(SH_NULL);
    helper_fcontext_create(SH_HANDLE);
    helper_fcontext_create(SH_CONNECT);
    helper_fcontext_create(SH_TRANS);
}

// Function semanage_fcontext_clone
// TODO: error gotos

void helper_fcontext_clone(level_t level, int fcontext_index) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_t *fcontext_clone;
    const char *expr;
    const char *expr_clone;
    int type;
    int type_clone;
    semanage_context_t *con;
    semanage_context_t *con_clone;
    
    setup_handle(level);

    fcontext = get_fcontext_nth(fcontext_index);

    // clone
    CU_ASSERT(semanage_fcontext_clone(sh, fcontext, &fcontext_clone) >= 0);

    // compare expr
    expr = semanage_fcontext_get_expr(fcontext);
    expr_clone = semanage_fcontext_get_expr(fcontext_clone);

    CU_ASSERT_STRING_EQUAL(expr, expr_clone);

    // compare type
    type = semanage_fcontext_get_type(fcontext);
    type_clone = semanage_fcontext_get_type(fcontext_clone);

    CU_ASSERT_EQUAL(type, type_clone);

    // compare context
    con = semanage_fcontext_get_con(fcontext);
    con_clone = semanage_fcontext_get_con(fcontext_clone);

    CU_ASSERT_CONTEXT_EQUAL(con, con_clone);

    semanage_fcontext_free(fcontext);
    semanage_fcontext_free(fcontext_clone);
    cleanup_handle(level);
}

void test_fcontext_clone(void) {
    helper_fcontext_clone(SH_CONNECT, I_FIRST);
    helper_fcontext_clone(SH_CONNECT, I_SECOND);

    helper_fcontext_clone(SH_TRANS, I_FIRST);
    helper_fcontext_clone(SH_TRANS, I_SECOND);
}

// Function semanage_fcontext_query

void helper_fcontext_query(level_t level, const char *fcontext_expr,
                           int fcontext_type, int exp_result) {
    semanage_fcontext_key_t *key;
    // some non-null value
    semanage_fcontext_t *response = (void *) 42;
    const char *expr;
    int result;

    setup_handle(level);

    key = get_fcontext_key_from_str(fcontext_expr, fcontext_type);

    result = semanage_fcontext_query(sh, key, &response);

    if (exp_result >= 0) {
        CU_ASSERT(result >= 0);

        expr = semanage_fcontext_get_expr(response);

        CU_ASSERT_STRING_EQUAL(expr, fcontext_expr);
    }
    else {
        CU_ASSERT(result < 0);
        CU_ASSERT(response == (void *) 42);
    }

    cleanup_handle(level);
}

void test_fcontext_query(void) {
    helper_fcontext_query(SH_CONNECT, FCONTEXT_NONEXISTENT_EXPR,
                          FCONTEXT_NONEXISTENT_TYPE, -1);
    // different type
    helper_fcontext_query(SH_CONNECT, FCONTEXT2_EXPR, FCONTEXT1_TYPE, -1);

    helper_fcontext_query(SH_CONNECT, FCONTEXT1_EXPR,  FCONTEXT1_TYPE,  1);
    helper_fcontext_query(SH_CONNECT, FCONTEXT2_EXPR, FCONTEXT2_TYPE, 1);

    helper_fcontext_query(SH_TRANS, FCONTEXT_NONEXISTENT_EXPR,
                          FCONTEXT_NONEXISTENT_TYPE, -1);
    // different type
    helper_fcontext_query(SH_TRANS, FCONTEXT2_EXPR, FCONTEXT1_TYPE, -1);

    helper_fcontext_query(SH_TRANS, FCONTEXT1_EXPR,  FCONTEXT1_TYPE,  1);
    helper_fcontext_query(SH_TRANS, FCONTEXT2_EXPR, FCONTEXT2_TYPE, 1);
}

// Function semanage_fcontext_exists

void helper_fcontext_exists(level_t level, char * fcontext_expr,
                            int fcontext_type, int exp_response) {
    semanage_fcontext_key_t *key;
    int response;

    setup_handle(level);

    key = get_fcontext_key_from_str(fcontext_expr, fcontext_type);

    CU_ASSERT(semanage_fcontext_exists(sh, key, &response) >= 0);
    CU_ASSERT(response == exp_response);

    semanage_fcontext_key_free(key);

    cleanup_handle(level);
}

void test_fcontext_exists(void) {
    helper_fcontext_exists(SH_CONNECT, FCONTEXT_NONEXISTENT_EXPR,
                           FCONTEXT_NONEXISTENT_TYPE, 0);
    // different type
    helper_fcontext_exists(SH_CONNECT, FCONTEXT2_EXPR, FCONTEXT1_TYPE, 0);

    helper_fcontext_exists(SH_CONNECT, FCONTEXT1_EXPR,  FCONTEXT1_TYPE,  1);
    helper_fcontext_exists(SH_CONNECT, FCONTEXT2_EXPR, FCONTEXT2_TYPE, 1);

    helper_fcontext_exists(SH_TRANS, FCONTEXT_NONEXISTENT_EXPR,
                           FCONTEXT_NONEXISTENT_TYPE, 0);
    // different type
    helper_fcontext_exists(SH_TRANS, FCONTEXT2_EXPR, FCONTEXT1_TYPE,  0);

    helper_fcontext_exists(SH_TRANS, FCONTEXT1_EXPR,  FCONTEXT1_TYPE,  1);
    helper_fcontext_exists(SH_TRANS, FCONTEXT2_EXPR, FCONTEXT2_TYPE, 1);
}

// Function semanage_fcontext_count

void test_fcontext_count(void) {
    unsigned int response;

    //handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_fcontext_count(sh, &response) < 0);

    CU_ASSERT(semanage_fcontext_count(sh, NULL) < 0);
    
    cleanup_handle(SH_HANDLE);
    
    //connect
    response = 0;
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_fcontext_count(sh, &response) >= 0);
    CU_ASSERT(response == FCONTEXTS_COUNT);

    cleanup_handle(SH_CONNECT);

    //trans
    response = 0;
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_fcontext_count(sh, &response) >= 0);
    CU_ASSERT(response == FCONTEXTS_COUNT);

    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_iterate

unsigned int counter_fcontext_iterate = 0;

int handler_fcontext_iterate(const semanage_fcontext_t *record, void *varg) {
    counter_fcontext_iterate++;
    return 0;
}

void helper_fcontext_iterate_invalid(void) {
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_fcontext_iterate(sh, &handler_fcontext_iterate,
                                        NULL) < 0);

    CU_ASSERT(semanage_fcontext_iterate(sh, NULL, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_fcontext_iterate(level_t level) {
    setup_handle(level);

    counter_fcontext_iterate = 0;

    CU_ASSERT(semanage_fcontext_iterate(sh, &handler_fcontext_iterate,
                                        NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate == FCONTEXTS_COUNT);

    cleanup_handle(level);
}

void test_fcontext_iterate(void) {
    helper_fcontext_iterate_invalid();
    helper_fcontext_iterate(SH_CONNECT);
    helper_fcontext_iterate(SH_TRANS);
}

// Function semanage_fcontext_list

void helper_fcontext_list_invalid(void) {
    semanage_fcontext_t **records;
    unsigned int count;

    setup_handle(SH_HANDLE);
    
    CU_ASSERT(semanage_fcontext_list(sh, &records, &count) < 0);

    CU_ASSERT(semanage_fcontext_list(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_fcontext_list(sh, &records, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_fcontext_list(level_t level) {
    semanage_fcontext_t **records;
    unsigned int count;

    setup_handle(level);
    
    CU_ASSERT(semanage_fcontext_list(sh, &records, &count) >= 0);
    CU_ASSERT(count == FCONTEXTS_COUNT);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    for (unsigned int i = 0; i < count; i++) {
        semanage_fcontext_free(records[i]);
    }

    cleanup_handle(level);
}

void test_fcontext_list(void) {
    helper_fcontext_list_invalid();
    helper_fcontext_list(SH_CONNECT);
    helper_fcontext_list(SH_TRANS);
}

// Function semanage_fcontext_modify_local, semanage_fcontext_del_local

void helper_fcontext_modify_del_local(level_t level, int fcontext_index,
                                      const char *con_str, int exp_result) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_t *fcontext_local;
    semanage_fcontext_key_t *key = NULL;
    semanage_context_t *con = NULL;
    int result;

    setup_handle(level);

    fcontext = get_fcontext_nth(fcontext_index);

    CU_ASSERT(semanage_fcontext_key_extract(sh, fcontext, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    if (con_str != NULL) {
        CU_ASSERT(semanage_context_from_string(sh, con_str, &con) >= 0);
        CU_ASSERT_PTR_NOT_NULL(con);
    }
    else {
        con = NULL;
    }

    CU_ASSERT(semanage_fcontext_set_con(sh, fcontext, con) >= 0);

    result = semanage_fcontext_modify_local(sh, key, fcontext);

    if (exp_result < 0) {
        CU_ASSERT(result < 0);
    }
    else {
        CU_ASSERT(result >= 0);

        if (level == SH_TRANS) {
            helper_commit();
            helper_begin_transaction();
        }

        CU_ASSERT(semanage_fcontext_query_local(sh, key, &fcontext_local) >= 0);
        CU_ASSERT(semanage_fcontext_compare2(fcontext_local, fcontext) == 0);

        CU_ASSERT(semanage_fcontext_del_local(sh, key) >= 0);

        CU_ASSERT(semanage_fcontext_query_local(sh, key, &fcontext_local) < 0);
    }

    semanage_fcontext_key_free(key);
    semanage_fcontext_free(fcontext);

    cleanup_handle(level);
}

void test_fcontext_modify_del_local(void) {
    helper_fcontext_modify_del_local(SH_CONNECT, I_FIRST,
                                     "system_u:object_r:tmp_t:s0", -1);
    helper_fcontext_modify_del_local(SH_CONNECT, I_SECOND,
                                     "system_u:object_r:tmp_t:s0", -1);

    helper_fcontext_modify_del_local(SH_TRANS, I_FIRST,
                                     "system_u:object_r:tmp_t:s0", 1);
    helper_fcontext_modify_del_local(SH_TRANS, I_SECOND,
                                     "system_u:object_r:tmp_t:s0", 1);
}

// Function semanage_fcontext_query_local

void test_fcontext_query_local(void) {
    semanage_fcontext_key_t *key = NULL;
    semanage_fcontext_t *response = NULL;

    // connect
    setup_handle(SH_CONNECT);

    key = get_fcontext_key_nth(I_FIRST);

    CU_ASSERT(semanage_fcontext_query_local(sh, key, &response) < 0);
    CU_ASSERT_PTR_NULL(response);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    key = get_fcontext_key_nth(I_FIRST);

    CU_ASSERT(semanage_fcontext_query_local(sh, key, &response) < 0);
    CU_ASSERT_PTR_NULL(response);

    add_local_fcontext(I_FIRST);

    CU_ASSERT(semanage_fcontext_query_local(sh, key, &response) >= 0);
    CU_ASSERT_PTR_NOT_NULL(response);

    semanage_fcontext_key_free(key);
    key = get_fcontext_key_nth(I_SECOND);

    add_local_fcontext(I_SECOND);

    CU_ASSERT(semanage_fcontext_query_local(sh, key, &response) >= 0);
    CU_ASSERT_PTR_NOT_NULL(response);

    delete_local_fcontext(I_FIRST);
    delete_local_fcontext(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_exists_local

void test_fcontext_exists_local(void) {
    int response = -1;
    semanage_fcontext_key_t *key;
    setup_handle(SH_TRANS);

    key = get_fcontext_key_nth(I_FIRST);

    CU_ASSERT(semanage_fcontext_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 0);

    add_local_fcontext(I_FIRST);
    response = -1;

    CU_ASSERT(semanage_fcontext_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 1);

    delete_local_fcontext(I_FIRST);
    response = -1;

    CU_ASSERT(semanage_fcontext_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 0);

    response = -1;

    CU_ASSERT(semanage_fcontext_exists_local(sh, NULL, &response) >= 0);
    CU_ASSERT(response == 0);

    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_count_local

void test_fcontext_count_local(void) {
    unsigned int response;

    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) < 0);

    cleanup_handle(SH_HANDLE);
    
    // connect
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 0);

    cleanup_handle(SH_CONNECT);
    
    // transaction
    setup_handle(SH_TRANS);
    
    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 0);

    add_local_fcontext(I_FIRST);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 1);

    add_local_fcontext(I_SECOND);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 2);

    delete_local_fcontext(I_SECOND);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 1);

    delete_local_fcontext(I_FIRST);

    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_iterate_local

unsigned int counter_fcontext_iterate_local = 0;

int handler_fcontext_iterate_local(const semanage_fcontext_t *record,
                                   void *varg) {
    counter_fcontext_iterate_local++;
    return 0;
}

void test_fcontext_iterate_local(void) {
    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                    &handler_fcontext_iterate_local, NULL) < 0);

    CU_ASSERT(semanage_fcontext_iterate_local(sh, NULL, NULL) < 0);

    cleanup_handle(SH_HANDLE);
    
    // connect
    setup_handle(SH_CONNECT);

    counter_fcontext_iterate_local = 0;
    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                   &handler_fcontext_iterate_local, NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate_local == 0);

    // FIXME
    CU_ASSERT(semanage_fcontext_iterate_local(sh, NULL, NULL) >= 0);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    counter_fcontext_iterate_local = 0;
    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                   &handler_fcontext_iterate_local, NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate_local == 0);

    add_local_fcontext(I_FIRST);

    counter_fcontext_iterate_local = 0;
    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                   &handler_fcontext_iterate_local, NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate_local == 1);

    add_local_fcontext(I_SECOND);

    counter_fcontext_iterate_local = 0;
    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                   &handler_fcontext_iterate_local, NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate_local == 2);

    delete_local_fcontext(I_FIRST);
    delete_local_fcontext(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_list_local

void test_fcontext_list_local(void) {
    semanage_fcontext_t **records;
    unsigned int count;

    // handle
    setup_handle(SH_HANDLE);
    
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) < 0);

    CU_ASSERT(semanage_fcontext_list_local(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, NULL) < 0);

    cleanup_handle(SH_HANDLE);

    // connect
    setup_handle(SH_CONNECT);
    
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 0);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 0);

    add_local_fcontext(I_FIRST);
    
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 1);
    CU_ASSERT_PTR_NOT_NULL(records[0]);

    add_local_fcontext(I_SECOND);
    
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 2);
    CU_ASSERT_PTR_NOT_NULL(records[0]);
    CU_ASSERT_PTR_NOT_NULL(records[1]);

    delete_local_fcontext(I_FIRST);
    delete_local_fcontext(I_SECOND);
    cleanup_handle(SH_TRANS);
}
