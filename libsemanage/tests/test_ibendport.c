#include "utilities.h"
#include "test_ibendport.h"
#include "test_ibendport_policy.h"

extern semanage_handle_t *sh;

int ibendport_test_init(void) {
	if (create_test_store() < 0) {
		fprintf(stderr, "Could not create test store\n");
		return 1;
	}

	if (write_test_policy(IBENDPORT_POLICY, IBENDPORT_POLICY_LEN) < 0) {
		fprintf(stderr, "Could not write test policy\n");
		return 1;
	}

	return 0;
}

int ibendport_test_cleanup(void) {
	if (destroy_test_store() < 0) {
		fprintf(stderr, "Could destroy test store\n");
		return 1;
	}

	return 0;
}

int ibendport_add_tests(CU_pSuite suite) {
	CU_add_test(suite, "ibendport_query", test_ibendport_query);
	CU_add_test(suite, "ibendport_exists", test_ibendport_exists);
	CU_add_test(suite, "ibendport_count", test_ibendport_count);
	CU_add_test(suite, "ibendport_iterate", test_ibendport_iterate);
	CU_add_test(suite, "ibendport_list", test_ibendport_list);

	CU_add_test(suite, "ibendport_modify_del_query_local",
		    test_ibendport_modify_del_query_local);
	CU_add_test(suite, "ibendport_exists_local",
		    test_ibendport_exists_local);
	CU_add_test(suite, "ibendport_count_local", test_ibendport_count_local);
	CU_add_test(suite, "ibendport_iterate_local",
		    test_ibendport_iterate_local);
	CU_add_test(suite, "ibendport_list_local", test_ibendport_list_local);

	return 0;
}

/* Helpers */

semanage_ibendport_t *get_ibendport_nth(int index) {
	semanage_ibendport_t **records;
	semanage_ibendport_t *ibendport;
	unsigned int count;

	if (index == I_NULL)
		return NULL;

	CU_ASSERT_FATAL(semanage_ibendport_list(sh, &records, &count) >= 0);
	CU_ASSERT_FATAL(count >= index + 1);

	ibendport = records[index];

	for (unsigned int i = 0; i < count; i++) {
		if (i != index)
			semanage_ibendport_free(records[i]);
	}

	return ibendport;
}

