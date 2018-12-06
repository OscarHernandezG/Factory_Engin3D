#ifndef __ParticlePlane_H__
#define __ParticlePlane_H__

#include "Geometry.h"

class ParticlePlane : public Geometry
{

public:

	ParticlePlane();
	ParticlePlane(float3 position);
	void LoadPlaneBuffers(float3 position);
	void InnerRender() const;
public:
	uint myIndices = 0u;
	uint myVertices = 0u;
	uint myTexture = 0u;
};
#endif //!__ParticlePlane__