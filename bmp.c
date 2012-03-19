#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "roint/memory.h"

#define BMP_PRINT_ERRORS

// Like memcpy, but adds the amount of bytes copied to the source pointer.
//#define ptrcopy(dest, source, size) hexdump(source, size); memcpy(dest, source, size); source += (size); ptr_pos += (size)
#define ptrcopy(dest, source, size) memcpy(dest, source, size); source += (size); ptr_pos += (size)

void* __bmp_alloc(size_t size) {
    return(get_roint_malloc_func()(size));
}

void __bmp_free(void* ptr) {
    get_roint_free_func()(ptr);
}

unsigned char *bmp8to32(const struct strBMP* bmp) {
	unsigned char *data, *ptr;
	unsigned int datasize;
	unsigned int bpr;
	int x, y;
	unsigned char idx;

	if (bmp->info.bpp == 32)
		return(NULL);

	if (bmp->info.bpp != 8)
		return(NULL);

	bpr = bmp->info.width;
	if (bpr % 4 != 0) {
		bpr += 4 - (bpr % 4);
	}
	datasize = bmp->info.width * bmp->info.height * 4;
	data = (unsigned char*)__bmp_alloc(datasize);
	ptr = data;

	for (x = 0; x < bmp->info.width; x++) {
		for (y = 0; y < bmp->info.height; y++) {
			idx = ((unsigned char*)bmp->data)[y*bpr+x];
			memcpy(ptr, &bmp->pallete[idx], 4);
			ptr += 4;
		}
	}

	return(data);
}

void hexdump(const unsigned char *ptr, unsigned int length) {
	unsigned int i, k;
	unsigned char buf[20];
	memset(buf, ' ', 20);
	buf[19] = 0;

	for (i = 0; i < length; i++, k++) {
		if (i % 16 == 0) {
			if (i > 0) {
				printf(" | %s\n", buf);
				memset(buf, ' ', 20);
				buf[19] = 0;
			}
			k  = 0;
			printf("%04x |", i);
		}
		if (i % 8 == 0) {
			printf(" ");
			k++;
		}

		if (ptr[i] >= 32 && ptr[i] <= 126) {
			buf[k] = ptr[i];
		}
		else {
			buf[k] = '.';
		}
		printf(" %02x", ptr[i]);
	}

	for (; i % 16 != 0; i++) {
		printf("   ");
		if (i % 8 == 0)
			printf(" ");
	}
	printf(" | %s\n", buf);
}

struct strBMP* loadBitmap(const unsigned char* data, unsigned int length) {
	const unsigned char *ptr;
	struct strBMP* bmp;
	unsigned long ptr_pos = 0;

	ptr = data;
	bmp = (struct strBMP*)__bmp_alloc(sizeof(struct strBMP));
	memset(bmp, 0, sizeof(struct strBMP));

	// File Header
	//ptrcopy(&bmp->header, ptr, sizeof(struct strBMPFileHeader));
	ptrcopy(bmp->header.type, ptr, 2);
	ptrcopy(&bmp->header.file_size, ptr, sizeof(unsigned int));
	ptrcopy(&bmp->header.reserved_1, ptr, sizeof(unsigned short));
	ptrcopy(&bmp->header.reserved_2, ptr, sizeof(unsigned short));
	ptrcopy(&bmp->header.offset, ptr, sizeof(unsigned int));

	if (bmp->header.type[0] != 'B' || bmp->header.type[1] != 'M') {
#ifdef BMP_PRINT_ERRORS
		fprintf(stderr, "Invalid BMP header: %c%c\n", bmp->header.type[0], bmp->header.type[1]);
#endif
		__bmp_free(bmp);
		return(NULL);
	}

	// Info Header
	ptrcopy(&bmp->info, ptr, sizeof(int));
	switch(bmp->info.size) {
	case 40:
		ptrcopy(&bmp->info.width, ptr, bmp->info.size - sizeof(int));
		break;
	case 12:
		// OS/2 V1
		ptrcopy(bmp->info.dimension, ptr, sizeof(int) * 2);
		ptr += 2;
		ptrcopy(&bmp->info.bpp, ptr, sizeof(short));
		break;
	default:
#ifdef BMP_PRINT_ERRORS
		fprintf(stderr, "Cannot read bmp with %d bytes on header.\n", bmp->info.size);
#endif
		__bmp_free(bmp);
		return(NULL);
	}

	if (0 == bmp->info.width || 0 == bmp->info.height) {
#ifdef BMP_PRINT_ERRORS
		fprintf(stderr, "Invalid bitmap size: %dx%d\n", bmp->info.dimension[0], bmp->info.dimension[1]);
#endif
		__bmp_free(bmp);
		return(NULL);
	}

	switch (bmp->info.bpp) {
	case 8:
		{
			int bpr = bmp->info.width; // Bytes per row. Must be a multiple of 4.
			int datasize, offset;
			int palsize;

			// copy pallete
			palsize = 256;
			if (bmp->info.colors_used > 0)
				palsize = bmp->info.colors_used;

			ptrcopy(bmp->pallete, ptr, 4 * palsize);

			if (bpr % 4 > 0) {
				bpr += 4 - (bpr % 4);
			}

			datasize = bpr * bmp->info.height;
			
			bmp->data = __bmp_alloc(datasize);

			offset = bmp->header.offset - ptr_pos;
			if (offset > 0) {
				ptr += offset;
				ptr_pos += offset;
			}

			ptrcopy(bmp->data, ptr, datasize);
		}
		break;
	}

	return(bmp);
}

void freeBitmap(struct strBMP* bmp) {
	if (bmp != NULL) {
		if (bmp->data != NULL)
			__bmp_free(bmp->data);
		__bmp_free(bmp);
	}
}

void freeBitmapData(unsigned char* data) {
    __bmp_free(data);
}

