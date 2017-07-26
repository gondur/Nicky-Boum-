
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define MAX_FILES_COUNT  100
#define FILENAME_LENGTH  16

#define HEADER_SIGNATURE 0x424F554D /* 'BOUM' */
#define HEADER_VERSION   1
#define HEADER_SIZE      16 /* signature + version + timestamp + num_entries */


typedef struct {
	char filename[FILENAME_LENGTH];
	int size;
	int offset;
	FILE *fp;
} file_entry_t;


static int file_entry_count;
static file_entry_t file_entry_table[MAX_FILES_COUNT];


static int mgd_get_file_size(FILE *fp) {
	int sz;
	int cur_pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fseek(fp, cur_pos, SEEK_SET);
	return sz;
}

static void mgd_write_file_uint32BE(FILE *fp, int n) {
	fputc((n >> 24) & 0xFF, fp);
	fputc((n >> 16) & 0xFF, fp);
	fputc((n >>  8) & 0xFF, fp);
	fputc((n >>  0) & 0xFF, fp);
}

static void mgd_string_upper(char *p) {
	for (; *p; ++p) {
		if (*p >= 'a' && *p <= 'z') {
			*p += 'A' - 'a';
		}
	}
}

static int mgd_is_valid_char(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '.');
}

static void mgd_string_trim(char *str) {
	char *p = str + strlen(str) - 1;
	while (p >= str && !mgd_is_valid_char(*p)) {
		*p-- = '\0';
	}
}

static int mgd_compare_file_entry(const void *a, const void *b) {
	const file_entry_t *fe1 = (const file_entry_t *)a;
	const file_entry_t *fe2 = (const file_entry_t *)b;
	return strcmp(fe1->filename, fe2->filename);
}

static void mgd_get_file_list(FILE *fp, const char *directory_name) {
	char file_path[512];
	char line_buffer[512];
	
	/* read the filenames */
	file_entry_count = 0;
	memset(file_entry_table, 0, sizeof(file_entry_table));
	while (fgets(line_buffer, sizeof(line_buffer), fp)) {
		mgd_string_trim(line_buffer);
		if (strlen(line_buffer) != 0) {
			file_entry_t *fe;
			assert(file_entry_count < MAX_FILES_COUNT);
			sprintf(file_path, "%s/%s", directory_name, line_buffer);
			fe = &file_entry_table[file_entry_count];
			fe->fp = fopen(file_path, "rb");
			if (!fe->fp) {
				printf("WARNING: Unable to open '%s', skipping\n", line_buffer);
			} else {
				fe->size = mgd_get_file_size(fe->fp);
				strncpy(fe->filename, line_buffer, FILENAME_LENGTH - 1);
				mgd_string_upper(fe->filename);
				++file_entry_count;
			}
		}
	}
	
	/* sort by filename */
	qsort(file_entry_table, file_entry_count, sizeof(file_entry_t), mgd_compare_file_entry);
}

static void mgd_write_header(FILE *fp) {
	int i, offset;
	
	offset = 0;
	for (i = 0; i < file_entry_count; ++i) {
		file_entry_t *fe = &file_entry_table[i];
		fe->offset = offset;
		printf("Adding file '%s' size %d offset %d\n", fe->filename, fe->size, fe->offset);
		/* align offsets on a dword boundary */
		offset += (fe->size + 3) & ~3;
	}
	/* write header */
	mgd_write_file_uint32BE(fp, HEADER_SIGNATURE);
	mgd_write_file_uint32BE(fp, HEADER_VERSION);
	mgd_write_file_uint32BE(fp, time(0));
	mgd_write_file_uint32BE(fp, file_entry_count);
	/* write file entries */
	for (i = 0; i < file_entry_count; ++i) {
		file_entry_t *fe = &file_entry_table[i];
		fwrite(fe->filename, 1, FILENAME_LENGTH, fp);
		mgd_write_file_uint32BE(fp, fe->size);
		mgd_write_file_uint32BE(fp, fe->offset);
	}
}

static void mgd_write_data_files(FILE *fp) {
	int i, b;
	char buf[2048];
	for (i = 0; i < file_entry_count; ++i) {
		file_entry_t *fe = &file_entry_table[i];
		int entry_size = fe->size;
		while (entry_size > 0 && !feof(fe->fp)) {
			int n = fread(buf, 1, sizeof(buf), fe->fp);
			fwrite(buf, 1, n, fp);
			entry_size -= n;
		}
		/* align the data on a dword boundary */
		b = (fe->size & 3);
		if (b != 0) {
			b = 4 - b;
			while (b--) {
				fputc(0, fp);
			}
		}
	}
}

static void mgd_free_file_list() {
	int i;
	for (i = 0; i < file_entry_count; ++i) {
		file_entry_t *fe = &file_entry_table[i];
		if (fe->fp) {
			fclose(fe->fp);
		}
	}
	memset(file_entry_table, 0, sizeof(file_entry_table));
}

int main(int argc, char *argv[]) {
	if (argc >= 4) {
		FILE *fp = fopen(argv[1], "r");
		const char *directory_name = argv[2];
		if (fp) {
			mgd_get_file_list(fp, directory_name);
			fclose(fp);
			fp = fopen(argv[3], "wb");
			if (fp) {
				mgd_write_header(fp);
				mgd_write_data_files(fp);
				mgd_free_file_list();
				fclose(fp);
			}
		}
	}
	return 0;
}
