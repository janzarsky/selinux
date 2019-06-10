#ifndef __TEST_USER_H__
#define __TEST_USER_H__

#include <CUnit/Basic.h>

int user_test_init(void);
int user_test_cleanup(void);
int user_add_tests(CU_pSuite suite);

#endif
