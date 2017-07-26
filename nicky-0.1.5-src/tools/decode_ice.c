
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"

typedef struct {
	uint8 *src;
	uint32 src_sz;
	uint8 *dst;
	uint32 dst_sz;
	uint8 *a4; /* beginning of unpacked buffer */
	uint8 *a5; /* end of packed buffer */
	uint8 *a6; /* end of unpacked buffer */
	uint32 d7; /* bits */
} decrunch_ctx_t;

static int ice_decruncher_helper3(decrunch_ctx_t *ctx) { /* get_1_bit */
	int cf = (ctx->d7 & 0x80000000) != 0;
	ctx->d7 <<= 1;
	if (ctx->d7 == 0) {
		ctx->d7 = (uint32)ctx->a5;
		if ((ctx->d7 & 1) == 0) {
			int inc = cf;
			ctx->a5 -= 4;
			ctx->d7 = read_uint32BE(ctx->a5);
			cf = (ctx->d7 & 0x80000000) != 0;
			ctx->d7 <<= 1;
			if (inc) {
				++ctx->d7;
			}
		} else {
			ctx->d7 = read_uint32BE(ctx->a5 - 5);
			ctx->d7 <<= 8;
			--ctx->a5;
			ctx->d7 |= *ctx->a5;
			ctx->a5 -= 3;
			cf = (ctx->d7 & 0x80000000) != 0;
			ctx->d7 <<= 1;
			ctx->d7 |= 1;
		}
	}
	return cf;
}

static uint16 decruncher_ice_helper4(decrunch_ctx_t *ctx, int16 d0) { /* get_d0_bits */
	uint16 d1 = 0;
	do {
		int cf = ice_decruncher_helper3(ctx);
		d1 <<= 1;
		if (cf) {
			++d1;
		}
	} while (--d0 != -1);
	return d1;
}


static uint32 _decruncher_ice_table1[] = { /* direkt_tab */
	0x7FFF000E,
	  0xFF0007,
	   0x70002,
	   0x30001,
	   0x30001,
	   
	0x10D,
	0xE,
	0x7,
	0x4,
	0x1
};

static void ice_decruncher_helper6(decrunch_ctx_t *ctx); /* strings */

static void ice_decruncher_helper2(decrunch_ctx_t *ctx) { /* normal_bytes */
	int cf = ice_decruncher_helper3(ctx);
	if (cf) {
		int16 d0;
		int16 d1 = 0;
		cf = ice_decruncher_helper3(ctx);
		if (cf) {
			int i = 4;
			uint32 *p = &_decruncher_ice_table1[5];
			assert(*p == 0x10D);
			do {
				--p;
				d0 = *p & 0xFFFF;
				d1 = decruncher_ice_helper4(ctx, d0);
				d0 = *p >> 16;
			} while (d1 == d0 && --i != -1);
			d1 += p[5];
		}
		do {
			--ctx->a6;
			--ctx->a5;
			*ctx->a6 = *ctx->a5;
		} while (--d1 != -1);
		assert(ctx->a5 >= ctx->src - 10);
	}
	if (ctx->a6 > ctx->a4) {
		ice_decruncher_helper6(ctx);
	}
}

static uint8 _decruncher_ice_table2[] = { /* length_tab */
	9, 1, 0, 0xFF, 0xFF, 8, 4, 2, 1, 0, 
	
	/* more_offset */
	11, 4, 7, 0,
	
	1, 0x20, 0, 0, 0, 0x20, 0x22, 0, 0xE8, 0x88, 2, 0x81, 0, 0, 0,
	0xF, 0x60, 8, 0x22, 0xD8, 0x22, 0xD8, 0x22, 0xD8, 0x22, 0xD8,
	4, 0x80, 0, 0, 0, 1, 0x6A, 0xF0, 0x60, 2
};

