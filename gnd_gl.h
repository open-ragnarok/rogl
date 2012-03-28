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
#ifndef __GND_GL_H
#define __GND_GL_H

#include "roint/gnd.h"
#include "roint/grf.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RoGndGL_VertexInfo {
	float coord[3];
	float tex[2];
	float normal[3];
	float lightmap_tex[2];	//< Texture coordinates are the same for both shadow and color
};

typedef unsigned short t_rogndidx[4];

struct ROGndGL {
	unsigned int objcount;	//< Number of quads
	struct RoGndGL_VertexInfo *vertexdata;
	t_rogndidx *indexdata;
	unsigned int *texturedata;
};

struct ROGndGLVBO {
	unsigned int vbo[2];	//< VBO Objects
	unsigned int objcount;	//< Number of quads
	unsigned int texturecount;	//< Number of textures used by this object
	unsigned int *texturesids;	//< OpenGL ids for registered textures  (size: texturecount)
	unsigned int *vertexcount;	//< Array with number of vertexes for each texture (size: texturecount)
	unsigned int lightmap_textures[2];
};

// "Translates" the ROGnd structure into an GL-Friendly one
struct ROGndGL *gndGL_load(const struct ROGnd*);
void gndGL_free(struct ROGndGL*);

struct ROGndGLVBO *gndGLVBO_load(const struct ROGndGL*, const struct ROGnd*, const struct ROGrf* grf);
void gndGLVBO_draw(const struct ROGndGLVBO*);
void gndGLVBO_free(struct ROGndGLVBO*);

#ifdef __cplusplus
}
#endif

#endif /* __GND_GL_H */
