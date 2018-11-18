#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleImGui.h"
//#include "ModuleGeometry.h"

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

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)//Debug Draw
		App->renderer3D->debugQuad = !App->renderer3D->debugQuad;

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) //Camera Culling
		App->renderer3D->cameraCulling = !App->renderer3D->cameraCulling;

	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)//Empty game object
	{
		float3 pos = math::float3((rand() % 100 )- 50, (rand() % 100) - 50, (rand() % 100) - 50);

		GameObject* random = App->gameObject->CreateGameObject(pos);

		const float3 center(pos.x, pos.y, pos.z);
		const float3 size(float3::two);

		random->SetABB(AABB::FromCenterAndSize(center, size));

		octree.Insert(random);
	}

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) //Camera Culling
		App->geometry->currentGameObject = App->geometry->cameraObject;

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
		{
			GetPreviousTransform();
			redoOc = true;
		}
		isSnap = true;
	}
	else if (isSnap)
	{
		isSnap = false;
	}


	return status;
}

// Update
update_status ModuleSceneIntro::Update()
{

	GuizmoUpdate();
	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate()
{
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::GuizmoUpdate()
{
	// Todo move on local
	GameObject* transformObject = App->geometry->currentGameObject;
	if (transformObject != nullptr)
	{
		float4x4 globalMatrix = transformObject->GetGlobalMatrix();
		globalMatrix.Transpose();
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(App->renderer3D->GetViewMatrix().ptr(), App->renderer3D->GetProjectionMatrix().ptr(), guizOperation, guizMode, globalMatrix.ptr(), nullptr, isSnap ? snap.ptr() : nullptr);

		if (ImGuizmo::IsUsing() && App->gameObject->CanTransform(transformObject))
		{
			if(App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_RALT) != KEY_REPEAT)
				MoveGO(globalMatrix, transformObject);
		}

		else
		{
			if (saveTransform)
			{
				SaveLastTransform(lastMat);
				saveTransform = false;
				redoOc = true;
			}
			lastMat = transformObject->GetLocalMatrix();
		}
	}
}

void ModuleSceneIntro::MoveGO(math::float4x4 &globalMatrix, GameObject* transformObject)
{
	if (transformObject)
	{
		float3 pos, scale;
		Quat rot;

		globalMatrix.Transpose();

		if (transformObject->father)
		{
			float4x4 temp = transformObject->father->GetGlobalMatrix();

			temp.Inverse();

			globalMatrix = temp.Mul(globalMatrix);
		}

		globalMatrix.Decompose(pos, rot, scale);

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
		saveTransform = true;
	}
}

void ModuleSceneIntro::SaveLastTransform(float4x4 matrix)
{
	LastTransform prevTrans;
	if (prevTransforms.empty() || App->geometry->currentGameObject->GetLocalMatrix().ptr() != prevTransforms.top().matrix.ptr())
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
	}
}

void ModuleSceneIntro::ReInsertOctree(GameObject* object)
{
	for (std::list<GameObject*>::iterator iterator = object->childs.begin(); iterator != object->childs.end(); ++iterator)
	{
		Geometry* currentGeometry = (Geometry*)(*iterator)->GetComponent(ComponentType_GEOMETRY);
		if (currentGeometry)
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