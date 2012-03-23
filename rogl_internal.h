/*
 * rogl_internal.h
 *
 *  Created on: 23/03/2012
 *      Author: N0C2
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

#ifdef _WIN32
#	ifdef __GNUC__
#		include "msvc/glext.h"
#	else
#		include "glext.h"
#	endif
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
#endif

#include <math.h>

#ifndef M_PI
#	define M_PI 3.1415926535
#endif

#endif /* ROGL_INTERNAL_H_ */
