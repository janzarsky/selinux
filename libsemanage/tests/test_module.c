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

#include "utilities.h"
#include "test_module.h"

#define CIL_NAME "test_cil_module"
#define CIL_LANG_EXT "cil"
#define CIL_FILE "test-modules/" CIL_NAME "." CIL_LANG_EXT
#define CIL_SOURCE "(boolean " CIL_NAME " true)\n"

#define BZIP_NAME "test_bzip_module"
#define BZIP_LANG_EXT "cil"
#define BZIP_FILE "test-modules/" BZIP_NAME "." BZIP_LANG_EXT
#define BZIP_SOURCE "(boolean my_bzip_module true)\n"

#define BZIP_INV_FILE "test-modules/test_invalid_bzip_module"

#define HLL_NAME "test_hll_module"
#define HLL_LANG_EXT "pp"
#define HLL_FILE "test-modules/" HLL_NAME "." HLL_LANG_EXT
char *HLL_DATA = NULL;
size_t HLL_DATA_LEN = 0;

#define CIL_INV_NAME "test_invalid_module"
#define CIL_INV_FILE "test-modules/" CIL_INV_NAME

#define CIL_PRIO_NAME "test_cil_module"
#define CIL_PRIO_FILE "test-modules/priority/" CIL_PRIO_NAME ".cil"

/* modules.h */
void test_module_list(void);
void test_module_install_invalid(void);
void test_module_install_cil(void);
void test_module_install_hll(void);
void test_module_install_file_invalid(void);
void test_module_install_file_cil(void);
void test_module_install_file_bzip(void);
void test_module_install_file_hll(void);
void test_module_install_file_overridden(void);
void test_module_install_file_override(void);
void test_module_install_file_conflict(void);
void test_module_install_info(void);
void test_module_remove(void);
void test_module_remove_key(void);
void test_module_extract_invalid(void);
void test_module_extract_cil(void);
void test_module_extract_bzip(void);
void test_module_list_all(void);
void test_module_get_name(void);
void test_module_get_module_info(void);
void test_module_info_create(void);
void test_module_info_get_set_priority_invalid(void);
void test_module_info_get_set_priority(void);
void test_module_info_get_set_name_invalid(void);
void test_module_info_get_set_name(void);
void test_module_info_get_set_lang_ext_invalid(void);
void test_module_info_get_set_lang_ext(void);
void test_module_info_get_set_enabled_invalid(void);
void test_module_info_get_set_enabled(void);
void test_module_key_create(void);
void test_module_key_get_set_name(void);
void test_module_key_get_set_priority(void);
void test_module_get_set_enabled(void);

extern semanage_handle_t *sh;

int read_hll_module(void)
{
	FILE *pp_file = fopen(HLL_FILE, "r");
	if (pp_file == NULL)
		return -1;

	fseek(pp_file, 0, SEEK_END);
	HLL_DATA_LEN = ftell(pp_file);
	rewind(pp_file);

	HLL_DATA = malloc(HLL_DATA_LEN);
	if (HLL_DATA == NULL) {
		fclose(pp_file);
		return -1;
	}

	if (fread(HLL_DATA, 1, HLL_DATA_LEN, pp_file) != HLL_DATA_LEN) {
		fclose(pp_file);
		return -1;
	}

	fclose(pp_file);
	return 0;
}

int module_test_init(void)
{
	if (create_test_store() < 0) {
		fprintf(stderr, "Could not create test store\n");
		goto err;
	}

	if (write_test_policy_from_file("test_module.policy") < 0) {
		fprintf(stderr, "Could not write test policy\n");
		goto err;
	}

	if (write_test_policy_src("test_module.cil.bz2") < 0) {
		fprintf(stderr, "Could not write test policy source\n");
		goto err;
	}

	if (read_hll_module() < 0) {
		fprintf(stderr, "Could not read " HLL_FILE "\n");
		goto err;
	}

	return 0;

err:
	destroy_test_store();
	return 1;
}

int module_test_cleanup(void)
{
	if (destroy_test_store() < 0) {
		fprintf(stderr, "Could not destroy test store\n");
		return 1;
	}

	return 0;
}

