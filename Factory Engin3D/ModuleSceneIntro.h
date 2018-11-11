#ifndef __ModuleSceneIntro_H__
#define __ModuleSceneIntro_H__

#include "Module.h"
#include "Globals.h"
#include "Mesh.h"
#include "Quadtree.h"

#include "ImGuizmo/ImGuizmo.h"

#include <vector>
#include <stack>

struct LastTransform
{
	float4x4 matrix;
	GameObject* object;
};
class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	void GuizmoUpdate();
	void MoveGO(math::float4x4 &globalMatrix, GameObject * transformObject);
	void SaveLastTransform(float4x4 matrix);
	void GetPreviousTransform();
	void ReInsertOctree(GameObject * object);


	void SetGuizOperation(ImGuizmo::OPERATION operation);
	ImGuizmo::OPERATION GetGuizOperation() const;
	void SetGuizMode(ImGuizmo::MODE mode);
	ImGuizmo::MODE GetGuizMode() const;

	bool CleanUp();

public:
	PrimitiveFrustum* frust = nullptr;
	PrimitiveCube* cube = nullptr;

	char* droppedFileDir = nullptr;
	Octree octree;
	
	float4x4 lastMat;
	stack<LastTransform> prevTransforms;
	bool saveTransform = false;
	bool isSnap = false;
	float3 snap = float3::two;
private:
	ImGuizmo::OPERATION guizOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE guizMode = ImGuizmo::MODE::WORLD;

};
#endif // !__ModuleSceneIntro_H__