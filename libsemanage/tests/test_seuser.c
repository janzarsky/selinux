/*
 * Authors: Jan Zarsky <jzarsky@redhat.com>
 *
 * Copyright (C) 2019 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <selinux/selinux.h>

#include "utilities.h"
#include "test_seuser.h"

#define SEUSER_NONEXISTENT "nonuser"
#define SEUSER_DEFAULT "__default__"

#define SEUSER "first"
#define SEUSER_SENAME "first_u"
#define SEUSER_MLSRANGE "s0-s0:c0.c1023"
#define SEUSER2 "second"
#define SEUSER2_SENAME "second_u"
#define SEUSER2_MLSRANGE "s0-s15:c0.c1023"

#define SEUSERS_COUNT 3
char *SEUSERS;

/* seuser_record.h */
void test_seuser_key_create(void);
void test_seuser_key_extract(void);
void test_seuser_compare(void);
void test_seuser_compare2(void);
void test_seuser_get_set_name(void);
void test_seuser_get_set_sename(void);
void test_seuser_get_set_mlsrange(void);
void test_seuser_clone(void);
void test_seuser_create(void);

/* seusers_policy.h */
void test_seuser_query(void);
void test_seuser_exists(void);
void test_seuser_count(void);
void test_seuser_iterate(void);
void test_seuser_list(void);

/* seusers_local.h */
void test_seuser_modify_del_local(void);
void test_seuser_query_local(void);
void test_seuser_exists_local(void);
void test_seuser_count_local(void);
void test_seuser_iterate_local(void);
void test_seuser_list_local(void);

extern semanage_handle_t *sh;

int seuser_test_init(void)
{
	if (create_test_store() < 0) {
		fprintf(stderr, "Could not create test store\n");
		goto err;
	}

	if (write_test_policy_from_file("test_user.policy") < 0) {
		fprintf(stderr, "Could not write test policy\n");
		goto err;
	}

	FILE *fptr = fopen("test-policy/store/active/seusers", "w+");
	if (!fptr) {
		perror("fopen");
		goto err;
	}
	
	const char *data = "__default__:first_u:s0-s0:c0.c1023\n"
			   SEUSER ":" SEUSER_SENAME ":" SEUSER_MLSRANGE "\n"
			   SEUSER2 ":" SEUSER2_SENAME ":" SEUSER2_MLSRANGE;
	if (fwrite(data, strlen(data), 1, fptr) != 1) {
		perror("fwrite");
		fclose(fptr);
		goto err;
	}

	fclose(fptr);
	return 0;

err:
	destroy_test_store();
	return 1;
}

int seuser_test_cleanup(void)
{
	if (destroy_test_store() < 0) {
		fprintf(stderr, "Could not destroy test store\n");
		return 1;
	}

	return 0;
}

int seuser_add_tests(CU_pSuite suite)
{
	CU_add_test(suite, "seuser_key_create", test_seuser_key_create);
	CU_add_test(suite, "seuser_key_extract", test_seuser_key_extract);
	CU_add_test(suite, "seuser_compare", test_seuser_compare);
	CU_add_test(suite, "seuser_compare2", test_seuser_compare2);
	CU_add_test(suite, "seuser_get_set_name", test_seuser_get_set_name);
	CU_add_test(suite, "seuser_get_set_sename", test_seuser_get_set_sename);
	CU_add_test(suite, "seuser_get_set_mlsrange",
		    test_seuser_get_set_mlsrange);
	CU_add_test(suite, "seuser_clone", test_seuser_clone);
	CU_add_test(suite, "seuser_create", test_seuser_create);
	CU_add_test(suite, "seuser_query", test_seuser_query);
	CU_add_test(suite, "seuser_exists", test_seuser_exists);
	CU_add_test(suite, "seuser_count", test_seuser_count);
	CU_add_test(suite, "seuser_iterate", test_seuser_iterate);
	CU_add_test(suite, "seuser_list", test_seuser_list);
	CU_add_test(suite, "seuser_modify_del_local",
		    test_seuser_modify_del_local);
	CU_add_test(suite, "seuser_query_local", test_seuser_query_local);
	CU_add_test(suite, "seuser_exists_local", test_seuser_exists_local);
	CU_add_test(suite, "seuser_count_local", test_seuser_count_local);
	CU_add_test(suite, "seuser_iterate_local", test_seuser_iterate_local);
	CU_add_test(suite, "seuser_list_local", test_seuser_list_local);
	 
	return 0;
}