int module_add_tests(CU_pSuite suite)
{
	CU_add_test(suite, "module_install (invalid)",
		    test_module_install_invalid);
	CU_add_test(suite, "module_install (cil)", test_module_install_cil);
	CU_add_test(suite, "module_install (hll)", test_module_install_hll);
	CU_add_test(suite, "module_install_file (invalid)",
		    test_module_install_file_invalid);
	CU_add_test(suite, "module_install_file (cil)",
		    test_module_install_file_cil);
	CU_add_test(suite, "module_install_file (bzip)",
		    test_module_install_file_bzip);
	CU_add_test(suite, "module_install_file (hll)",
		    test_module_install_file_hll);
	CU_add_test(suite, "module_install_file (overridden)",
		    test_module_install_file_overridden);
	CU_add_test(suite, "module_install_file (override)",
		    test_module_install_file_override);
	CU_add_test(suite, "module_install_file (conflict)",
		    test_module_install_file_conflict);
	CU_add_test(suite, "module_install_info", test_module_install_info);
	CU_add_test(suite, "module_remove", test_module_remove);
	CU_add_test(suite, "module_remove_key", test_module_remove_key);
	CU_add_test(suite, "module_extract (invalid)",
		    test_module_extract_invalid);
	CU_add_test(suite, "module_extract (cil)", test_module_extract_cil);
	CU_add_test(suite, "module_extract (bzip)", test_module_extract_bzip);
	CU_add_test(suite, "module_list_all", test_module_list_all);
	CU_add_test(suite, "module_list", test_module_list);
	CU_add_test(suite, "module_get_name", test_module_get_name);
	CU_add_test(suite, "module_get_module_info",
		    test_module_get_module_info);
	CU_add_test(suite, "module_info_create", test_module_info_create);
	CU_add_test(suite, "module_info_get_set_priority (invalid)",
		    test_module_info_get_set_priority_invalid);
	CU_add_test(suite, "module_info_get_set_priority",
		    test_module_info_get_set_priority);
	CU_add_test(suite, "module_info_get_set_name (invalid)",
		    test_module_info_get_set_name_invalid);
	CU_add_test(suite, "module_info_get_set_name",
		    test_module_info_get_set_name);
	CU_add_test(suite, "module_info_get_set_lang_ext (invalid)",
		    test_module_info_get_set_lang_ext_invalid);
	CU_add_test(suite, "module_info_get_set_lang_ext",
		    test_module_info_get_set_lang_ext);
	CU_add_test(suite, "module_info_get_set_enabled (invalid)",
		    test_module_info_get_set_enabled_invalid);
	CU_add_test(suite, "module_info_get_set_enabled",
		    test_module_info_get_set_enabled);
	CU_add_test(suite, "module_key_create", test_module_key_create);
	CU_add_test(suite, "module_key_get_set_name",
		    test_module_key_get_set_name);
	CU_add_test(suite, "module_key_get_set_priority",
		    test_module_key_get_set_priority);
	CU_add_test(suite, "module_get_set_enabled",
		    test_module_get_set_enabled);

	return 0;
}

/* Helpers */

semanage_module_info_t *get_module_info_nth(int index,
					semanage_module_info_t **modinfo_free)
{
	int result;
	semanage_module_info_t *records = NULL;
	semanage_module_info_t *modinfo;
	int count;

	if (index == I_NULL)
		return NULL;

	result = semanage_module_list_all(sh, &records, &count);

	CU_ASSERT_FATAL(result >= 0);
	CU_ASSERT_FATAL(records != NULL);
	CU_ASSERT_FATAL(count >= index + 1);

	modinfo = semanage_module_list_nth(records, index);

	CU_ASSERT_FATAL(modinfo != NULL);

	for (unsigned int i = 0; i < (unsigned int) count; i++) {
		if (i != (unsigned int) index) {
			semanage_module_info_t *tmp;
			tmp = semanage_module_list_nth(records, i);
			semanage_module_info_destroy(sh, tmp);
		}
	}

	*modinfo_free = records;
	return modinfo;
}

semanage_module_key_t *get_module_key_nth(int index)
{
	semanage_module_key_t *modkey = NULL;
	semanage_module_info_t *modinfo = NULL;
	semanage_module_info_t *modinfo_free = NULL;
	const char *name = NULL;
	uint16_t priority = 0;

	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);

	modinfo = get_module_info_nth(index, &modinfo_free);

	CU_ASSERT(semanage_module_info_get_name(sh, modinfo, &name) >= 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, name) >= 0);

	CU_ASSERT(semanage_module_info_get_priority(sh, modinfo,
						    &priority) >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, priority) >= 0);

	free(modinfo_free);
	return modkey;
}

