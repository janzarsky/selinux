#ifndef __TEST_IFACE_H__
#define __TEST_IFACE_H__

#include <CUnit/Basic.h>

int iface_test_init(void);
int iface_test_cleanup(void);
int iface_add_tests(CU_pSuite suite);

#endif
