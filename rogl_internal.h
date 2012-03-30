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
#ifndef ROGL_INTERNAL_H_
#define ROGL_INTERNAL_H_

#if defined(_WIN32) && !defined(__GNUC__)
#	include <windows.h>
#endif

#ifdef __APPLE__
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#else
#   include <gl/gl.h>
#   include <gl/glu.h>
#endif

#include "roint/grf.h"

#define ROGL_OFFSET(x) ((void*)(x))
#define ROGL_SHORT_OFFSET(x) ((void*)(sizeof(short) * (x)))
#define ROGL_FLOAT_OFFSET(x) ((void*)(sizeof(float) * (x)))

#define ROGL_VBO_VERTEXES 0
#define ROGL_VBO_INDICES 1

#ifdef _WIN32
#	ifdef __GNUC__
#		include "msvc/glext.h"
#	else
#		include "glext.h"
#	endif
// Please note that your implementation must provide these functions. These are only external prototypes.
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
extern PFNGLMAPBUFFERPROC glMapBuffer;
extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;
extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
extern PFNGLTEXIMAGE3DPROC glTexImage3D;
extern PFNGLTEXSUBIMAGE3DEXTPROC glTexSubImage3D;
extern PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
#endif

#include <math.h>

#ifndef M_PI
#	define M_PI 3.1415926535
#endif

#endif /* ROGL_INTERNAL_H_ */
