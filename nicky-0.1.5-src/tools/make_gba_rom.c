
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int mgr_get_file_size(FILE *fp) {
	int sz;
	int cur_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, cur_pos, SEEK_SET);
	return sz;
}

static void mgr_copy_and_pad_data(FILE *fp_rom, FILE *fp_dat, FILE *fp_out) {
	char buf[2048];
	int rom_size, dat_size, cur_size, align;
	
	rom_size = mgr_get_file_size(fp_rom);
	dat_size = mgr_get_file_size(fp_dat);
	
	/* copy rom */
	cur_size = rom_size;
	while (cur_size > 0 && !feof(fp_rom)) {
		int n = fread(buf, 1, sizeof(buf), fp_rom);
		fwrite(buf, 1, n, fp_out);
		cur_size -= n;
	}
	
	/* pad */
	memset(buf, 0, sizeof(buf));
	align = rom_size & 255;
	if (align != 0) {
		align = 256 - align;
		fwrite(buf, 1, align, fp_out);
	}
	
	/* copy data */
	cur_size = dat_size;
	while (cur_size > 0 && !feof(fp_dat)) {
		int n = fread(buf, 1, sizeof(buf), fp_dat);
		fwrite(buf, 1, n, fp_out);
		cur_size -= n;
	}
	
	printf("data_offset = 0x%08X\n", 0x08000000 | ((rom_size + 255) & ~255));
}

int main(int argc, char *argv[]) {
	if (argc >= 4) {
		FILE *fp_rom = 0;
		FILE *fp_dat = 0;
		fp_rom = fopen(argv[1], "rb");
		fp_dat = fopen(argv[2], "rb");
		if (!fp_rom) {
			printf("Unable to open rom file '%s' for reading\n", argv[1]);
		} else if (!fp_dat) {
			printf("Unable to open data file '%s' for reading\n", argv[2]);
		} else {
			FILE *fp_out;
			char rom_filename[64];
			fp_out = fopen(argv[3], "wb");
			if (fp_out) {
				mgr_copy_and_pad_data(fp_rom, fp_dat, fp_out);
				fclose(fp_out);
			} else {
				printf("Unable to open file '%s' for writing\n", rom_filename);
			}
		}
		if (fp_rom) {
			fclose(fp_rom);
		}
		if (fp_dat) {
			fclose(fp_dat);
		}
	}
	return 0;
}
