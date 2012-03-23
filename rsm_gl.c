#include "rsm_gl.h"

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

