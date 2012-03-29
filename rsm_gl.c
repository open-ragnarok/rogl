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
#include "rsm_gl.h"

#include "textures.h"
#include "rogl_internal.h"

#include <stdlib.h>
#include <string.h>

#define USE_SLERP

void conv33to44f(float dest[16], const float source[9]) {
	dest[3]  = 0.0f;
	dest[7]  = 0.0f;
	dest[11] = 0.0f;
	dest[12] = 0.0f;
	dest[13] = 0.0f;
	dest[14] = 0.0f;
	dest[15] = 1.0f;

	memcpy(&dest[0], &source[0], sizeof(float) * 3);
	memcpy(&dest[4], &source[3], sizeof(float) * 3);
	memcpy(&dest[8], &source[6], sizeof(float) * 3);
}

void glMultMatrix33f(const float *matrix) {
	float x[16];
	conv33to44f(x, matrix);
	glMultMatrixf(x);
}

void rsm_apply_quaternion(const float q[4]) {
	float dest[16];
#if 0
	// Reference: http://en.wikipedia.org/wiki/Quaternion#Matrix_representations
	dest[0] = dest[5] = dest[10] = dest[15] = q[0];
	dest[1] = dest[14] = q[1];
	dest[4] = dest[11] = -q[1];
	dest[2] = dest[7] = q[2];
	dest[8] = dest[13] = -q[2];
	dest[3] = dest[9] = q[3];
	dest[6] = dest[12] = -q[3];
#elif 0
	// Reference: http://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation#Conversion_to_and_from_the_matrix_representation
	float a, b, c, d;
	float a2,b2,c2,d2;
	a =  q[3];
	b =  q[0];
	c =  q[1];
	d =  q[2];
	a2 = a*a;
	b2 = b*b;
	c2 = c*c;
	d2 = d*d;

	dest[0] = a2 + b2 - c2 - d2;
	dest[1] = 2 * b * c - 2 * a * d;
	dest[2] = 2 * b * d + 2 * a * c;
	dest[3] = 0;
	
	dest[4] = 2 * b * c + 2 * a * d;
	dest[5] = a2 - b2 + c2 - d2;
	dest[6] = 2 * c * d - 2 * a * b;
	dest[7] = 0;
	
	dest[8] = 2 * b * d - 2 * a * c;
	dest[9] = 2 * c * d + 2 * a * b;
	dest[10] = a2 - b2 - c2 + d2;
	dest[11] = 0;


	/*
	dest[0] = q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3];
	dest[1] = 2 * q[1] * q[2] - 2 * q[0] * q[3];
	dest[2] = 2 * q[1] * q[3] + 2 * q[1] * q[2];
	dest[3] = 0;
	
	dest[4] = 2 * q[1] * q[2] + 2 * q[0] * q[3];
	dest[5] = q[0] * q[0] - q[1] * q[1] + q[2] * q[2] - q[3] * q[3];
	dest[6] = 2 * q[2] * q[3] - 2 * q[0] * q[1];
	dest[7] = 0;
	
	dest[8] = 2 * q[1] * q[3] - 2 * q[1] * q[2];
	dest[9] = 2 * q[2] * q[3] + 2 * q[0] * q[1];
	dest[10] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];
	dest[11] = 0;
	*/
	
	dest[12] = 0;
	dest[13] = 0;
	dest[14] = 0;
	dest[15] = 1;
#else
	// Reference: RagCam (mostly)
	// Also, this paper: http://www.itu.dk/people/erikdam/DOWNLOAD/98-5.pdf
	float w, x, y, z;
	x =  q[0];
	y =  q[1];
	z =  q[2];
	w =  q[3];

	// First row
	dest[ 0] = 1.0f - 2.0f * ( y * y + z * z ); 
	dest[ 1] = 2.0f * (x * y + z * w);
	dest[ 2] = 2.0f * (x * z - y * w);
	dest[ 3] = 0.0f;  

	// Second row
	dest[ 4] = 2.0f * ( x * y - z * w );  
	dest[ 5] = 1.0f - 2.0f * ( x * x + z * z ); 
	dest[ 6] = 2.0f * (z * y + x * w );  
	dest[ 7] = 0.0f;  

	// Third row
	dest[ 8] = 2.0f * ( x * z + y * w );
	dest[ 9] = 2.0f * ( y * z - x * w );
	dest[10] = 1.0f - 2.0f * ( x * x + y * y );  
	dest[11] = 0.0f;  

	// Fourth row
	dest[12] = 0;  
	dest[13] = 0;  
	dest[14] = 0;  
	dest[15] = 1.0f;
#endif

	glMultMatrixf(dest);
}