int module_exists(const char *name)
{
	semanage_module_info_t *modinfo = NULL;
	int num_modules = 0;
	int ret = 0;

	CU_ASSERT(semanage_module_list(sh, &modinfo, &num_modules) > 0);

	for (int i = 0; i < num_modules; i++) {
		const char *tmp_name = semanage_module_get_name(
					  semanage_module_list_nth(modinfo, i));
		if (!strcmp(name, tmp_name)) {
			ret = 1;
			break;
		}
	}

	for (int i = 0; i < num_modules; i++) {
		semanage_module_info_destroy(sh, semanage_module_list_nth(
								   modinfo, i));
	}

	free(modinfo);
	return ret;
}

/* Function semanage_module_install */
void test_module_install_invalid(void)
{
	/* not in transaction */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_install(sh, (char *) CIL_SOURCE,
					  strlen(CIL_SOURCE), (char *) CIL_NAME,
					  (char *) CIL_LANG_EXT) >= 0);
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);
	cleanup_handle(SH_CONNECT);

	/* invalid name */
	setup_handle(SH_TRANS);
	CU_ASSERT(semanage_module_install(sh, (char *) CIL_SOURCE,
					  strlen(CIL_SOURCE), (char *) "#invalid",
					  (char *) CIL_LANG_EXT) < 0);

	/* invalid lang ext */
	CU_ASSERT(semanage_module_install(sh, (char *) CIL_SOURCE,
					  strlen(CIL_SOURCE), (char *) CIL_NAME,
					  (char *) "#invalid") < 0);

	/* cleanup */
	cleanup_handle(SH_TRANS);
}

void test_module_install_cil(void)
{
	/* setup */
	setup_handle(SH_TRANS);

	/* test */
	CU_ASSERT(semanage_module_install(sh, (char *) CIL_SOURCE,
					  strlen(CIL_SOURCE), (char *) CIL_NAME,
					  (char *) CIL_LANG_EXT) >= 0);
	helper_commit();
	CU_ASSERT(module_exists(CIL_NAME));

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);
	cleanup_handle(SH_TRANS);
}

void test_module_install_hll(void)
{
	/* setup */
	setup_handle(SH_TRANS);

	/* test */
	CU_ASSERT(semanage_module_install(sh, HLL_DATA, HLL_DATA_LEN,
					  (char *) HLL_NAME,
					  (char *) HLL_LANG_EXT) >= 0);
	helper_commit();
	CU_ASSERT(module_exists(HLL_NAME));

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_module_remove(sh, (char *) HLL_NAME) >= 0);
	cleanup_handle(SH_TRANS);
}

/* Function semanage_module_install_file */
void test_module_install_file_invalid(void)
{
	/* disconnected */
	setup_handle(SH_CONNECT);
	helper_disconnect();

	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) < 0);

	cleanup_handle(SH_HANDLE);

	/* not in transaction */
	setup_handle(SH_CONNECT);

	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);

	/* nonexisting module */
	CU_ASSERT(semanage_module_install_file(sh, "nonexisting") < 0);

	/* module with invalid lang ext */
	CU_ASSERT(semanage_module_install_file(sh, CIL_INV_FILE) < 0);

	/* compressed module with invalid lang ext */
	CU_ASSERT(semanage_module_install_file(sh, BZIP_INV_FILE) < 0);

	cleanup_handle(SH_TRANS);
}

void test_module_install_file_cil(void)
{
	/* setup */
	setup_handle(SH_TRANS);

	/* test */
	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);
	helper_commit();
	CU_ASSERT(module_exists(CIL_NAME));

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);
	cleanup_handle(SH_TRANS);
}

void test_module_install_file_bzip(void)
{
	/* setup */
	setup_handle(SH_TRANS);

	/* test */
	CU_ASSERT(semanage_module_install_file(sh, BZIP_FILE) >= 0);
	helper_commit();
	CU_ASSERT(module_exists(BZIP_NAME));

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_module_remove(sh, (char *) BZIP_NAME) >= 0);
	cleanup_handle(SH_TRANS);
}

void test_module_install_file_hll(void)
{
	/* setup */
	setup_handle(SH_TRANS);
	CU_ASSERT(semanage_module_install_file(sh, HLL_FILE) >= 0);
	helper_commit();

	/* test */
	CU_ASSERT(module_exists(HLL_NAME));

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_module_remove(sh, (char *) HLL_NAME) >= 0);
	cleanup_handle(SH_TRANS);
}

