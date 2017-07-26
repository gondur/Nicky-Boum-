
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"

#define TAG_CRUN 0x4352554E
#define BLOCK_SIZE 0x20000

static uint8 decodeBlock[BLOCK_SIZE];

static uint8 *crun_decode(uint8 *a0, uint32 *size) {
	const uint8 *src = a0;
	int d1, d3, d4;
	uint32 d0, d2, d5;
	uint8 *a1, *a2;
	int cf, xf;
	
	a2 = a0;
	a1 = a0 + 0xC;
	d0 = read_uint32BE(a0 + 4); /* packed_size */
	d1 = read_uint32BE(a0 + 8); /* unpacked_size */
	assert(d1 < BLOCK_SIZE);
	*size = d1;
	d2 = d0;
	a0 -= 8;
	do {
		*a0++ = *a1++;
	} while (--d2 != 0);
	a0 = a2;
	a1 = a0;
	a0 -= 8;
	a2 = a1 + d1;
	a0 += d0;

	a0 -= 4;
	d5 = read_uint32BE(a0);
	a0 -= 4;
	d0 = read_uint32BE(a0);
	d5 ^= d0;

loc_1CFE:
	xf = cf = (d0 & 1) != 0;
	d0 >>= 1;
	if (d0 == 0) {
		a0 -= 4;
		d0 = read_uint32BE(a0);
		d5 ^= d0;
		xf = cf = (d0 & 1) != 0;
		d0 = 0x80000000 | (d0 >> 1);
	}
	if (cf) goto loc_1D68;
	d1 = 8;
	d3 = 1;
	xf = cf = (d0 & 1) != 0;
	d0 >>= 1;
	if (d0 == 0) {
		a0 -= 4;
		d0 = read_uint32BE(a0);
		d5 ^= d0;
		xf = cf = (d0 & 1) != 0;
		d0 = 0x80000000 | (d0 >> 1);
	}
	if (cf) goto loc_1DB6;
	d1 = 3;
	d4 = 0;

loc_1D28:
	--d1;
	d2 = 0;
	do {
		xf = cf = (d0 & 1) != 0;
		d0 >>= 1;
		if (d0 == 0) {
			a0 -= 4;
			d0 = read_uint32BE(a0);
			d5 ^= d0;
			xf = cf = (d0 & 1) != 0;
			d0 = 0x80000000 | (d0 >> 1);
		}
		cf = (d2 & 0x80000000) != 0;
		d2 <<= 1;
		if (xf) d2 |= 1;
		xf = cf;
	} while (--d1 != -1);
	d3 = d2 + d4;

	do {
		d1 = 7;
		do {
			xf = cf = (d0 & 1) != 0;
			d0 >>= 1;
			if (d0 == 0) {
				a0 -= 4;
				d0 = read_uint32BE(a0);
				d5 ^= d0;
				xf = cf = (d0 & 1) != 0;
				d0 = 0x80000000 | (d0 >> 1);
			}
			cf = (d2 & 0x80000000) != 0;
			d2 <<= 1;
			if (xf) d2 |= 1;
			xf = cf;
		} while (--d1 != -1);
		--a2;
		assert(a2 >= src);
		*a2 = d2;
	} while (--d3 != -1);
	goto loc_1DD8;

loc_1D68:
	d1 = 2 - 1;
	d2 = 0;
	do {
		xf = cf = (d0 & 1) != 0;
		d0 >>= 1;
		if (d0 == 0) {
			a0 -= 4;
			d0 = read_uint32BE(a0);
			d5 ^= d0;
			xf = cf = (d0 & 1) != 0;
			d0 = 0x80000000 | (d0 >> 1);
		}
		cf = (d2 & 0x80000000) != 0;
		d2 <<= 1;
		if (xf) d2 |= 1;
		xf = cf;
	} while (--d1 != -1);
	if (d2 < 2) {
		d1 = 9 + d2;
		d2 += 2;
		d3 = d2;
		goto loc_1DB6;
	}
	if (d2 == 3) {
		d1 = 8;
		d4 = 8;
		goto loc_1D28;
	}
	d1 = 8 - 1;
	d2 = 0;
	do {
		xf = cf = (d0 & 1) != 0;
		d0 >>= 1;
		if (d0 == 0) {
			a0 -= 4;
			d0 = read_uint32BE(a0);
			d5 ^= d0;
			xf = cf = (d0 & 1) != 0;
			d0 = 0x80000000 | (d0 >> 1);
		}
		cf = (d2 & 0x80000000) != 0;
		d2 <<= 1;
		if (xf) d2 |= 1;
		xf = cf;
	} while (--d1 != -1);
	d3 = d2;
	d1 = 0xC;
	goto loc_1DB6;

loc_1DB6:
	--d1;
	d2 = 0;
	do {
		xf = cf = (d0 & 1) != 0;
		d0 >>= 1;
		if (d0 == 0) {
			a0 -= 4;
			d0 = read_uint32BE(a0);
			d5 ^= d0;
			xf = cf = (d0 & 1) != 0;
			d0 = 0x80000000 | (d0 >> 1);
		}
		cf = (d2 & 0x80000000) != 0;
		d2 <<= 1;
		if (xf) d2 |= 1;
		xf = cf;
	} while (--d1 != -1);
	do {
		--a2;
		assert(a2 >= src);
		*a2 = *(a2 + d2);
	} while (--d3 != -1);
	
loc_1DD8:
	if (a1 < a2) goto loc_1CFE;
	return a1;
}

int main(int argc, char *argv[]) {
	if (argc >= 2) {
		FILE *fp = fopen(argv[1], "rb");
		if (fp) {
			uint32 size;
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			fread(decodeBlock, 1, size, fp);
			fclose(fp);
			if (read_uint32BE(decodeBlock) == TAG_CRUN) {
				FILE *fo;
				uint8 *dst = crun_decode(decodeBlock, &size);
				char filename[20];
				sprintf(filename, "%s.decrun", argv[1]);
				fo = fopen(filename, "wb");
				if (fo) {
					fwrite(dst, 1, size, fo);
					fclose(fo);
				}
			}
		}
	}
	return 0;	
}
