#ifndef __TEST_PORT_H__
#define __TEST_PORT_H__

#include <CUnit/Basic.h>

int port_test_init(void);
int port_test_cleanup(void);
int port_add_tests(CU_pSuite suite);

#endif