typedef union {
	float q[4];
	struct { float x, y, z, w; };
} quat;

float absf(float x) {
	if (x > 0)
		return(x);
	return(-x);
}

/*
 * Reference: http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/index.htm
 * @param qm Quaternion to return
 */
void slerpf(float _qm[4], const float _qa[4], const float _qb[4], double t) {
	quat *qm;
	const quat *qa, *qb;
	float cosHalfTheta;
	float halfTheta;
	float sinHalfTheta;
	float ratioA, ratioB;
	
	qm = (quat*)_qm;
	qa = (const quat*)_qa;
	qb = (const quat*)_qb;
	
	// Calculate angle between them.
	cosHalfTheta = qa->w * qb->w + qa->x * qb->x + qa->y * qb->y + qa->z * qb->z;
	// if qa=qb or qa=-qb then theta = 0 and we can return qa
	if (absf(cosHalfTheta) >= 1.0){
		qm->w = qa->w;
		qm->x = qa->x;
		qm->y = qa->y;
		qm->z = qa->z;
		return;
	}
	// Calculate temporary values.
	halfTheta = (float)acos(cosHalfTheta);
	sinHalfTheta = (float)sqrt(1.0 - cosHalfTheta * cosHalfTheta);
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to qa or qb
	if (fabs(sinHalfTheta) < 0.001){ // fabs is floating point absolute
		qm->w = (qa->w * 0.5f + qb->w * 0.5f);
		qm->x = (qa->x * 0.5f + qb->x * 0.5f);
		qm->y = (qa->y * 0.5f + qb->y * 0.5f);
		qm->z = (qa->z * 0.5f + qb->z * 0.5f);
		return;
	}
	ratioA = (float)sin((1 - t) * halfTheta) / sinHalfTheta;
	ratioB = (float)sin(t * halfTheta) / sinHalfTheta; 
	//calculate Quaternion.
	qm->w = (qa->w * ratioA + qb->w * ratioB);
	qm->x = (qa->x * ratioA + qb->x * ratioB);
	qm->y = (qa->y * ratioA + qb->y * ratioB);
	qm->z = (qa->z * ratioA + qb->z * ratioB);
}

void rsm_calc_quaternion(const struct RORsmNode *node, unsigned long curtime, float result[4]) {
	int maxframe;
	int curframe;
	int curkey;
	int nextkey;
	float q[4]; //Our new calculated quaternion
#ifndef USE_SLERP
	int qi;
#endif

	float amount;

	if (node->rotkey_count <= 0)
		return;

	maxframe = node->rotkeys[node->rotkey_count - 1].frame;
	curframe = curtime % maxframe;
	nextkey = 1;


	// Find out wich one is the next key
	while (node->rotkeys[nextkey].frame < curframe)
		nextkey++;

	curkey = nextkey - 1;

	// how far along are we from one frame to the other? -- this will always be a number from 0 to 1.
	amount = (float)(curframe - node->rotkeys[curkey].frame)/(float)(node->rotkeys[nextkey].frame - node->rotkeys[curkey].frame);

#ifdef USE_SLERP
	// Use SLERP
	slerpf(q, node->rotkeys[curkey].q, node->rotkeys[nextkey].q, amount);
#else
	// Values proportional to our "amount" variable -- linear!
	for (qi = 0; qi < 4; qi++) {
		//q[qi] = node->rotkeys[curkey].q[qi] + (node->rotkeys[nextkey].q[qi] - node->rotkeys[curkey].q[qi]) * amount;
		q[qi] = node->rotkeys[curkey].q[qi] * (1-amount) + node->rotkeys[nextkey].q[qi] * amount;
	}
#endif

	result[0] = q[0];
	result[1] = q[1];
	result[2] = q[2];
	result[3] = q[3];
}

