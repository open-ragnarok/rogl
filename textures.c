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

#include "textures.h"
#include "bmp.h"

#include "rogl_internal.h"

#include <string.h>

int rogl_load_texture(const struct ROGrf* grf, const char* tex_fn, unsigned int glidx) {
	unsigned char m_alloc;
	struct strBMP *bmp;
	unsigned char *buf;
	struct ROGrfFile *file;
	char fn[64];
	unsigned int i, fn_size;

	sprintf(fn, "data\\texture\\%s", tex_fn);
	// Convert to Lowercase
	fn_size = strlen(fn);
	for (i = 0; i < fn_size; i++)
		fn[i] = (fn[i] >= 65 && fn[i] <= 90)?(fn[i]+32):fn[i];

	file = grf_getfileinfobyname(grf, fn);

	if (file == NULL)
		return(-1);

	if (file->data == NULL) {
		m_alloc = 1;
		grf_getdata(file);
	}
	else {
		m_alloc = 0;
	}

	bmp = loadBitmap(file->data, file->uncompressedLength);
	switch(bmp->info.bpp) {
	case 8:
		buf = bmp8to32(bmp);
		break;
	case 32:
		buf = (unsigned char*)bmp->data;
		break;
	default:
		// invalid BPP
		freeBitmap(bmp);
		if (m_alloc == 1)
			grf_freedata(file);
		return(-2);
	}

	glBindTexture(GL_TEXTURE_2D, glidx);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // Linear Min Filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   // Linear Mag Filter
	glTexImage2D(GL_TEXTURE_2D, 0, 4, bmp->info.width, bmp->info.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buf);

	if (bmp->info.bpp != 32) {
		freeBitmapData(buf);
	}
	freeBitmap(bmp);
	if (m_alloc == 1)
		grf_freedata(file);

	return(0);
}