void test_module_install_file_overridden(void)
{
	/* setup */
	setup_handle(SH_TRANS);

	CU_ASSERT(semanage_set_default_priority(sh, 400) >= 0);
	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);

	CU_ASSERT(semanage_set_default_priority(sh, 1) >= 0);
	CU_ASSERT(semanage_module_install_file(sh, CIL_PRIO_FILE) >= 0);

	helper_commit();

	/* test */
	/* TODO */

	/* cleanup */
	helper_begin_transaction();

	CU_ASSERT(semanage_set_default_priority(sh, 1) >= 0);
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_PRIO_NAME) >= 0);

	CU_ASSERT(semanage_set_default_priority(sh, 400) >= 0);
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);

	cleanup_handle(SH_TRANS);
}

void test_module_install_file_override(void)
{
	/* setup */
	setup_handle(SH_TRANS);

	/* test */
	CU_ASSERT(semanage_set_default_priority(sh, 400) >= 0);
	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);
	CU_ASSERT(semanage_set_default_priority(sh, 999) >= 0);
	CU_ASSERT(semanage_module_install_file(sh, CIL_PRIO_FILE) >= 0);
	helper_commit();

	/* TODO */

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_set_default_priority(sh, 999) >= 0);
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_PRIO_NAME) >= 0);
	CU_ASSERT(semanage_set_default_priority(sh, 400) >= 0);
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);
	cleanup_handle(SH_TRANS);
}

void test_module_install_file_conflict(void)
{
	/* setup */
	setup_handle(SH_TRANS);

	/* test */
	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);
	CU_ASSERT(semanage_module_install_file(sh, CIL_PRIO_FILE) >= 0);
	helper_commit();

	/* TODO */

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_PRIO_NAME) >= 0);
	cleanup_handle(SH_TRANS);
}

/* Function semanage_module_install_info */
void test_module_install_info(void)
{
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, 400) >= 0);
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo, CIL_NAME) >= 0);
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo,
						    CIL_LANG_EXT) >= 0);
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, 1) >= 0);

	/* test disconnected */
	helper_disconnect();
	CU_ASSERT(semanage_module_install_info(sh, modinfo, (char *) CIL_SOURCE,
					       strlen(CIL_SOURCE)) < 0);

	/* test not in transaction */
	helper_connect();
	CU_ASSERT(semanage_module_install_info(sh, modinfo, (char *) CIL_SOURCE,
					       strlen(CIL_SOURCE)) >= 0);
	helper_commit();
	CU_ASSERT(module_exists(CIL_NAME));

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);
	helper_commit();
	semanage_module_info_destroy(sh, modinfo);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_module_remove */
void test_module_remove(void)
{
	/* setup */
	setup_handle(SH_TRANS);
	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);
	helper_commit();

	/* test disconnected */
	helper_disconnect();
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) < 0);

	/* test not in transaction */
	helper_connect();
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);

	/* cleanup */
	cleanup_handle(SH_TRANS);
}

/* Function semanage_module_remove_key */
void test_module_remove_key(void)
{
	semanage_module_key_t *modkey = NULL;

	/* setup */
	setup_handle(SH_TRANS);
	CU_ASSERT(semanage_set_default_priority(sh, 400) >= 0);
	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);
	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, CIL_NAME) >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 400) >= 0);
	helper_commit();

	/* test disconnected */
	helper_disconnect();
	CU_ASSERT(semanage_module_remove_key(sh, modkey) < 0);

	/* test not in transaction */
	helper_connect();
	CU_ASSERT(semanage_module_remove_key(sh, modkey) >= 0);
	helper_commit();

	/* cleanup */
	semanage_module_key_destroy(sh, modkey);
	free(modkey);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_module_extract */
void test_module_extract_invalid(void)
{
	semanage_module_key_t *modkey = NULL;
	semanage_module_info_t *modinfo = NULL;
	void *mapped_data = NULL;
	size_t data_len = 0;

	/* setup */
	setup_handle(SH_CONNECT);
	modkey = get_module_key_nth(I_FIRST);
	helper_disconnect();

	/* test disconnected */
	CU_ASSERT(semanage_module_extract(sh, modkey, 0, &mapped_data,
					  &data_len, &modinfo) < 0);

	/* test in transaction */
	helper_connect();
	helper_begin_transaction();
	CU_ASSERT(semanage_module_key_set_name(sh, modkey,
					       "my_nonexisting_module") >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 400) >= 0);
	CU_ASSERT(semanage_module_extract(sh, modkey, 0, &mapped_data,
					  &data_len, &modinfo) < 0);

	/* cleanup */
	cleanup_handle(SH_TRANS);
}

