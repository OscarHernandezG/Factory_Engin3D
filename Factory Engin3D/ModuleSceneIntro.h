#ifndef __ModuleSceneIntro_H__
#define __ModuleSceneIntro_H__

#include "Module.h"
#include "Globals.h"
#include "Mesh.h"
#include "Quadtree.h"

#include "ImGuizmo/ImGuizmo.h"

#include <vector>
class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	void GuizmoUpdate();
	update_status PostUpdate(float dt);

	void SetGuizOperation(ImGuizmo::OPERATION operation);
	ImGuizmo::OPERATION GetGuizOperation() const;
	void SetGuizMode(ImGuizmo::MODE mode);
	ImGuizmo::MODE GetGuizMode() const;

	bool CleanUp();

public:
	PrimitiveFrustum* frust = nullptr;
	PrimitiveCube* cube = nullptr;

	char* droppedFileDir = nullptr;
	Quadtree quadtree;

private:
	ImGuizmo::OPERATION guizOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE guizMode = ImGuizmo::MODE::WORLD;

};
#endif // !__ModuleSceneIntro_H__