static void ice_decruncher_helper6(decrunch_ctx_t *ctx) { /* strings */
	uint8 *a1;
	int16 d0;
	uint16 d1;
	int d4, cf;
	int i = 3;
	do {
		cf = ice_decruncher_helper3(ctx);
	} while (cf && --i != -1);
	d1 = 0;
	d0 = (int8)_decruncher_ice_table2[i + 1];
	if (d0 >= 0) {
		d1 = decruncher_ice_helper4(ctx, d0);
	}
	d4 = _decruncher_ice_table2[i + 6] + d1;
	if (d4 != 0) {
		uint8 *a1 = &_decruncher_ice_table2[10];
		assert(*a1 == 11);
		i = 1;
		do {
			cf = ice_decruncher_helper3(ctx);
		} while (cf && --i != -1);
		d1 = 0;
		d0 = (int8)a1[i + 1];
		assert(d0 >= 0);
		d1 = decruncher_ice_helper4(ctx, d0);
		d1 += read_uint16BE(a1 + i * 2 + 6);
	} else {
		d1 = 0;
		d0 = 5;
		i = 0;
		cf = ice_decruncher_helper3(ctx);
		if (cf) {
			d0 = 8;
			i = 0x40;
		}
		d1 = decruncher_ice_helper4(ctx, d0) + i;
	}
	a1 = ctx->a6 + d4 + 2 + (int16)d1;
	--a1;
	--ctx->a6;
	*ctx->a6 = *a1;
	do {
		--a1;
		--ctx->a6;
		*ctx->a6 = *a1;
	} while (--d4 != -1);
	ice_decruncher_helper2(ctx);
}


static void ice_decruncher_helper1(decrunch_ctx_t *ctx) { /* getinfo */
	ctx->a5 -= 4;
	ctx->d7 = read_uint32BE(ctx->a5);
}

static void ice_decrunch(decrunch_ctx_t *ctx) {
	ctx->a5 = ctx->src + ctx->src_sz;
	ctx->a4 = ctx->dst;
	ctx->a6 = ctx->dst + ctx->dst_sz;
	ice_decruncher_helper1(ctx);
/*	printf("ice_decrunch() ctx->d7=0x%X ctx->src=0x%X a5=0x%X a4=0x%X a6=0x%X\n", ctx->d7, ctx->src, ctx->a5, ctx->a4, ctx->a6); */
	ice_decruncher_helper2(ctx);
}

int main(int argc, char *argv[]) {
	if (argc >= 2) {
		FILE *fp = fopen(argv[1], "rb");
		if (fp) {
			int sz;
			uint8 *src;
			fseek(fp, 0, SEEK_END);
			sz = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			src = (uint8 *)malloc(sz);
			assert(src && ((uint32)src & 3) == 0);
			fread(src, 1, sz, fp);
			fclose(fp);
			if (read_uint32BE(src) == 0x49636521) { /* 'Ice!' */
				FILE *fp_out;
				char out_file[256];
				decrunch_ctx_t ctx;
				memset(&ctx, 0, sizeof(ctx));
				ctx.src_sz = read_uint32BE(src + 4);
				ctx.dst_sz = read_uint32BE(src + 8);
				ctx.src = src;
				printf("packed_size = %ld unpacked_size = %ld\n", ctx.src_sz, ctx.dst_sz);
				assert(ctx.src_sz < ctx.dst_sz);
				ctx.dst = (uint8 *)malloc(ctx.dst_sz);
				assert(ctx.dst);
				memset(ctx.dst, 0, ctx.dst_sz);
				ice_decrunch(&ctx);
				sprintf(out_file, "%s.melt", argv[1]);
				fp_out = fopen(out_file, "wb");
				if (fp_out) {
					fwrite(ctx.dst, 1, ctx.dst_sz, fp_out);
					fclose(fp_out);
				}
				printf("wrote unpacked file\n");
				free(ctx.src);
				free(ctx.dst);
			}
		}
	}
	return 0;
}
