/* Authors: Christopher Ashworth <cashworth@tresys.com>
 *
 * Copyright (C) 2006 Tresys Technology, LLC
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fts.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <CUnit/Basic.h>

#include "semanage/semanage.h"

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

typedef enum { SH_NULL, SH_HANDLE, SH_CONNECT, SH_TRANS } level_t;

void test_msg_handler(void *varg, semanage_handle_t * handle, const char *fmt,
		      ...);

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

#endif
