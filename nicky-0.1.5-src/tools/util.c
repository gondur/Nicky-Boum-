
#include "util.h"

#if defined SYS_LITTLE_ENDIAN

uint16 read_uint16LE(const void *ptr) {
	return *(const uint16 *)ptr;
}

uint32 read_uint32LE(const void *ptr) {
	return *(const uint32 *)ptr;
}

uint16 read_uint16BE(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[0] << 8) | b[1];
}

uint32 read_uint32BE(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

#elif defined SYS_BIG_ENDIAN

uint16 read_uint16LE(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[1] << 8) | b[0];
}

uint32 read_uint32LE(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
}

uint16 read_uint16BE(const void *ptr) {
	return *(const uint16 *)ptr;
}

uint32 read_uint32BE(const void *ptr) {
	return *(const uint32 *)ptr;
}

#else

#error No endianness defined

#endif