void rsm_draw_children(const struct RORsm *rsm, const unsigned int *textures, unsigned long timelapse, const char *parent) {
	// time in milliseconds
	int curface;
	int curnode = 0;
	int j;
	const struct RORsmNode *node;
	const struct RORsmFace *face;
	float q[4];
	int rsm_texid; // The texture in the RORsm::textures array.

	for (curnode = 0; curnode < rsm->node_count; curnode++) {
		node = &rsm->nodes[curnode];

		if (parent == NULL) {
			// We should have the name of the main_node
			if (strcmp(rsm->main_node, node->name) != 0)
				continue;
		}
		else {
			// We're not the root node. We need to be drawn according to the parent name
			if (strcmp(parent, node->parent) != 0)
				continue;
		}

		glPushMatrix();

		// pos_?
		glTranslatef(node->pos.x, node->pos.y, node->pos.z);
		glRotatef(node->rot_angle * 180.0f / (float)M_PI, node->rot_axis.x, -node->rot_axis.y, node->rot_axis.z);
		if (node->rotkey_count > 0) {
			rsm_calc_quaternion(node, timelapse, q);
			rsm_apply_quaternion(q);
		}
		glScalef(node->scale.x, node->scale.y, node->scale.z);

		glPushMatrix();
		glTranslatef(node->pos_.x, node->pos_.y, node->pos_.z);
		glMultMatrix33f(node->offsetMT);

		// The actual drawing
		for (curface = 0; curface < node->face_count; curface++) {
			face = &node->faces[curface];
			if (textures != NULL) {
				rsm_texid = node->textures[face->texid];
				glBindTexture(GL_TEXTURE_2D, textures[rsm_texid]);
			}
			glBegin(GL_TRIANGLES);
			for (j = 0; j < 3; j++) {
				glTexCoord2f(node->texv[face->tvertidx[j]].u, node->texv[face->tvertidx[j]].v);
				glVertex3fv(node->vertices[face->vertidx[j]].v);
			}
			glEnd();
		}

		// Draw children
		glPopMatrix();
		rsm_draw_children(rsm, textures, timelapse, node->name);
		glPopMatrix();
	}
}

void rsm_draw(const struct RORsm *rsm, const unsigned int *textures, unsigned long timelapse) {
	rsm_draw_children(rsm, textures, timelapse, NULL);
}

// VBO Stuff

// Function declarations
void rsmGLVBO_draw_node_index(const struct RoRsmGLVBO* rsm, unsigned long time, unsigned int index);
struct RoRsmGLVBO* rsmGLVBO_load(const struct RORsm *rsm, const struct ROGrf* grf);
void rsmGLVBO_draw_child(const struct RoRsmGLVBO* rsm, unsigned long time, const char *parent_name);
void rsmGLVBO_draw_node(const struct RoRsmGLVBO* rsm, unsigned long time, const char *node_name);
void rsmGLVBO_draw(const struct RoRsmGLVBO* rsm, unsigned long time);

struct RoRsmGLVBO* rsmGLVBO_load(const struct RORsm *rsm, const struct ROGrf* grf) {
	struct RoRsmGLVBO* ret = NULL;
	int i, j, k, idx, face_idx;
	unsigned int facecount;
	unsigned int vertexcount;

	struct RoRsmGL_VertexInfo *vertexdata;	//< List of ALL vertices.
	unsigned short *indices;				//< List of ALL indices. in the same sequence of the nodes.

	if (NULL == rsm)
		return(ret);

