#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleImGui.h"

#include "Geometries.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

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
	quadtree.Create(AABB(float3(-50, 0, -50), float3(50, 15, 50)));

	return ret;
}

update_status ModuleSceneIntro::PreUpdate(float dt)
{
	update_status status = UPDATE_CONTINUE;

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		debugQuadtree = !debugQuadtree;
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		float3 pos = math::float3((rand() % 100 )- 50, rand() % 10, (rand() % 100) - 50);

		GameObject* random = App->gameObject->CreateGameObject(pos);

		const math::float3 center(pos.x, pos.y, pos.z);
		const math::float3 size(2.0f, 2.0f, 2.0f);

		random->transform->boundingBox.SetFromCenterAndSize(center, size);

		quadtree.Insert(random);
	}

	return status;
}

void ModuleSceneIntro::DrawQuad(static float3* corners, Color color)
{
	glLineWidth(2.0f);
	glColor3f(color.r,color.g,color.b);
	glBegin(GL_QUADS);

	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[3]);

	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);
	glVertex3fv((GLfloat*)&corners[6]);

	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);
	glVertex3fv((GLfloat*)&corners[7]);

	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[2]);

	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[6]);
	glVertex3fv((GLfloat*)&corners[2]);

	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[1]);

	glEnd();

}



// Load assets
bool ModuleSceneIntro::CleanUp()
{	
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{

	if (debugQuadtree)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		std::vector<const QuadtreeNode*> aabb;
		quadtree.GetBoxLimits(aabb);
		for (vector<const QuadtreeNode*>::const_iterator iterator = aabb.begin(); iterator != aabb.end(); ++iterator)
		{
			static float3 corners[8];
			(*iterator)->limits.GetCornerPoints(corners);

			DrawQuad(corners);
		}

		std::vector<GameObject*> objects;
		quadtree.GetGameObjects(objects);

		for (vector<GameObject*>::const_iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
		{
			static float3 corners[8];
			(*iterator)->transform->boundingBox.GetCornerPoints(corners);

			DrawQuad(corners, Red);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	return UPDATE_CONTINUE;
}