void test_module_extract_cil(void)
{
	semanage_module_key_t *modkey = NULL;
	semanage_module_info_t *modinfo = NULL;
	void *mapped_data = NULL;
	size_t data_len = 0;
	const char *name = NULL;
	uint16_t priority = 42;
	const char *lang_ext = NULL;
	int enabled = 42;

	/* setup */
	setup_handle(SH_TRANS);
	CU_ASSERT(semanage_set_default_priority(sh, 400) >= 0);
	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);
	helper_commit();

	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, CIL_NAME) >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 400) >= 0);

	/* test */
	CU_ASSERT(semanage_module_extract(sh, modkey, 0, &mapped_data,
					  &data_len, &modinfo) >= 0);

	CU_ASSERT(mapped_data != NULL);
	CU_ASSERT(data_len == strlen(CIL_SOURCE));
	assert(mapped_data);
	CU_ASSERT(memcmp(mapped_data, CIL_SOURCE, data_len) == 0);

	CU_ASSERT(modinfo != NULL);
	CU_ASSERT(semanage_module_info_get_priority(sh, modinfo,
						    &priority) >= 0);
	CU_ASSERT(priority == 400);

	CU_ASSERT(semanage_module_info_get_name(sh, modinfo, &name) >= 0);
	CU_ASSERT_STRING_EQUAL(name, CIL_NAME);

	CU_ASSERT(semanage_module_info_get_lang_ext(sh, modinfo,
						    &lang_ext) >= 0);
	CU_ASSERT_STRING_EQUAL(lang_ext, CIL_LANG_EXT);

	CU_ASSERT(semanage_module_info_get_enabled(sh, modinfo, &enabled) >= 0);
	CU_ASSERT(enabled == 1);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

void test_module_extract_bzip(void)
{
	semanage_module_key_t *modkey = NULL;
	semanage_module_info_t *modinfo = NULL;
	void *mapped_data = NULL;
	size_t data_len = 0;
	const char *name = NULL;
	uint16_t priority = 42;
	const char *lang_ext = NULL;
	int enabled = 42;

	/* setup */
	setup_handle(SH_TRANS);
	CU_ASSERT(semanage_set_default_priority(sh, 400) >= 0);
	CU_ASSERT(semanage_module_install_file(sh, BZIP_FILE) >= 0);
	helper_commit();

	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, BZIP_NAME) >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 400) >= 0);

	/* test */
	CU_ASSERT(semanage_module_extract(sh, modkey, 1, &mapped_data,
					  &data_len, &modinfo) >= 0);

	CU_ASSERT(mapped_data != NULL);
	CU_ASSERT(data_len == strlen(BZIP_SOURCE));
	assert(mapped_data);
	CU_ASSERT(memcmp(mapped_data, BZIP_SOURCE, data_len) == 0);

	CU_ASSERT(modinfo != NULL);
	CU_ASSERT(semanage_module_info_get_priority(sh, modinfo,
						    &priority) >= 0);
	CU_ASSERT(priority == 400);

	CU_ASSERT(semanage_module_info_get_name(sh, modinfo, &name) >= 0);
	CU_ASSERT_STRING_EQUAL(name, BZIP_NAME);

	CU_ASSERT(semanage_module_info_get_lang_ext(sh, modinfo,
						    &lang_ext) >= 0);
	CU_ASSERT_STRING_EQUAL(lang_ext, BZIP_LANG_EXT);

	CU_ASSERT(semanage_module_info_get_enabled(sh, modinfo, &enabled) >= 0);
	CU_ASSERT(enabled == 1);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_module_list_all, semanage_module_list_nth,
 * semanage_info_destroy
 */
void helper_module_list_all_invalid(void)
{
	semanage_module_info_t *records;
	int count = -1;

	setup_handle(SH_CONNECT);
	helper_disconnect();

	CU_ASSERT(semanage_module_list_all(sh, &records, &count) < 0);

	cleanup_handle(SH_HANDLE);
}

void helper_module_list_all(level_t level)
{
	semanage_module_info_t *records;
	int count = -1;

	/* setup */
	setup_handle(level);

	/* test */
	CU_ASSERT(semanage_module_list_all(sh, &records, &count) >= 0);

	/* TODO: check real count */
	CU_ASSERT(count > 0);

	for (int i = 0; i < count; i++) {
		semanage_module_info_t *info = semanage_module_list_nth(records,
									i);
		/* TODO: check real names */
		CU_ASSERT_STRING_NOT_EQUAL(semanage_module_get_name(info), "");
		semanage_module_info_destroy(sh, info);
	}

	/* cleanup */
	free(records);
	cleanup_handle(level);
}

void test_module_list_all(void)
{
	helper_module_list_all_invalid();
	helper_module_list_all(SH_CONNECT);
	helper_module_list_all(SH_TRANS);
}

