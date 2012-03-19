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
	unsigned int texturecount;	//< Number of textures used by this object
	unsigned int *texturesids;	//< OpenGL ids for registered textures  (size: texturecount)
	unsigned int *vertexcount;	//< Array with number of vertexes for each texture (size: texturecount)
};

// "Translates" the ROGnd structure into an GL-Friendly one
struct ROGndGL *gndGL(const struct ROGnd*);
struct ROGndGLVBO *gndGLVBO(const struct ROGndGL*, const struct ROGnd*, const struct ROGrf* grf);
void freeGndGL(struct ROGndGL*);
void gndGLVBO_draw(const struct ROGndGLVBO*);

#ifdef __cplusplus
}
#endif

#endif /* __GND_GL_H */
