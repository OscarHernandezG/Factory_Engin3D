#include "Application.h"

#include "ModuleGameObjectManager.h"


ModuleGameObjectManager::ModuleGameObjectManager(Application * app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGameObjectManager::~ModuleGameObjectManager()
{

}

bool ModuleGameObjectManager::Start()
{
	root = new GameObject(float3::zero, Quat::identity, float3::one, nullptr, "Scene");

	return true;
}

update_status ModuleGameObjectManager::Update()
{
	root->Update(App->time->dtReal);

	return UPDATE_CONTINUE;
}

update_status ModuleGameObjectManager::PostUpdate()
{
	for (list<GameObject*>::iterator iterator = gameObjects.begin(); iterator != gameObjects.end(); ++iterator)
	{
		if ((*iterator)->toDelete)
		{
			delete (*iterator);
		}
	}
	return UPDATE_CONTINUE;
}

bool ModuleGameObjectManager::CleanUp()
{
	CleanAllGameObjects();

	return true;
}

void ModuleGameObjectManager::CleanAllGameObjects()
{
	// Deleting a GameObject will cause all his childs to be deleted
	// If we delete the root GameObject, all the GameObjects will be deletet recursively
	if (root != nullptr)
	{
		delete root;
		root = nullptr;
	}
}

GameObject* ModuleGameObjectManager::CreateGameObject(float3 position, Quat rotation, float3 scale, GameObject* father, const char* name)
{
	GameObject* newGameObject = nullptr;

	if (father == nullptr)
		father = root;

	newGameObject = new GameObject(position, rotation, scale, father, name);

	father->childs.push_back(newGameObject);

	//newGameObject->transform->boundingBox.minPoint = float3(-5, -5, -5);
	//newGameObject->transform->boundingBox.minPoint = float3(5, 5, 5);

	//if (newGameObject)
	//	App->sceneIntro->quadtree.Insert(newGameObject);

	return newGameObject;
}