	facecount = 0;
	vertexcount = 0;

	ret = (struct RoRsmGLVBO*)malloc(sizeof(struct RoRsmGLVBO));
	ret->nodes = (struct RoRsmGLVBO_NodeInfo*)malloc(sizeof(struct RoRsmGLVBO_NodeInfo) * rsm->node_count);
	memset(ret->nodes, 0, sizeof(struct RoRsmGLVBO_NodeInfo) * rsm->node_count);

	strcpy(ret->root_name, rsm->main_node);

	// Find out how many vertices we really have
	ret->node_count = rsm->node_count;
	for (i = 0; i < rsm->node_count; i++) {
		facecount += rsm->nodes[i].face_count; // Each face is a triangle

		// We need to know what's the texture for each face and store them somewhere
		ret->nodes[i].texture_ids = (unsigned int*)malloc(sizeof(unsigned int) * rsm->nodes[i].face_count);
	}
	vertexcount = facecount * 3;	// Every face has three vertices on RSM

	// Textures
	ret->texture_count = rsm->texture_count;
	ret->texids = (GLuint*)malloc(sizeof(GLuint) * ret->texture_count);
	glGenTextures(ret->texture_count, ret->texids);
	for (i = 0; i < rsm->texture_count; i++) {
		j = rogl_load_texture(grf, rsm->textures[i], ret->texids[i]);
#ifdef DEBUG
		if (j != 0) {
			if (j == -1) {
				fprintf(stderr, "ERROR: Cannot find texture: %s\n", rsm->textures[i]);
			}
			else if (j == -2) {
				fprintf(stderr, "ERROR: Texture not supported: %s\n", rsm->textures[i]);
			}
		}
#endif
	}

	// Allocate some space for our vertices and indices
	vertexdata = (struct RoRsmGL_VertexInfo*)malloc(sizeof(struct RoRsmGL_VertexInfo) * vertexcount);
	memset(vertexdata, 0, sizeof(struct RoRsmGL_VertexInfo) * vertexcount);
	indices = (unsigned short*)malloc(sizeof(unsigned short) * vertexcount);
	memset(indices, 0, sizeof(unsigned short) * vertexcount);

	// Copy vertex data to the right place
	idx = 0;
	face_idx = 0;
	for (i = 0; i < rsm->node_count; i++) {						// For each node...
		for (j = 0; j < rsm->nodes[i].face_count; j++) {		// ...and each face...
			for (k = 0; k < 3; k++) {							// ...we have 3 vertices
				// Coordinates
				memcpy(vertexdata[idx].coord, rsm->nodes[i].vertices[rsm->nodes[i].faces[j].vertidx[k]].v, sizeof(float) * 3);
				// Texture
				memcpy(vertexdata[idx].tex, &rsm->nodes[i].texv[rsm->nodes[i].faces[j].tvertidx[k]].u, sizeof(float) * 2);

				// Indice
				indices[idx] = idx;
			    idx++;
			}

			ret->nodes[i].texture_ids[j] = rsm->nodes[i].textures[rsm->nodes[i].faces[j].texid]; // Texture information
			ret->nodes[i].facecount++;	// Number of faces per node
			face_idx++;
		}

		// Setup node details
		strcpy(ret->nodes[i].name, rsm->nodes[i].name);
		strcpy(ret->nodes[i].parent, rsm->nodes[i].parent);
		memcpy(ret->nodes[i].position, rsm->nodes[i].pos.v, sizeof(float) * 3);
		ret->nodes[i].rotation_angle = rsm->nodes[i].rot_angle * 180.0f / (float)M_PI;			// We want degrees.
		memcpy(ret->nodes[i].rotation_axis, rsm->nodes[i].rot_axis.v, sizeof(float) * 3);
		memcpy(ret->nodes[i].scale, rsm->nodes[i].scale.v, sizeof(float) * 3);

		// Transformation matrix for the node
		memset(ret->nodes[i].transformMatrix, 0, sizeof(float) * 16);
		memcpy(&ret->nodes[i].transformMatrix[0], &rsm->nodes[i].offsetMT[0], sizeof(float) * 3);
		memcpy(&ret->nodes[i].transformMatrix[4], &rsm->nodes[i].offsetMT[3], sizeof(float) * 3);
		memcpy(&ret->nodes[i].transformMatrix[8], &rsm->nodes[i].offsetMT[6], sizeof(float) * 3);
		memcpy(&ret->nodes[i].transformMatrix[12], &rsm->nodes[i].pos_, sizeof(float) * 3);
		ret->nodes[i].transformMatrix[15] = 1.0f;
		// TODO: Position keys
		// TODO: Rotation keys
	}