/* Function semanage_module_list, semanage_module_list_nth,
 * semanage_info_datum_destroy, semanage_info_destroy
 */
void helper_module_list_invalid(void)
{
	semanage_module_info_t *records;
	int count = -1;

	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_disconnect(sh) >= 0);
	CU_ASSERT(semanage_module_list(sh, &records, &count) < 0);
	cleanup_handle(SH_HANDLE);
}

void helper_module_list(level_t level)
{
	semanage_module_info_t *records;
	int count = -1;

	/* setup */
	setup_handle(level);

	/* test */
	CU_ASSERT(semanage_module_list(sh, &records, &count) >= 0);

	/* TODO: check real count */
	CU_ASSERT(count > 0);

	for (int i = 0; i < count; i++) {
		semanage_module_info_t *info = semanage_module_list_nth(records, i);
		/* TODO: check real names */
		CU_ASSERT_STRING_NOT_EQUAL(semanage_module_get_name(info), "");
		semanage_module_info_destroy(sh, info);
	}

	/* cleanup */
	free(records);
	cleanup_handle(level);
}

void test_module_list(void)
{
	helper_module_list_invalid();
	helper_module_list(SH_CONNECT);
	helper_module_list(SH_TRANS);
}

/* Function semanage_module_get_name */
void test_module_get_name(void)
{
	const char *name = NULL;
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo,
						"module_name") >= 0);

	/* test */
	name = semanage_module_get_name(modinfo);
	CU_ASSERT_STRING_EQUAL(name, "module_name");

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_get_module_info */
void test_module_get_module_info(void)
{
	semanage_module_key_t *modkey = NULL;
	semanage_module_info_t *modinfo = NULL;
	const char *name = NULL;
	uint16_t priority = 42;

	/* setup */
	setup_handle(SH_HANDLE);
	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);

	/* test disconnected */
	helper_connect();
	helper_disconnect();
	CU_ASSERT(semanage_module_get_module_info(sh, modkey, &modinfo) < 0);

	/* test nonexisting module */
	helper_connect();
	CU_ASSERT(semanage_module_key_set_name(sh, modkey,
					       "test_nonexisting_module") >= 0);
	CU_ASSERT(semanage_module_get_module_info(sh, modkey, &modinfo) < 0);

	/* test existing module */
	helper_connect();
	helper_begin_transaction();
	CU_ASSERT(semanage_module_install_file(sh, CIL_FILE) >= 0);
	helper_commit();

	CU_ASSERT(semanage_module_key_set_name(sh, modkey, CIL_NAME) >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 400) >= 0);
	CU_ASSERT(semanage_module_get_module_info(sh, modkey, &modinfo) >= 0);

	CU_ASSERT(semanage_module_info_get_name(sh, modinfo, &name) >= 0);
	CU_ASSERT_STRING_EQUAL(name, CIL_NAME);

	CU_ASSERT(semanage_module_info_get_priority(sh, modinfo,
						    &priority) >= 0);
	CU_ASSERT(priority == 400);

	/* cleanup */
	helper_begin_transaction();
	CU_ASSERT(semanage_module_remove(sh, (char *) CIL_NAME) >= 0);
	semanage_module_key_destroy(sh, modkey);
	free(modkey);
	cleanup_handle(SH_TRANS);
}

/* Function semanage_module_get_name, semanage_module_info_destroy */
void helper_module_info_create(level_t level)
{
	semanage_module_info_t *modinfo;
	uint16_t priority = 42;
	const char *name = "asdf";
	const char *lang_ext = "asdf";
	int enabled = 42;

	/* setup */
	setup_handle(level);

	/* test */
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	CU_ASSERT(semanage_module_info_get_priority(sh, modinfo,
						    &priority) >= 0);
	CU_ASSERT(priority == 0);

	CU_ASSERT(semanage_module_info_get_name(sh, modinfo, &name) >= 0);
	CU_ASSERT(name == NULL);

	CU_ASSERT(semanage_module_info_get_lang_ext(sh, modinfo,
						    &lang_ext) >= 0);
	CU_ASSERT(lang_ext == NULL);

	CU_ASSERT(semanage_module_info_get_enabled(sh, modinfo, &enabled) >= 0);
	CU_ASSERT(enabled == -1);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(level);
}

void test_module_info_create(void)
{
	helper_module_info_create(SH_CONNECT);
	helper_module_info_create(SH_TRANS);
}

/* Function semanage_module_info_get_priority,
 * semanage_module_info_set_priority
 */
