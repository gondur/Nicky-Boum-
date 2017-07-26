
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void calc_quad(int alpha, int base) {
	int a, line_count;
	printf("quad(%d) = \n", alpha);
	line_count = 0;
	for (a = 0; a < base; ++a) {
		double angle_deg = alpha + a * 90. / base;
		unsigned int theta = sin(angle_deg * M_PI / 180) * 256;
		printf("0x%04X, ", theta & 0xFFFF);
		if (line_count == 10) {
			printf("\n");
			line_count = 0;
		} else {
			++line_count;
		}
	}
	printf("\n");
}

static void gen_trigo_table(double (*trigo)(double), const char *name) {
	int a, line_count;
	printf("uint16 %s[512] = {\n", name);
	line_count = 0;
	for (a = 0; a < 128 * 4; ++a) {
		double alpha = a * M_PI / 256;
		unsigned int theta = (*trigo)(alpha) * 256;
		printf("0x%04X, ", theta & 0xFFFF);
		if (line_count == 10) {
			printf("\n");
			line_count = 0;
		} else {
			++line_count;
		}
	}
	printf("};\n");
}

static void gen_mod_vibrato_sine_wave_form(const char *name) {
	int i, line_count = 0;
	printf("uint8 %s[64] = {\n\t", name);
	for (i = 0; i < 64; ++i) {
		unsigned char sw = (unsigned char)(sin(i * 2 * M_PI / 64) * 255);
		printf("0x%02X, ", sw);
		if (line_count == 10) {
			printf("\n\t");
			line_count = 0;
		} else {
			++line_count;
		}
	}
	printf("};\n");
}

int main(int argc, char *argv[]) {
	int i;
	for (i = 0; i < 4; ++i) {
		calc_quad(90 * i, 128);
	}
	gen_trigo_table(cos, "cos_table");
	gen_trigo_table(sin, "sin_table");
	gen_mod_vibrato_sine_wave_form("mpl_vibrato_table");
	return 0;
}
