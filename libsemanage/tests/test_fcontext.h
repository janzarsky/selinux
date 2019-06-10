#ifndef __TEST_FCONTEXT_H__
#define __TEST_FCONTEXT_H__

#include <CUnit/Basic.h>

int fcontext_test_init(void);
int fcontext_test_cleanup(void);
int fcontext_add_tests(CU_pSuite suite);

#endif