semanage_ibendport_key_t *get_ibendport_key_nth(int index) {
	semanage_ibendport_key_t *key;
	semanage_ibendport_t *ibendport;
	int result;

	if (index == I_NULL)
		return NULL;

	ibendport = get_ibendport_nth(index);

	result = semanage_ibendport_key_extract(sh, ibendport, &key);

	CU_ASSERT_FATAL(result >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(key);

	return key;
}

void add_local_ibendport(int index) {
	semanage_ibendport_t *ibendport;
	semanage_ibendport_key_t *key = NULL;

	ibendport = get_ibendport_nth(index);

	CU_ASSERT_FATAL(semanage_ibendport_key_extract(sh, ibendport,
						       &key) >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(key);

	CU_ASSERT_FATAL(semanage_ibendport_modify_local(sh, key,
							ibendport) >= 0);
}

void delete_local_ibendport(int index) {
	semanage_ibendport_key_t *key = NULL;
	key = get_ibendport_key_nth(index);
	CU_ASSERT_FATAL(semanage_ibendport_del_local(sh, key) >= 0);
}

/* Function semanage_ibendport_query */
void test_ibendport_query(void) {
	semanage_ibendport_t *ibendport = NULL;
	semanage_ibendport_t *ibendport_exp = NULL;
	semanage_ibendport_key_t *key = NULL;
	semanage_context_t *con = NULL;
	semanage_context_t *con_exp = NULL;
	char *name;
	char *name_exp;

	/* setup */
	setup_handle(SH_CONNECT);
	key = get_ibendport_key_nth(I_FIRST);
	ibendport_exp = get_ibendport_nth(I_FIRST);

	/* test */
	CU_ASSERT(semanage_ibendport_query(sh, key, &ibendport) >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ibendport);

	CU_ASSERT(semanage_ibendport_get_ibdev_name(sh, ibendport, &name) >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(name);
	CU_ASSERT(semanage_ibendport_get_ibdev_name(sh, ibendport_exp,
						    &name_exp) >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(name_exp);
	CU_ASSERT_STRING_EQUAL(name, name_exp);

	CU_ASSERT(semanage_ibendport_get_port(ibendport) ==
		  semanage_ibendport_get_port(ibendport_exp));

	con = semanage_ibendport_get_con(ibendport);
	con_exp = semanage_ibendport_get_con(ibendport_exp);
	CU_ASSERT_PTR_NOT_NULL_FATAL(con);
	CU_ASSERT_PTR_NOT_NULL_FATAL(con_exp);
	CU_ASSERT_CONTEXT_EQUAL(con, con_exp);

	/* cleanup */
	free(name);
	semanage_ibendport_free(ibendport);
	semanage_ibendport_free(ibendport_exp);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_ibendport_exists */
void test_ibendport_exists(void) {
	semanage_ibendport_key_t *key1 = NULL;
	semanage_ibendport_key_t *key2 = NULL;
	int response = 42;

	/* setup */
	setup_handle(SH_CONNECT);
	key1 = get_ibendport_key_nth(I_FIRST);
	CU_ASSERT(semanage_ibendport_key_create(sh, "asdf", 1, &key2) >= 0);

	/* test */
	CU_ASSERT(semanage_ibendport_exists(sh, key1, &response) >= 0);
	CU_ASSERT(response);

	CU_ASSERT(semanage_ibendport_exists(sh, key2, &response) >= 0);
	CU_ASSERT(!response);

	/* cleanup */
	semanage_ibendport_key_free(key1);
	semanage_ibendport_key_free(key2);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_ibendport_count */
void test_ibendport_count(void) {
	unsigned int count = 42;

	/* setup */
	setup_handle(SH_CONNECT);

	/* test */
	CU_ASSERT(semanage_ibendport_count(sh, &count) >= 0);
	CU_ASSERT(count == IBENDPORT_COUNT);

	/* cleanup */
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_ibendport_iterate */
unsigned int helper_ibendport_iterate_counter = 0;

int helper_ibendport_iterate(const semanage_ibendport_t *ibendport,
			     void *fn_arg) {
	CU_ASSERT(fn_arg == (void *) 42);
	helper_ibendport_iterate_counter++;
	return 0;
}

int helper_ibendport_iterate_error(const semanage_ibendport_t *ibendport,
				   void *fn_arg) {
	CU_ASSERT(fn_arg == (void *) 42);
	helper_ibendport_iterate_counter++;
	return -1;
}

int helper_ibendport_iterate_break(const semanage_ibendport_t *ibendport,
				   void *fn_arg) {
	CU_ASSERT(fn_arg == (void *) 42);
	helper_ibendport_iterate_counter++;
	return 1;
}

void test_ibendport_iterate(void) {
	/* setup */
	setup_handle(SH_CONNECT);

	/* test */
	helper_ibendport_iterate_counter = 0;
	CU_ASSERT(semanage_ibendport_iterate(sh, helper_ibendport_iterate,
					     (void *) 42) >= 0);
	CU_ASSERT(helper_ibendport_iterate_counter == IBENDPORT_COUNT);

	/* test function which returns error */
	helper_ibendport_iterate_counter = 0;
	CU_ASSERT(semanage_ibendport_iterate(sh, helper_ibendport_iterate_error,
					     (void *) 42) < 0);
	CU_ASSERT(helper_ibendport_iterate_counter == 1);

	/* test function which requests break */
	helper_ibendport_iterate_counter = 0;
	CU_ASSERT(semanage_ibendport_iterate(sh, helper_ibendport_iterate_break,
					     (void *) 42) >= 0);
	CU_ASSERT(helper_ibendport_iterate_counter == 1);

	/* cleanup */
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_ibendport_list */
void test_ibendport_list(void) {
	semanage_ibendport_t **records = NULL;
	unsigned int count = 42;
	char *name = NULL;
	semanage_context_t *con = NULL;

	/* setup */
	setup_handle(SH_CONNECT);

	/* test */
	CU_ASSERT(semanage_ibendport_list(sh, &records, &count) >= 0);

	CU_ASSERT_PTR_NOT_NULL_FATAL(records);
	CU_ASSERT(count == IBENDPORT_COUNT);

	for (unsigned int i = 0; i < count; i++) {
		CU_ASSERT_PTR_NOT_NULL_FATAL(records[i]);
		CU_ASSERT(semanage_ibendport_get_ibdev_name(sh, records[i],
							    &name) >= 0);
		con = semanage_ibendport_get_con(records[i]);
		CU_ASSERT_PTR_NOT_NULL_FATAL(con);
		free(name);
	}

	/* cleanup */
	for (unsigned int i = 0; i < count; i++) {
		semanage_ibendport_free(records[i]);
	}
	free(records);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_ibendport_modify_local, semanage_ibendport_del_local,
 * semanage_ibendport_query_local
 */
void test_ibendport_modify_del_query_local(void) {
	semanage_ibendport_t *ibendport;
	semanage_ibendport_t *ibendport_local;
	semanage_ibendport_key_t *key = NULL;

	/* setup */
	setup_handle(SH_TRANS);
	ibendport = get_ibendport_nth(I_FIRST);
	CU_ASSERT(semanage_ibendport_key_extract(sh, ibendport, &key) >= 0);
	CU_ASSERT_PTR_NOT_NULL(key);

	/* test */
	CU_ASSERT(semanage_ibendport_modify_local(sh, key, ibendport) >= 0);

	/* write changes to file */
	helper_commit();
	helper_begin_transaction();

	CU_ASSERT(semanage_ibendport_query_local(sh, key,
						 &ibendport_local) >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(ibendport_local);

	CU_ASSERT(semanage_ibendport_del_local(sh, key) >= 0);
	CU_ASSERT(semanage_ibendport_query_local(sh, key,
						 &ibendport_local) < 0);

	/* cleanup */
	semanage_ibendport_free(ibendport);
	cleanup_handle(SH_TRANS);
}

/* Function semanage_ibendport_exists_local */
void test_ibendport_exists_local(void) {
	semanage_ibendport_key_t *key1 = NULL;
	semanage_ibendport_key_t *key2 = NULL;
	int response = 42;

	/* setup */
	setup_handle(SH_TRANS);
	add_local_ibendport(I_FIRST);
	key1 = get_ibendport_key_nth(I_FIRST);
	key2 = get_ibendport_key_nth(I_SECOND);

	/* test */
	CU_ASSERT(semanage_ibendport_exists_local(sh, key1, &response) >= 0);
	CU_ASSERT(response);

	CU_ASSERT(semanage_ibendport_exists_local(sh, key2, &response) >= 0);
	CU_ASSERT(!response);

	/* cleanup */
	CU_ASSERT(semanage_ibendport_del_local(sh, key1) >= 0);
	semanage_ibendport_key_free(key1);
	semanage_ibendport_key_free(key2);
	cleanup_handle(SH_TRANS);
}

/* Function semanage_ibendport_count_local */
void test_ibendport_count_local(void) {
	unsigned int count = 42;

	/* setup */
	setup_handle(SH_TRANS);

	/* test */
	CU_ASSERT(semanage_ibendport_count_local(sh, &count) >= 0);
	CU_ASSERT(count == 0);

	add_local_ibendport(I_FIRST);
	CU_ASSERT(semanage_ibendport_count_local(sh, &count) >= 0);
	CU_ASSERT(count == 1);

	add_local_ibendport(I_SECOND);
	CU_ASSERT(semanage_ibendport_count_local(sh, &count) >= 0);
	CU_ASSERT(count == 2);

	delete_local_ibendport(I_SECOND);
	CU_ASSERT(semanage_ibendport_count_local(sh, &count) >= 0);
	CU_ASSERT(count == 1);

	delete_local_ibendport(I_FIRST);
	CU_ASSERT(semanage_ibendport_count_local(sh, &count) >= 0);
	CU_ASSERT(count == 0);

	/* cleanup */
	cleanup_handle(SH_TRANS);
}

/* Function semanage_ibendport_iterate_local */
unsigned int helper_ibendport_iterate_local_counter = 0;

int helper_ibendport_iterate_local(const semanage_ibendport_t *ibendport,
				   void *fn_arg) {
	CU_ASSERT(fn_arg == (void *) 42);
	helper_ibendport_iterate_local_counter++;
	return 0;
}

int helper_ibendport_iterate_local_error(const semanage_ibendport_t *ibendport,
					 void *fn_arg) {
	CU_ASSERT(fn_arg == (void *) 42);
	helper_ibendport_iterate_local_counter++;
	return -1;
}

int helper_ibendport_iterate_local_break(const semanage_ibendport_t *ibendport,
					 void *fn_arg) {
	CU_ASSERT(fn_arg == (void *) 42);
	helper_ibendport_iterate_local_counter++;
	return 1;
}

void test_ibendport_iterate_local(void) {
	/* setup */
	setup_handle(SH_TRANS);
	add_local_ibendport(I_FIRST);
	add_local_ibendport(I_SECOND);
	add_local_ibendport(I_THIRD);

	/* test */
	helper_ibendport_iterate_local_counter = 0;
	CU_ASSERT(semanage_ibendport_iterate_local(sh,
			     helper_ibendport_iterate_local, (void *) 42) >= 0);
	CU_ASSERT(helper_ibendport_iterate_local_counter == 3);

	/* test function which returns error */
	helper_ibendport_iterate_local_counter = 0;
	CU_ASSERT(semanage_ibendport_iterate_local(sh,
			helper_ibendport_iterate_local_error, (void *) 42) < 0);
	CU_ASSERT(helper_ibendport_iterate_local_counter == 1);

	/* test function which requests break */
	helper_ibendport_iterate_local_counter = 0;
	CU_ASSERT(semanage_ibendport_iterate_local(sh,
		       helper_ibendport_iterate_local_break, (void *) 42) >= 0);

	/* cleanup */
	delete_local_ibendport(I_FIRST);
	delete_local_ibendport(I_SECOND);
	delete_local_ibendport(I_THIRD);
	cleanup_handle(SH_TRANS);
}

/* Function semanage_ibendport_list_local */
void test_ibendport_list_local(void) {
	semanage_ibendport_t **records = NULL;
	unsigned int count = 42;
	char *name = NULL;
	semanage_context_t *con = NULL;

	/* setup */
	setup_handle(SH_TRANS);
	add_local_ibendport(I_FIRST);
	add_local_ibendport(I_SECOND);
	add_local_ibendport(I_THIRD);

	/* test */
	CU_ASSERT(semanage_ibendport_list_local(sh, &records, &count) >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(records);
	CU_ASSERT(count == 3);

	for (unsigned int i = 0; i < count; i++) {
		CU_ASSERT_PTR_NOT_NULL_FATAL(records[i]);
		CU_ASSERT(semanage_ibendport_get_ibdev_name(sh, records[i],
								   &name) >= 0);
		con = semanage_ibendport_get_con(records[i]);
		CU_ASSERT_PTR_NOT_NULL_FATAL(con);
		free(name);
	}

	/* cleanup */
	for (unsigned int i = 0; i < count; i++) {
		semanage_ibendport_free(records[i]);
	}
	free(records);
	delete_local_ibendport(I_FIRST);
	delete_local_ibendport(I_SECOND);
	delete_local_ibendport(I_THIRD);
	cleanup_handle(SH_TRANS);
}