	// Create VBOs and copy stuff to them
	glGenBuffers(2, ret->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret->vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * vertexcount, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, ret->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct RoRsmGL_VertexInfo) * vertexcount, vertexdata, GL_STATIC_DRAW);

    // Free unused memory
    free(vertexdata);
    free(indices);


	return(ret);
}

void rsmGLVBO_draw_child(const struct RoRsmGLVBO* rsm, unsigned long time, const char *parent_name) {
	unsigned int i;
	for (i = 0; i < rsm->node_count; i++) {
		if (strcmp(rsm->nodes[i].parent, parent_name) == 0) {
			rsmGLVBO_draw_node_index(rsm, time, i);
		}
	}
}

void rsmGLVBO_draw_node(const struct RoRsmGLVBO* rsm, unsigned long time, const char *node_name) {
	unsigned int i;
	for (i = 0; i < rsm->node_count; i++) {
		if (strcmp(rsm->nodes[i].name, node_name) == 0) {
			rsmGLVBO_draw_node_index(rsm, time, i);
			break;
		}
	}
}

void rsmGLVBO_draw_node_index(const struct RoRsmGLVBO* rsm, unsigned long time, unsigned int index) {
	unsigned int start;	//< Vertex index offset that we will start drawing (NOT face index!)
	unsigned int end;	//< Vertex end offset
	unsigned int i;

	struct RoRsmGLVBO_NodeInfo *node;	//< The node we're currently drawing.

	start = 0;
	node = NULL;

	if (index >= rsm->node_count) {
#ifdef DEBUG
		fprintf(stderr,"Index out of range\n");
#endif
		return;
	}

	for (i = 0; i < index; i++) {
		start += rsm->nodes[i].facecount * 3;
	}

	node = &rsm->nodes[index];

	end = start + node->facecount * 3;

	glPushMatrix();
	glTranslatef(node->position[0], node->position[1], node->position[2]);	// Position our node
	glRotatef(node->rotation_angle, node->rotation_axis[0], -node->rotation_axis[1], node->rotation_axis[2]);
	// TODO: Animate
	glScalef(node->scale[0], node->scale[1], node->scale[2]);

	glPushMatrix();	// Other changes will not be pushed to our children
	glMultMatrixf(node->transformMatrix);

	// The actual drawing
	// TODO: Optimize this
	for (i = 0; i < node->facecount; i++) {
		glBindTexture(GL_TEXTURE_2D, rsm->texids[node->texture_ids[i]]);
		glDrawRangeElements(GL_TRIANGLES, start, end, 3, GL_UNSIGNED_SHORT, (void*)(sizeof(unsigned short) * (start + i * 3)));
	}
	//glDrawRangeElements(GL_TRIANGLES, start, end, node->facecount * 3, GL_UNSIGNED_SHORT, (void*)(sizeof(unsigned short) * start));

	glPopMatrix();

	// Now, the kids...
	rsmGLVBO_draw_child(rsm, time, node->name);

	glPopMatrix();
}

void rsmGLVBO_draw(const struct RoRsmGLVBO* rsm, unsigned long time) {
	rsmGLVBO_draw_node(rsm, time, rsm->root_name);
}

void rsmGLVBO_free(struct RoRsmGLVBO* rsm) {
	// TODO: Free RoRsmGLVBO data
}


