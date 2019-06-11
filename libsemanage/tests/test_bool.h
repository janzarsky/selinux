#ifndef __TEST_BOOL_H__
#define __TEST_BOOL_H__

#include <CUnit/Basic.h>
#include "semanage/semanage.h"

int bool_test_init(void);
int bool_test_cleanup(void);
int bool_add_tests(CU_pSuite suite);

#endif
