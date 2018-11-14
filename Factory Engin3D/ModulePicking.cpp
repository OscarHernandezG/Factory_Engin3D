#include "Globals.h"
#include "Application.h"
#include "ModulePicking.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"
#include "ModuleImGui.h"

ModulePicking::ModulePicking(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePicking::~ModulePicking()
{}

bool ModulePicking::Start()
{
	bool ret = true;

	return ret;
}

bool ModulePicking::CleanUp()
{

	return true;
}

update_status ModulePicking::Update()
{

	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_DOWN && !App->gui->IsAnyWindowHovered()
		&& App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_RALT) != KEY_REPEAT)
	{
		float mouseX = -(1.0f - ((float(App->input->GetMouseX()) * 2.0f) / (float)App->window->width));
		float mouseY = 1.0f - ((float(App->input->GetMouseY()) * 2.0f) / (float)App->window->height);

		LineSegment ray = App->renderer3D->GetCameraFrustrum().UnProjectLineSegment(mouseX, mouseY);

		std::vector<GameObject*> objects;
		App->sceneIntro->octree.GetIntersects(objects, ray);

		float smallerDist = 0.0f;
		GameObject* nearObject = nullptr;

		//Iterate Octree Objects
		for (std::vector<GameObject*>::const_iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
			CheckObjectPicking(*iterator, ray, smallerDist, nearObject);

		//Iterate Dynamic Objects
		std::list<GameObject*>::const_iterator iterator = App->gameObject->dynamicObjects.begin();
		for (iterator; iterator != App->gameObject->dynamicObjects.end(); ++iterator)
			CheckObjectPicking(*iterator, ray, smallerDist, nearObject);

		if (nearObject != nullptr)
			App->geometry->currentGameObject = nearObject;

		rayDraw = RayLine(ray.a, ray.b);
		rayDraw.gameObject = App->gameObject->rootGameObject;
	}
	rayDraw.Render();
	return UPDATE_CONTINUE;
}

void ModulePicking::CheckObjectPicking(GameObject* iterator, const LineSegment &ray, float &smallerDist, GameObject* &nearObject)
{
	if (iterator->HasComponent(ComponentType_GEOMETRY) && iterator->GetActive())
	{
		LineSegment rayTransformed(ray);
		rayTransformed.Transform(iterator->transform->GetMatrix().Inverted());

		Geometry* geometry = (Geometry*)iterator->GetComponent(ComponentType_GEOMETRY);
		if (geometry->GetType() == Primitive_Mesh)
		{
			if (CheckMeshTri(geometry, rayTransformed, smallerDist))
				nearObject = iterator;
		}
	}
}

bool ModulePicking::CheckMeshTri(Geometry * geometry, LineSegment &ray, float &smallerDist)
{
	bool ret = false;
	Triangle triangle;
	Buffer<uint> index = ((Mesh*)geometry)->buffer->index;
	for (int i = 0; i < index.size / 3; ++i)
	{
		triangle.a = SetTrianglePoint(((Mesh*)geometry)->buffer->vertex, index, (i * 3));
		triangle.b = SetTrianglePoint(((Mesh*)geometry)->buffer->vertex, index, (i * 3) + 1);
		triangle.c = SetTrianglePoint(((Mesh*)geometry)->buffer->vertex, index, (i * 3) + 2);

		float distance;
		float3 pos;
		if (ray.Intersects(triangle, &distance, &pos))
		{
			if (distance < smallerDist || smallerDist == 0.0f)
			{
				smallerDist = distance;
				ret = true;
			}
		}
	}
	return ret;
}

float3 ModulePicking::SetTrianglePoint(Buffer<float> vertex, Buffer<uint> index, uint idIndex)
{
	float3 newPoint;
	newPoint.x = vertex.buffer[index.buffer[idIndex] * 3];
	newPoint.y = vertex.buffer[index.buffer[idIndex] * 3 + 1];
	newPoint.z = vertex.buffer[index.buffer[idIndex] * 3 + 2];

	return newPoint;
}