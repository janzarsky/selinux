#ifndef __TEST_BOOL_H__
#define __TEST_BOOL_H__

#include <CUnit/Basic.h>
#include "semanage/semanage.h"

int bool_test_init(void);
int bool_test_cleanup(void);
int bool_add_tests(CU_pSuite suite);

semanage_bool_t *get_bool_nth(int index);
semanage_bool_t *get_bool_new(void);
semanage_bool_key_t *get_bool_key_nth(int index);
semanage_bool_key_t *get_bool_key_from_str(const char *str);

#endif
