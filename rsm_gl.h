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
#ifndef __ROGL_RSM_GL_H
#define __ROGL_RSM_GL_H

#include "roint/rsm.h"
#include "roint/grf.h"
#include <gl/gl.h>

#ifdef __cplusplus
extern "C" {
#endif 

struct RoRsmGL_VertexInfo {
	float coord[3];
	float tex[2];
	float normal[3];
	union {
		unsigned int color;
		struct {
			unsigned char r, g, b, a;
		};
	};
};

struct RoRsmGLVBO_NodeInfo {
	char name[40];
	char parent[40];
	unsigned int facecount;					//< Number of triangles on this node
	unsigned int *texture_ids;

	float transformMatrix[16];				//< 4x4 transformation matrix
	float position[3];
	float rotation_angle, rotation_axis[3];
	float scale[3];
	// TODO: Poskeys
	// TODO: Rotkeys
};

struct RoRsmGLVBO {
	unsigned int vbo[2];					//< Vertex Buffer Object indices (vbo and ibo)

	unsigned int texture_count;
	GLuint *texids;							//< Texture ID for each face (total count = vertexcount/3)

	char root_name[40];

	unsigned int node_count;				//< Number of nodes
	struct RoRsmGLVBO_NodeInfo *nodes;		//< Information of each node
};

void rsm_draw(const struct RORsm *rsm, const unsigned int *textures, unsigned long timelapse);

struct RoRsmGLVBO* rsmGLVBO_load(const struct RORsm *rsm, const struct ROGrf* grf);
void rsmGLVBO_draw(const struct RoRsmGLVBO*, unsigned long time);
void rsmGLVBO_free(struct RoRsmGLVBO*);

#ifdef __cplusplus
}
#endif 

#endif /* __ROGL_RSM_GL_H */
