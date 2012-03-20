#ifndef __ROGL_RSM_GL_H
#define __ROGL_RSM_GL_H

#include "roint/rsm.h"

#ifdef __cplusplus
extern "C" {
#endif 

void rsm_draw(const struct RORsm *rsm, const unsigned int *textures, unsigned long timelapse);

#ifdef __cplusplus
}
#endif 

#endif /* __ROGL_RSM_GL_H */