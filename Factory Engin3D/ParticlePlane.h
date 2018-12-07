#ifndef __ParticlePlane_H__
#define __ParticlePlane_H__

#include "Globals.h"
#include "MathGeoLib/Math/float3.h"

class Texture;

class ParticlePlane
{
public:
	ParticlePlane();
	void LoadPlaneBuffers();
	void Render(float4x4 matrix, Texture* texture, float4 color) const;

private:
	void DrawPlane(Texture* texture, float4 &color) const;

public:
	uint myIndices = 0u;
	uint myVertices = 0u;
	uint myTexture = 0u;
};
#endif //!__ParticlePlane__