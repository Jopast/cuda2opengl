#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <stdint.h>

#define HIGHT_BIT_DEPTH

#ifdef HIGHT_BIT_DEPTH
typedef uint8_t    gpel_t;
typedef uchar3     gpel3_t;
#else

#endif

#endif // !_DEFINES_H_


