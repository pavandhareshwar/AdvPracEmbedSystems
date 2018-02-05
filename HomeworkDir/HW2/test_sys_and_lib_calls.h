#ifndef _TEST_SYS_AND_LIB_CALLS_H_
#define _TEST_SYS_AND_LIB_CALLS_H_

#define PRINT(fmt, args...) fprintf(stderr, "%s :%d :%s() " fmt, __FILE__, __LINE__, __func__,  ##args)

#endif
