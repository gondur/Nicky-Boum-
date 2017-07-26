
#ifndef __UTIL_H__
#define __UTIL_H__

typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned long uint32;
typedef signed long int32;

extern uint16 read_uint16LE(const void *ptr);
extern uint32 read_uint32LE(const void *ptr);
extern uint16 read_uint16BE(const void *ptr);
extern uint32 read_uint32BE(const void *ptr);

#endif /* __UTIL_H__ */