void test_module_info_get_set_priority_invalid(void)
{
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	/* test */
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, -42) < 0);
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, -1) < 0);
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, 0) < 0);
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, 1) >= 0);
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, 123) >= 0);
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, 999) >= 0);
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, 1000) < 0);
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, 1234) < 0);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

void test_module_info_get_set_priority(void)
{
	uint16_t priority = 42;
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	/* test */
	CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, 123) >= 0);
	CU_ASSERT(semanage_module_info_get_priority(sh, modinfo, &priority)
		  >= 0);
	CU_ASSERT(priority == 123);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_module_info_get_name, semanage_module_info_set_name */
void test_module_info_get_set_name_invalid(void)
{
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	/* test */
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo, "") < 0);
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo, "4asdf") < 0);
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo, "asdf%") < 0);
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo, "asdf.") < 0);
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo, "a") >= 0);
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo,
					      "asdf_asdf-1234.asdf.asdf") >= 0);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

void test_module_info_get_set_name(void)
{
	const char *name = NULL;
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	/* test */
	CU_ASSERT(semanage_module_info_set_name(sh, modinfo, "asdf") >= 0);
	CU_ASSERT(semanage_module_info_get_name(sh, modinfo, &name) >= 0);
	CU_ASSERT_STRING_EQUAL(name, "asdf");

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_module_info_get_lang_ext,
 * semanage_module_info_set_lang_ext
 */
void test_module_info_get_set_lang_ext_invalid(void)
{
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	/* test */
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo, "") < 0);
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo, "_asdf") < 0);
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo, "a#asdf") < 0);
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo, "as#asdf") < 0);
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo, "asdf#") < 0);
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo, "a") >= 0);
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo,
						    "asdf-1234_ASDF") >= 0);
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo,
						    "1234_asdf-ASDF") >= 0);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

void test_module_info_get_set_lang_ext(void)
{
	const char *lang_ext = NULL;
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	/* test */
	CU_ASSERT(semanage_module_info_set_lang_ext(sh, modinfo, "asdf") >= 0);
	CU_ASSERT(semanage_module_info_get_lang_ext(sh, modinfo,
						    &lang_ext) >= 0);
	CU_ASSERT_STRING_EQUAL(lang_ext, "asdf");

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_module_info_get_enabled,
 * semanage_module_info_set_enabled
 */
void test_module_info_get_set_enabled_invalid(void)
{
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	/* test */
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, -123) < 0);
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, -2) < 0);
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, -1) >= 0);
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, 0) >= 0);
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, 1) >= 0);
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, 2) < 0);
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, 123) < 0);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

void test_module_info_get_set_enabled(void)
{
	int enabled;
	semanage_module_info_t *modinfo = NULL;

	/* setup */
	setup_handle(SH_CONNECT);
	CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

	/* test */
	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, 0) >= 0);
	CU_ASSERT(semanage_module_info_get_enabled(sh, modinfo, &enabled) >= 0);
	CU_ASSERT(enabled == 0);

	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, 1) >= 0);
	CU_ASSERT(semanage_module_info_get_enabled(sh, modinfo, &enabled) >= 0);
	CU_ASSERT(enabled == 1);

	CU_ASSERT(semanage_module_info_set_enabled(sh, modinfo, -1) >= 0);
	CU_ASSERT(semanage_module_info_get_enabled(sh, modinfo, &enabled) >= 0);
	CU_ASSERT(enabled == -1);

	/* cleanup */
	CU_ASSERT(semanage_module_info_destroy(sh, modinfo) >= 0);
	free(modinfo);
	cleanup_handle(SH_CONNECT);
}

/* Function semanage_module_key_create, semanage_module_key_destroy */
void helper_module_key_create(level_t level)
{
	semanage_module_key_t *modkey = NULL;
	const char *name = NULL;
	uint16_t priority = 42;

	/* setup */
	setup_handle(level);

	/* test */
	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);

	CU_ASSERT(semanage_module_key_get_name(sh, modkey, &name) >= 0);
	CU_ASSERT(name == NULL);

	CU_ASSERT(semanage_module_key_get_priority(sh, modkey, &priority) >= 0);
	CU_ASSERT(priority == 0);

	/* cleanup */
	CU_ASSERT(semanage_module_key_destroy(sh, modkey) >= 0);
	free(modkey);
	cleanup_handle(level);
}

void test_module_key_create(void)
{
	helper_module_key_create(SH_CONNECT);
	helper_module_key_create(SH_TRANS);
}

