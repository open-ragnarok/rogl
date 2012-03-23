/*
    ------------------------------------------------------------------------------------
    LICENSE:
    ------------------------------------------------------------------------------------
    This file is part of The Open Ragnarok Project
    Copyright 2007 - 2012 The Open Ragnarok Team
    For the latest information visit http://www.open-ragnarok.org
    ------------------------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any later
    version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
    this program; if not, write to the Free Software Foundation, Inc., 59 Temple
    Place - Suite 330, Boston, MA 02111-1307, USA, or go to
    http://www.gnu.org/copyleft/lesser.txt.
    ------------------------------------------------------------------------------------
*/
#ifndef __BMP_H_339
#define __BMP_H_339

#ifdef __cplusplus
extern "C" {
#endif

struct strBMPFileHeader {
	unsigned char type[2];          /**< This specifies the type of file. This should be equal to BM */
	unsigned int file_size; /**< This gives the bitmap size in bytes */
	unsigned short reserved_1;      /**< This is reserved and must be 0 */
	unsigned short reserved_2;      /**< This is also reserved and must also be 0 */
	unsigned int offset;            /**< This specifies the number of bytes from the starting of the bitmap to where the actual data part starts */
};

struct strBMPInfoHeader {
	unsigned int size;
	union {
		struct {
			int width;                                              /* This specifies the width of bitmap in pixels */
			int height;                                             /* This specifies the height of bitmap in pixels */
		};
		int dimension[2];
	};
	short color_planes;    /* This specifies the number of color planes */
	short bpp;                             /* This specifies the bits per pixel i.e. the total number of colors in the bitmap */
	unsigned int compression;               /* This specifies the type of RLE compression in the bitmap. No compression means that the value will be 0. */
	unsigned long image_size;               /* This specifies the size of image in pixels i.e. width x height */
	int horizontal_resolution;              /* This specifies the horizontal resolution of image as pixels per meter */
	int vertical_resolution;                /* This specifies the vertical resolution of image as pixels per meter */
	unsigned int colors_used;               /* This specifies the colors used in the bitmap. For 4-bit image this value is 0 */
	unsigned int important_colors;  /* This specifies the minimum number of important colors in the bitmap */
} ;

struct strBMP {
	struct strBMPFileHeader header;
	struct strBMPInfoHeader info;

	union {
		struct {
			unsigned char b;
			unsigned char g;
			unsigned char r;
			unsigned char a;
		};
		unsigned char color[4];
		unsigned int intcolor;
	} pallete[256];


	void *data;
};

struct strBMP* loadBitmap(const unsigned char* data, unsigned int length);
void freeBitmap(struct strBMP*);

// Converts an 8-bit BMP to a 32-bit chunk of data. use freeBitmapData to release the pointer memory.
unsigned char *bmp8to32(const struct strBMP* bmp);
    
void freeBitmapData(unsigned char* data);

#ifdef __cplusplus
}
#endif


#endif /* __BMP_H_339 */