/* Helpers */
semanage_seuser_t *get_seuser_nth(int index)
{
	int result;
	semanage_seuser_t **records;
	semanage_seuser_t *seuser;
	unsigned int count;

	if (index == I_NULL)
		return NULL;

	result = semanage_seuser_list(sh, &records, &count);

	CU_ASSERT_FATAL(result >= 0);
	CU_ASSERT_FATAL(count >= (unsigned int) index + 1);

	seuser = records[index];

	for (unsigned int i = 0; i < count; i++) {
		if (i != (unsigned int) index)
			semanage_seuser_free(records[i]);
	}

	return seuser;
}

semanage_seuser_t *get_seuser_new(void)
{
	int result;
	semanage_seuser_t *seuser;

	result = semanage_seuser_create(sh, &seuser);

	CU_ASSERT_FATAL(result >= 0);

	return seuser;
}

semanage_seuser_key_t *get_seuser_key_nth(int index)
{
	semanage_seuser_key_t *key;
	semanage_seuser_t *seuser;
	int result;

	if (index == I_NULL)
		return NULL;

	seuser = get_seuser_nth(index);

	result = semanage_seuser_key_extract(sh, seuser, &key);

	CU_ASSERT_FATAL(result >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(key);

	return key;
}

semanage_seuser_key_t *get_seuser_key_from_str(const char *str)
{
	semanage_seuser_key_t *key;
	int result;

	if (str == NULL)
		return NULL;

	result = semanage_seuser_key_create(sh, str, &key);

	CU_ASSERT_FATAL(result >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(key);

	return key;
}

void add_local_seuser(int seuser_index)
{
	semanage_seuser_t *seuser;
	semanage_seuser_key_t *key = NULL;

	CU_ASSERT_FATAL(seuser_index != I_NULL);

	seuser = get_seuser_nth(seuser_index);

	CU_ASSERT_FATAL(semanage_seuser_key_extract(sh, seuser, &key) >= 0);
	CU_ASSERT_PTR_NOT_NULL_FATAL(key);

	CU_ASSERT_FATAL(semanage_seuser_modify_local(sh, key, seuser) >= 0);

	semanage_seuser_key_free(key);
	semanage_seuser_free(seuser);
}

void delete_local_seuser(int seuser_index)
{
	semanage_seuser_key_t *key = NULL;
	CU_ASSERT_FATAL(seuser_index != I_NULL);
	key = get_seuser_key_nth(seuser_index);

	CU_ASSERT_FATAL(semanage_seuser_del_local(sh, key) >= 0);

	semanage_seuser_key_free(key);
}

/* Function seuser_key_create */
void helper_seuser_key_create(level_t level)
{
	semanage_seuser_key_t *key = NULL;

	/* setup */
	setup_handle(level);

	/* name == "" */
	key = NULL;
	CU_ASSERT(semanage_seuser_key_create(sh, "", &key) >= 0);
	CU_ASSERT_PTR_NOT_NULL(key);
	semanage_seuser_key_free(key);

	/* name == "testuser" */
	key = NULL;
	CU_ASSERT(semanage_seuser_key_create(sh, "testuser", &key) >= 0);
	CU_ASSERT_PTR_NOT_NULL(key);

	/* cleanup */
	semanage_seuser_key_free(key);
	cleanup_handle(level);
}

void test_seuser_key_create(void)
{
	helper_seuser_key_create(SH_CONNECT);
	helper_seuser_key_create(SH_TRANS);
}

/* Function seuser_key_extract */
#define SK_NULL 1
#define SK_NEW 2
#define SK_INDEX 3
#define SK_KEY_NULL 4
void helper_seuser_key_extract(level_t level, int mode)
{
	semanage_seuser_t *seuser = NULL;
	semanage_seuser_key_t *key = NULL;
	int result;

	/* setup */
	setup_handle(level);

	switch (mode) {
		case SK_NULL:
			seuser = NULL;
			break;
		case SK_NEW:
			seuser = get_seuser_new();
			break;
		case SK_INDEX:
			seuser = get_seuser_nth(0);
			break;
		case SK_KEY_NULL:
			seuser = get_seuser_nth(0);
			break;
		default:
			CU_FAIL_FATAL("Invalid mode\n");
	}

	/* test */
	if (mode == SK_KEY_NULL)
		result = semanage_seuser_key_extract(sh, seuser, NULL);
	else
		result = semanage_seuser_key_extract(sh, seuser, &key);

	CU_ASSERT(result >= 0);

	result = semanage_seuser_compare(seuser, key);
	CU_ASSERT(result == 0);

	/* cleanup */
	semanage_seuser_key_free(key);
	semanage_seuser_free(seuser);
	cleanup_handle(level);
}

void test_seuser_key_extract(void)
{
	helper_seuser_key_extract(SH_CONNECT, SK_INDEX);
	helper_seuser_key_extract(SH_TRANS, SK_INDEX);
}
#undef SK_NULL
#undef SK_NEW
#undef SK_INDEX
#undef SK_KEY_NULL

/* Function seuser_compare */
void helper_seuser_compare(level_t level, int seuser_index1, int seuser_index2)
{
	semanage_seuser_t *seuser;
	semanage_seuser_key_t *key;
	int result;

	/* setup */
	setup_handle(level);
	seuser = get_seuser_nth(seuser_index1);
	key = get_seuser_key_nth(seuser_index2);

	/* test */
	result = semanage_seuser_compare(seuser, key);

	if (seuser_index1 == seuser_index2) {
		CU_ASSERT(result == 0);
	}
	else {
		CU_ASSERT(result != 0);
	}

	/* cleanup */
	semanage_seuser_free(seuser);
	semanage_seuser_key_free(key);
	cleanup_handle(level);
}

void test_seuser_compare(void)
{
	helper_seuser_compare(SH_CONNECT, I_FIRST,  I_FIRST);
	helper_seuser_compare(SH_CONNECT, I_FIRST,  I_SECOND);
	helper_seuser_compare(SH_CONNECT, I_SECOND, I_FIRST);
	helper_seuser_compare(SH_CONNECT, I_SECOND, I_SECOND);
	helper_seuser_compare(SH_TRANS, I_FIRST,  I_FIRST);
	helper_seuser_compare(SH_TRANS, I_FIRST,  I_SECOND);
	helper_seuser_compare(SH_TRANS, I_SECOND, I_FIRST);
	helper_seuser_compare(SH_TRANS, I_SECOND, I_SECOND);
}

/* Function seuser_compare2 */
void helper_seuser_compare2(level_t level, int seuser_index1, int seuser_index2)
{
	semanage_seuser_t *seuser1;
	semanage_seuser_t *seuser2;
	int result;

	/* setup */
	setup_handle(level);
	seuser1 = get_seuser_nth(seuser_index1);
	seuser2 = get_seuser_nth(seuser_index2);

	/* test */
	result = semanage_seuser_compare2(seuser1, seuser2);

	if (seuser_index1 == seuser_index2) {
		CU_ASSERT(result == 0);
	}
	else {
		CU_ASSERT(result != 0);
	}

	/* cleanup */
	semanage_seuser_free(seuser1);
	semanage_seuser_free(seuser2);
	cleanup_handle(level);
}

void test_seuser_compare2(void)
{
	helper_seuser_compare2(SH_CONNECT, I_FIRST,  I_FIRST);
	helper_seuser_compare2(SH_CONNECT, I_FIRST,  I_SECOND);
	helper_seuser_compare2(SH_CONNECT, I_SECOND, I_FIRST);
	helper_seuser_compare2(SH_CONNECT, I_SECOND, I_SECOND);
	helper_seuser_compare2(SH_TRANS, I_FIRST,  I_FIRST);
	helper_seuser_compare2(SH_TRANS, I_FIRST,  I_SECOND);
	helper_seuser_compare2(SH_TRANS, I_SECOND, I_FIRST);
	helper_seuser_compare2(SH_TRANS, I_SECOND, I_SECOND);
}

/* Function seuser_get_name, seuser_set_name */
void helper_seuser_get_set_name(level_t level, int seuser_index,
				const char *name)
{
	semanage_seuser_t *user;
	const char *new_name = NULL;

	/* setup */
	setup_handle(level);
	user = get_seuser_nth(seuser_index);

	/* test */
	CU_ASSERT(semanage_seuser_set_name(sh, user, name) >= 0);
	new_name = semanage_seuser_get_name(user);
	CU_ASSERT_PTR_NOT_NULL(new_name);
	assert(new_name);
	CU_ASSERT_STRING_EQUAL(new_name, name);

	/* cleanup */
	semanage_seuser_free(user);
	cleanup_handle(level);
}

void test_seuser_get_set_name(void)
{
	helper_seuser_get_set_name(SH_CONNECT, I_FIRST, "testseuser");
	helper_seuser_get_set_name(SH_CONNECT, I_FIRST, "");
	helper_seuser_get_set_name(SH_CONNECT, I_SECOND, "testseuser");
	helper_seuser_get_set_name(SH_CONNECT, I_SECOND, "");
	helper_seuser_get_set_name(SH_TRANS, I_FIRST, "testseuser");
	helper_seuser_get_set_name(SH_TRANS, I_FIRST, "");
	helper_seuser_get_set_name(SH_TRANS, I_SECOND, "testseuser");
	helper_seuser_get_set_name(SH_TRANS, I_SECOND, "");
}

/* Function seuser_get_sename, seuser_set_sename */
void helper_seuser_get_set_sename(level_t level, int seuser_index,
				  const char *sename)
{
	semanage_seuser_t *user;
	const char *old_sename = NULL;
	const char *new_sename = NULL;

	/* setup */
	setup_handle(level);
	user = get_seuser_nth(seuser_index);

	/* test */
	old_sename = semanage_seuser_get_sename(user);
	CU_ASSERT_PTR_NOT_NULL(old_sename);
	CU_ASSERT(semanage_seuser_set_sename(sh, user, sename) >= 0);

	new_sename = semanage_seuser_get_sename(user);
	CU_ASSERT_PTR_NOT_NULL(new_sename);
	assert(new_sename);
	CU_ASSERT_STRING_EQUAL(new_sename, sename);

	/* cleanup */
	semanage_seuser_free(user);
	cleanup_handle(level);
}

void test_seuser_get_set_sename(void)
{
	helper_seuser_get_set_sename(SH_CONNECT, I_FIRST,  "testseuser");
	helper_seuser_get_set_sename(SH_CONNECT, I_FIRST,  "");
	helper_seuser_get_set_sename(SH_CONNECT, I_SECOND, "testseuser");
	helper_seuser_get_set_sename(SH_CONNECT, I_SECOND, "");
	helper_seuser_get_set_sename(SH_TRANS, I_FIRST,  "testseuser");
	helper_seuser_get_set_sename(SH_TRANS, I_FIRST,  "");
	helper_seuser_get_set_sename(SH_TRANS, I_SECOND, "testseuser");
	helper_seuser_get_set_sename(SH_TRANS, I_SECOND, "");
}

/* Function seuser_get_mlsrange, seuser_set_mlsrange */
void helper_seuser_get_set_mlsrange(level_t level, int seuser_index,
				    const char *mlsrange)
{
	semanage_seuser_t *user;
	const char *old_mlsrange = NULL;
	const char *new_mlsrange = NULL;

	/* setup */
	setup_handle(level);
	user = get_seuser_nth(seuser_index);

	/* test */
	old_mlsrange = semanage_seuser_get_mlsrange(user);
	CU_ASSERT_PTR_NOT_NULL(old_mlsrange);
	CU_ASSERT(semanage_seuser_set_mlsrange(sh, user, mlsrange) >= 0);

	new_mlsrange = semanage_seuser_get_mlsrange(user);
	CU_ASSERT_PTR_NOT_NULL(new_mlsrange);
	assert(new_mlsrange);
	CU_ASSERT_STRING_EQUAL(new_mlsrange, mlsrange);

	/* cleanup */
	semanage_seuser_free(user);
	cleanup_handle(level);
}

void test_seuser_get_set_mlsrange(void)
{
	helper_seuser_get_set_mlsrange(SH_CONNECT, I_FIRST,  "testmlsrange");
	helper_seuser_get_set_mlsrange(SH_CONNECT, I_FIRST,  "");
	helper_seuser_get_set_mlsrange(SH_CONNECT, I_SECOND, "testmlsrange");
	helper_seuser_get_set_mlsrange(SH_CONNECT, I_SECOND, "");
	helper_seuser_get_set_mlsrange(SH_TRANS, I_FIRST,  "testmlsrange");
	helper_seuser_get_set_mlsrange(SH_TRANS, I_FIRST,  "");
	helper_seuser_get_set_mlsrange(SH_TRANS, I_SECOND, "testmlsrange");
	helper_seuser_get_set_mlsrange(SH_TRANS, I_SECOND, "");
}

/* Function seuser_create */
void helper_seuser_create(level_t level)
{
	semanage_seuser_t *seuser;

	/* setup */
	setup_handle(level);

	/* test */
	CU_ASSERT(semanage_seuser_create(sh, &seuser) >= 0);
	CU_ASSERT_PTR_NULL(semanage_seuser_get_name(seuser));
	CU_ASSERT_PTR_NULL(semanage_seuser_get_sename(seuser));
	CU_ASSERT_PTR_NULL(semanage_seuser_get_mlsrange(seuser));

	/* cleanup */
	semanage_seuser_free(seuser);
	cleanup_handle(level);
}

void test_seuser_create(void)
{
	helper_seuser_create(SH_NULL);
	helper_seuser_create(SH_HANDLE);
	helper_seuser_create(SH_CONNECT);
	helper_seuser_create(SH_TRANS);
}

/* Function seuser_clone */
void helper_seuser_clone(level_t level, int seuser_index)
{
	semanage_seuser_t *seuser;
	semanage_seuser_t *seuser_clone;
	const char *str;
	const char *str_clone;

	/* setup */
	setup_handle(level);
	seuser = get_seuser_nth(seuser_index);

	/* test */
	CU_ASSERT(semanage_seuser_clone(sh, seuser, &seuser_clone) >= 0);

	str = semanage_seuser_get_name(seuser);
	str_clone = semanage_seuser_get_name(seuser_clone);
	CU_ASSERT_STRING_EQUAL(str, str_clone);

	str = semanage_seuser_get_sename(seuser);
	str_clone = semanage_seuser_get_sename(seuser_clone);
	CU_ASSERT_STRING_EQUAL(str, str_clone);

	str = semanage_seuser_get_mlsrange(seuser);
	str_clone = semanage_seuser_get_mlsrange(seuser_clone);
	CU_ASSERT_STRING_EQUAL(str, str_clone);

	/* cleanup */
	semanage_seuser_free(seuser);
	semanage_seuser_free(seuser_clone);
	cleanup_handle(level);
}

void test_seuser_clone(void)
{
	helper_seuser_clone(SH_CONNECT, I_FIRST);
	helper_seuser_clone(SH_CONNECT, I_SECOND);
	helper_seuser_clone(SH_TRANS, I_FIRST);
	helper_seuser_clone(SH_TRANS, I_SECOND);
}

/* Function seuser_query */
void helper_seuser_query(level_t level, const char *seuser_str, int exp_result)
{
	semanage_seuser_key_t *key;
	semanage_seuser_t *response;

	/* setup */
	setup_handle(level);
	key = get_seuser_key_from_str(seuser_str);

	/* test */
	if (exp_result < 0) {
		CU_ASSERT(semanage_seuser_query(sh, key, &response) < 0);
	}
	else {
		CU_ASSERT(semanage_seuser_query(sh, key, &response) >= 0);
		const char *name = semanage_seuser_get_name(response);
		CU_ASSERT_STRING_EQUAL(name, seuser_str);
	}

	/* cleanup */
	semanage_seuser_key_free(key);
	cleanup_handle(level);
}

void test_seuser_query(void)
{
	helper_seuser_query(SH_CONNECT, SEUSER, 1);
	helper_seuser_query(SH_CONNECT, SEUSER2, 1);
	helper_seuser_query(SH_CONNECT, SEUSER_DEFAULT, 1);
	helper_seuser_query(SH_CONNECT, SEUSER_NONEXISTENT, -1);
	helper_seuser_query(SH_TRANS, SEUSER, 1);
	helper_seuser_query(SH_TRANS, SEUSER2, 1);
	helper_seuser_query(SH_TRANS, SEUSER_DEFAULT, 1);
	helper_seuser_query(SH_TRANS, SEUSER_NONEXISTENT, -1);
}

/* Function seuser_exists */
void helper_seuser_exists(level_t level, char * seuser_str, int exp_response)
{
	semanage_seuser_key_t *key;
	int response;

	/* setup */
	setup_handle(level);
	key = get_seuser_key_from_str(seuser_str);

	/* test */
	CU_ASSERT(semanage_seuser_exists(sh, key, &response) >= 0);
	CU_ASSERT(response == exp_response);

	/* cleanup */
	semanage_seuser_key_free(key);
	cleanup_handle(level);
}

void test_seuser_exists(void)
{
	helper_seuser_exists(SH_CONNECT, (char *) SEUSER, 1);
	helper_seuser_exists(SH_CONNECT, (char *) SEUSER2, 1);
	helper_seuser_exists(SH_CONNECT, (char *) SEUSER_DEFAULT, 1);
	helper_seuser_exists(SH_CONNECT, (char *) SEUSER_NONEXISTENT, 0);
	helper_seuser_exists(SH_TRANS, (char *) SEUSER, 1);
	helper_seuser_exists(SH_TRANS, (char *) SEUSER2, 1);
	helper_seuser_exists(SH_TRANS, (char *) SEUSER_DEFAULT, 1);
	helper_seuser_exists(SH_TRANS, (char *) SEUSER_NONEXISTENT, 0);
}

/* Function seuser_count */
void test_seuser_count(void)
{
	unsigned int response;

	/* test with handle */
	setup_handle(SH_HANDLE);
	CU_ASSERT(semanage_seuser_count(sh, &response) < 0);
	CU_ASSERT(semanage_seuser_count(sh, NULL) < 0);

	/* test connected */
	helper_connect();
	CU_ASSERT(semanage_seuser_count(sh, &response) >= 0);
	CU_ASSERT(response == SEUSERS_COUNT);

	/* test in transaction */
	helper_begin_transaction();
	CU_ASSERT(semanage_seuser_count(sh, &response) >= 0);
	CU_ASSERT(response == SEUSERS_COUNT);

	/* cleanup */
	cleanup_handle(SH_TRANS);
}

/* Function seuser_iterate */
unsigned int counter_seuser_iterate = 0;

int handler_seuser_iterate(const semanage_seuser_t *record, void *varg)
{
	counter_seuser_iterate++;
	return 0;
}

void helper_seuser_iterate_invalid(void)
{
	setup_handle(SH_HANDLE);
	CU_ASSERT(semanage_seuser_iterate(sh, &handler_seuser_iterate, NULL)
		  < 0);
	CU_ASSERT(semanage_seuser_iterate(sh, NULL, NULL) < 0);
	cleanup_handle(SH_HANDLE);
}

void helper_seuser_iterate(level_t level)
{
	setup_handle(level);
	counter_seuser_iterate = 0;

	CU_ASSERT(semanage_seuser_iterate(sh, &handler_seuser_iterate, NULL)
		  >= 0);
	CU_ASSERT(counter_seuser_iterate == SEUSERS_COUNT);

	cleanup_handle(level);
}

void test_seuser_iterate(void)
{
	helper_seuser_iterate_invalid();
	helper_seuser_iterate(SH_CONNECT);
	helper_seuser_iterate(SH_TRANS);
}

/* Function seuser_list */
void helper_seuser_list_invalid(void)
{
	semanage_seuser_t **records;
	unsigned int count;

	/* setup */
	setup_handle(SH_HANDLE);

	/* test */
	CU_ASSERT(semanage_seuser_list(sh, &records, &count) < 0);

	CU_ASSERT(semanage_seuser_list(sh, NULL, &count) < 0);
	CU_ASSERT(semanage_seuser_list(sh, &records, NULL) < 0);

	/* cleanup */
	cleanup_handle(SH_HANDLE);
}

void helper_seuser_list(level_t level)
{
	semanage_seuser_t **records;
	unsigned int count;

	/* setup */
	setup_handle(level);

	/* test */
	CU_ASSERT(semanage_seuser_list(sh, &records, &count) >= 0);
	CU_ASSERT(count == SEUSERS_COUNT);

	for (unsigned int i = 0; i < count; i++) {
		CU_ASSERT_PTR_NOT_NULL(records[i]);
	}

	for (unsigned int i = 0; i < count; i++) {
		semanage_seuser_free(records[i]);
	}

	/* cleanup */
	free(records);
	cleanup_handle(level);
}

void test_seuser_list(void)
{
	helper_seuser_list_invalid();
	helper_seuser_list(SH_CONNECT);
	helper_seuser_list(SH_TRANS);
}

/* Function seuser_modify_local, seuser_del_local */
void helper_seuser_modify_del_local(level_t level, int seuser_index,
				    int exp_result)
{
	semanage_seuser_t *seuser;
	semanage_seuser_t *seuser_local;
	semanage_seuser_key_t *key = NULL;
	int result;

	/* setup */
	setup_handle(level);
	seuser = get_seuser_nth(seuser_index);
	CU_ASSERT(semanage_seuser_key_extract(sh, seuser, &key) >= 0);
	CU_ASSERT_PTR_NOT_NULL(key);

	/* test */
	result = semanage_seuser_modify_local(sh, key, seuser);

	if (exp_result < 0) {
		CU_ASSERT(result < 0);
	}
	else {
		CU_ASSERT(result >= 0);

		/* write changes to file */
		if (level == SH_TRANS) {
			helper_commit();
			helper_begin_transaction();
		}

		CU_ASSERT(semanage_seuser_query_local(sh, key, &seuser_local)
			  >= 0);
		CU_ASSERT(semanage_seuser_compare2(seuser_local, seuser) == 0);
		CU_ASSERT(semanage_seuser_del_local(sh, key) >= 0);
		CU_ASSERT(semanage_seuser_query_local(sh, key, &seuser_local)
			  < 0);
	}

	/* cleanup */
	semanage_seuser_key_free(key);
	semanage_seuser_free(seuser);
	cleanup_handle(level);
}

void test_seuser_modify_del_local(void)
{
	helper_seuser_modify_del_local(SH_CONNECT, I_FIRST, -1);
	helper_seuser_modify_del_local(SH_CONNECT, I_SECOND, -1);
	helper_seuser_modify_del_local(SH_TRANS, I_FIRST, 1);
	helper_seuser_modify_del_local(SH_TRANS, I_SECOND, 1);
}

/* Function seuser_query_local */
void test_seuser_query_local(void)
{
	semanage_seuser_key_t *key = NULL;
	semanage_seuser_t *response = NULL;

	/* connect */
	setup_handle(SH_CONNECT);
	key = get_seuser_key_nth(I_FIRST);

	CU_ASSERT(semanage_seuser_query_local(sh, key, &response) < 0);
	CU_ASSERT_PTR_NULL(response);

	semanage_seuser_key_free(key);
	cleanup_handle(SH_CONNECT);

	/* transaction */
	setup_handle(SH_TRANS);
	key = get_seuser_key_nth(I_FIRST);

	CU_ASSERT(semanage_seuser_query_local(sh, key, &response) < 0);
	CU_ASSERT_PTR_NULL(response);

	add_local_seuser(I_FIRST);
	CU_ASSERT(semanage_seuser_query_local(sh, key, &response) >= 0);
	CU_ASSERT_PTR_NOT_NULL(response);

	semanage_seuser_key_free(key);
	key = get_seuser_key_nth(I_SECOND);

	add_local_seuser(I_SECOND);
	CU_ASSERT(semanage_seuser_query_local(sh, key, &response) >= 0);
	CU_ASSERT_PTR_NOT_NULL(response);

	/* cleanup */
	semanage_seuser_key_free(key);
	delete_local_seuser(I_FIRST);
	delete_local_seuser(I_SECOND);
	cleanup_handle(SH_TRANS);
}

/* Function seuser_exists_local */
void test_seuser_exists_local(void)
{
	int response = -1;
	semanage_seuser_key_t *key;

	/* setup */
	setup_handle(SH_TRANS);
	key = get_seuser_key_nth(I_FIRST);

	/* test */
	CU_ASSERT(semanage_seuser_exists_local(sh, key, &response) >= 0);
	CU_ASSERT(response == 0);

	add_local_seuser(I_FIRST);
	response = -1;
	CU_ASSERT(semanage_seuser_exists_local(sh, key, &response) >= 0);
	CU_ASSERT(response == 1);

	delete_local_seuser(I_FIRST);
	response = -1;
	CU_ASSERT(semanage_seuser_exists_local(sh, key, &response) >= 0);
	CU_ASSERT(response == 0);

	response = -1;
	CU_ASSERT(semanage_seuser_exists_local(sh, NULL, &response) >= 0);
	CU_ASSERT(response == 0);

	/* cleanup */
	semanage_seuser_key_free(key);
	cleanup_handle(SH_TRANS);
}

/* Function seuser_count_local */
void test_seuser_count_local(void)
{
	unsigned int response;

	/* test with handle */
	setup_handle(SH_HANDLE);
	CU_ASSERT(semanage_seuser_count_local(sh, &response) < 0);

	/* test connected */
	helper_connect();
	CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
	CU_ASSERT(response == 0);

	/* test in transaction */
	helper_begin_transaction();
	CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
	CU_ASSERT(response == 0);

	add_local_seuser(I_FIRST);
	CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
	CU_ASSERT(response == 1);

	add_local_seuser(I_SECOND);
	CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
	CU_ASSERT(response == 2);

	delete_local_seuser(I_SECOND);
	CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
	CU_ASSERT(response == 1);

	/* cleanup */
	delete_local_seuser(I_FIRST);
	cleanup_handle(SH_TRANS);
}

/* Function seuser_iterate_local */
unsigned int counter_seuser_iterate_local = 0;

int handler_seuser_iterate_local(const semanage_seuser_t *record, void *varg)
{
	counter_seuser_iterate_local++;
	return 0;
}

void test_seuser_iterate_local(void)
{
	/* test with handle */
	helper_handle_create();

	CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
						NULL) < 0);
	CU_ASSERT(semanage_seuser_iterate_local(sh, NULL, NULL) < 0);

	/* test connected */
	helper_connect();
	counter_seuser_iterate_local = 0;
	CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
						NULL) >= 0);
	CU_ASSERT(counter_seuser_iterate_local == 0);

	/* test in transaction */
	helper_begin_transaction();
	counter_seuser_iterate_local = 0;
	CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
						NULL) >= 0);
	CU_ASSERT(counter_seuser_iterate_local == 0);

	add_local_seuser(I_FIRST);
	counter_seuser_iterate_local = 0;
	CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
						NULL) >= 0);
	CU_ASSERT(counter_seuser_iterate_local == 1);

	add_local_seuser(I_SECOND);
	counter_seuser_iterate_local = 0;
	CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
						NULL) >= 0);
	CU_ASSERT(counter_seuser_iterate_local == 2);

	/* cleanup */
	delete_local_seuser(I_FIRST);
	delete_local_seuser(I_SECOND);
	cleanup_handle(SH_TRANS);
}

/* Function seuser_list_local */
void test_seuser_list_local(void)
{
	semanage_seuser_t **records;
	unsigned int count;

	/* test with handle */
	helper_handle_create();
	CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) < 0);
	CU_ASSERT(semanage_seuser_list_local(sh, NULL, &count) < 0);
	CU_ASSERT(semanage_seuser_list_local(sh, &records, NULL) < 0);

	/* test connected */
	helper_connect();
	CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) >= 0);
	CU_ASSERT(count == 0);

	/* test in transaction */
	helper_begin_transaction();
	CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) >= 0);
	CU_ASSERT(count == 0);

	add_local_seuser(I_FIRST);
	CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) >= 0);
	CU_ASSERT(count == 1);
	CU_ASSERT_PTR_NOT_NULL(records[0]);

	add_local_seuser(I_SECOND);
	CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) >= 0);
	CU_ASSERT(count == 2);
	CU_ASSERT_PTR_NOT_NULL(records[0]);
	CU_ASSERT_PTR_NOT_NULL(records[1]);

	/* cleanup */
	delete_local_seuser(I_FIRST);
	delete_local_seuser(I_SECOND);
	cleanup_handle(SH_TRANS);
}
