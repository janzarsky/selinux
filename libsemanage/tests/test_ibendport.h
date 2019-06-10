#ifndef __TEST_IBENDPORT_H__
#define __TEST_IBENDPORT_H__

#include <CUnit/Basic.h>

int ibendport_test_init(void);
int ibendport_test_cleanup(void);
int ibendport_add_tests(CU_pSuite suite);

#endif
