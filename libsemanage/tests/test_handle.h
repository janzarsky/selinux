#ifndef __TEST_HANDLE_H__
#define __TEST_HANDLE_H__

#include <CUnit/Basic.h>

int handle_test_init(void);
int handle_test_cleanup(void);
int handle_add_tests(CU_pSuite suite);

#endif
