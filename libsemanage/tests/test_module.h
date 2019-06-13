#ifndef __TEST_MODULE_H__
#define __TEST_MODULE_H__

#include <CUnit/Basic.h>

int module_test_init(void);
int module_test_cleanup(void);
int module_add_tests(CU_pSuite suite);

#endif