/* Function semanage_module_key_get_name, semanage_module_key_set_name */
void helper_module_key_get_set_name_invalid(level_t level)
{
	semanage_module_key_t *modkey = NULL;

	/* setup */
	setup_handle(level);
	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);

	/* test */
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, "") < 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, "4asdf") < 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, "asdf%") < 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, "asdf.") < 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, "a") >= 0);
	CU_ASSERT(semanage_module_key_set_name(sh, modkey,
					      "asdf_asdf-1234.asdf.asdf") >= 0);

	/* cleanup */
	CU_ASSERT(semanage_module_key_destroy(sh, modkey) >= 0);
	free(modkey);
	cleanup_handle(level);
}

void helper_module_key_get_set_name(level_t level)
{
	semanage_module_key_t *modkey;
	const char *name = NULL;

	/* setup */
	setup_handle(level);
	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);

	/* test */
	CU_ASSERT(semanage_module_key_set_name(sh, modkey, "asdf") >= 0);
	CU_ASSERT(semanage_module_key_get_name(sh, modkey, &name) >= 0);

	CU_ASSERT(name != NULL);
	assert(name);
	CU_ASSERT_STRING_EQUAL(name, "asdf");

	/* cleanup */
	CU_ASSERT(semanage_module_key_destroy(sh, modkey) >= 0);
	free(modkey);
	cleanup_handle(level);
}

void test_module_key_get_set_name(void)
{
	helper_module_key_get_set_name_invalid(SH_CONNECT);
	helper_module_key_get_set_name(SH_CONNECT);
	helper_module_key_get_set_name(SH_CONNECT);
	helper_module_key_get_set_name_invalid(SH_TRANS);
	helper_module_key_get_set_name(SH_TRANS);
	helper_module_key_get_set_name(SH_TRANS);
}

/* Function semanage_module_key_get_priority,
 * semanage_module_key_set_priority
 */
void helper_module_key_get_set_priority_invalid(level_t level)
{
	semanage_module_key_t *modkey = NULL;

	/* setup */
	setup_handle(level);
	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);

	/* test */
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, -42) < 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, -1) < 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 0) < 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 1) >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 123) >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 999) >= 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 1000) < 0);
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 1234) < 0);

	/* cleanup */
	CU_ASSERT(semanage_module_key_destroy(sh, modkey) >= 0);
	free(modkey);
	cleanup_handle(level);
}

void helper_module_key_get_set_priority(level_t level)
{
	semanage_module_key_t *modkey;
	uint16_t priority = 42;

	/* setup */
	setup_handle(level);
	CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);

	/* test */
	CU_ASSERT(semanage_module_key_set_priority(sh, modkey, 200) >= 0);
	CU_ASSERT(semanage_module_key_get_priority(sh, modkey, &priority) >= 0);
	CU_ASSERT(priority == 200);

	/* cleanup */
	CU_ASSERT(semanage_module_key_destroy(sh, modkey) >= 0);
	free(modkey);
	cleanup_handle(level);
}

void test_module_key_get_set_priority(void)
{
	helper_module_key_get_set_priority_invalid(SH_CONNECT);
	helper_module_key_get_set_priority(SH_CONNECT);
	helper_module_key_get_set_priority(SH_CONNECT);
	helper_module_key_get_set_priority_invalid(SH_TRANS);
	helper_module_key_get_set_priority(SH_TRANS);
	helper_module_key_get_set_priority(SH_TRANS);
}

/* Function semanage_module_get_enabled, semanage_module_set_enabled */
void helper_module_get_set_enabled(level_t level, int index)
{
	semanage_module_key_t *modkey = NULL;
	int enabled_old = 42;
	int enabled_new = 42;
	int enabled = 42;

	/* setup */
	setup_handle(level);
	modkey = get_module_key_nth(index);
	CU_ASSERT(semanage_module_get_enabled(sh, modkey, &enabled_old) >= 0);
	CU_ASSERT(enabled_old == 0 || enabled_old == 1);
	enabled_new = !enabled_old;

	/* test */
	CU_ASSERT(semanage_module_set_enabled(sh, modkey, enabled_new) >= 0);
	CU_ASSERT(semanage_module_get_enabled(sh, modkey, &enabled) >= 0);
	CU_ASSERT(enabled == enabled_new);

	/* cleanup */
	CU_ASSERT(semanage_module_set_enabled(sh, modkey, enabled_old) >= 0);
	cleanup_handle(level);
}

void test_module_get_set_enabled(void)
{
	helper_module_get_set_enabled(SH_CONNECT, I_FIRST);
	helper_module_get_set_enabled(SH_TRANS, I_FIRST);
}
