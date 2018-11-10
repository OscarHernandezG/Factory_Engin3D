#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleImGui.h"
//#include "ModuleGeometryManager.h"

#include "Geometries.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "ImGuizmo/ImGuizmo.h"
#include <fstream>  

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");

	bool ret = true;

	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	octree.Create(AABB(float3(-5, 0, -5), float3(5, 5, 5)));

	ImGuizmo::Enable(true);
	return ret;
}

update_status ModuleSceneIntro::PreUpdate()
{
	update_status status = UPDATE_CONTINUE;

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->renderer3D->debugQuad = !App->renderer3D->debugQuad;
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)//Empty game object
	{
		float3 pos = math::float3((rand() % 100 )- 50, (rand() % 100) - 50, (rand() % 100) - 50);

		GameObject* random = App->gameObject->CreateGameObject(pos);

		const float3 center(pos.x, pos.y, pos.z);
		const float3 size(float3::two);

		random->SetABB(AABB::FromCenterAndSize(center, size));

		octree.Insert(random);
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->renderer3D->cameraCulling = !App->renderer3D->cameraCulling;
	}

	//Guizmos Options

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		guizOperation = ImGuizmo::OPERATION::TRANSLATE;

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		guizOperation = ImGuizmo::OPERATION::SCALE;

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		guizOperation = ImGuizmo::OPERATION::ROTATE;

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
		guizOperation = ImGuizmo::OPERATION::BOUNDS;

	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
			GetPreviousTransform();

		isSnap = true;
	}
	else if(isSnap)
		isSnap = false;

	return status;
}

// Update
update_status ModuleSceneIntro::Update()
{

	GuizmoUpdate();
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::GuizmoUpdate()
{
	GameObject* transformObject = App->geometry->currentGameObject;
	if (transformObject != nullptr)
	{

		/*float pos[3];
		float rot[3];
		float scale[3];

		ImGuizmo::DecomposeMatrixToComponents(transform->GetMatrix().ptr(), pos, rot, scale);
		ImGui::InputFloat3("Tr", pos, 3);
		ImGui::InputFloat3("Rt", rot, 3);
		ImGui::InputFloat3("Sc", scale, 3);
		ImGuizmo::RecomposeMatrixFromComponents(pos, rot, scale, (float*)transform->GetMatrix().ptr());

		transform->SetPos(pos[0], pos[1], pos[2]);
		transform->SetRotation({ math::DegToRad(rot[0]),  math::DegToRad(rot[1]),  math::DegToRad(rot[2]) });
		transform->SetScale(scale[0], scale[1], scale[2]);*/
		float4x4 globalMatrix = transformObject->GetGlobalMatrix();
		globalMatrix.Transpose();
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(App->camera->GetViewMatrix().ptr(), App->camera->GetProjectionMatrix().ptr(), guizOperation, guizMode, globalMatrix.ptr(), nullptr, isSnap ? snap.ptr() : nullptr);

		if (ImGuizmo::IsUsing())
			MoveGO(globalMatrix, transformObject);
		
		else
		{
			if (saveTransform)
			{
				SaveLastTransform(lastMat);
				saveTransform = false;
			}
			lastMat = transformObject->GetGlobalMatrix();
		}
	}
}

void ModuleSceneIntro::MoveGO(math::float4x4 &globalMatrix, GameObject * transformObject)
{
	float3 pos, scale;
	Quat rot;
	globalMatrix.Transposed().Decompose(pos, rot, scale);
	switch (guizOperation)
	{
	case ImGuizmo::TRANSLATE:
		transformObject->SetPos(pos);
		break;
	case ImGuizmo::ROTATE:
		transformObject->SetRotation(rot);
		break;
	case ImGuizmo::SCALE:
		transformObject->SetScale(scale);
		break;
	default:
		break;
	}
	octree.ReDoOctree(AABB(), true);
	saveTransform = true;
}

void ModuleSceneIntro::SaveLastTransform(float4x4 matrix)
{
	LastTransform prevTrans;
	if (prevTransforms.empty() || App->geometry->currentGameObject->GetGlobalMatrix().ptr() != prevTransforms.top().matrix.ptr())
	{
		prevTrans.matrix = matrix;
		prevTrans.object = App->geometry->currentGameObject;
		prevTransforms.push(prevTrans);
	}
}

void ModuleSceneIntro::GetPreviousTransform()
{
	if (!prevTransforms.empty())
	{
		LastTransform prevTrans = prevTransforms.top();
		App->geometry->currentGameObject = prevTrans.object;
		App->geometry->currentGameObject->SetTransform(prevTrans.matrix);
		prevTransforms.pop();
		octree.ReDoOctree(AABB(), true);
	}
}

void ModuleSceneIntro::ReInsertOctree(GameObject* object)
{
	for (list<GameObject*>::iterator iterator = object->childs.begin(); iterator != object->childs.end(); ++iterator)
	{
		ReInsertOctree(*iterator);
	}
	octree.Insert(object);
}

void ModuleSceneIntro::SetGuizOperation(ImGuizmo::OPERATION operation)
{
	guizOperation = operation;
}

ImGuizmo::OPERATION ModuleSceneIntro::GetGuizOperation() const
{
	return guizOperation;
}

void ModuleSceneIntro::SetGuizMode(ImGuizmo::MODE mode)
{
	guizMode = mode;
}

ImGuizmo::MODE ModuleSceneIntro::GetGuizMode() const
{
	return guizMode;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	return true;
}