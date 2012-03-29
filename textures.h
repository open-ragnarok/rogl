/*
 * textures.h
 *
 *  Created on: 29/03/2012
 *      Author: N0C2
 */

#ifndef __ROGL_TEXTURES_H
#define __ROGL_TEXTURES_H

#include "roint/grf.h"


/**
 * Returns 0 on success. -1 when file is not found. -2 when file is not supported
 *
 * Currently only loads 8-bits and 32-bits bitmap files.
 */
int rogl_load_texture(const struct ROGrf* grf, const char* tex_fn, unsigned int glidx);


#endif /* __ROGL_TEXTURES_H */
