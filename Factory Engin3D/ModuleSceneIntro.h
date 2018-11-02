#ifndef __ModuleSceneIntro_H__
#define __ModuleSceneIntro_H__

#include "Module.h"
#include "Globals.h"
#include "Mesh.h"
#include "Quadtree.h"

#include <vector>
class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void DrawQuad(float3 * corners, Color color = White);

	bool CleanUp();

public:
	PrimitiveFrustum* frust = nullptr;
	PrimitiveCube* cube = nullptr;

	char* droppedFileDir = nullptr;
	Quadtree quadtree;
	bool debugQuadtree = false;
};
#endif // !__ModuleSceneIntro_H__