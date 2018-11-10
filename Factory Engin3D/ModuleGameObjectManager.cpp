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
	root->Update(App->time->Getdt());

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

void ModuleGameObjectManager::SaveScene()
{
	if (root != nullptr)
	{
		JSON_Value* rootValue = json_value_init_object();
		JSON_Object* rootObject = json_value_get_object(rootValue);

		SaveGameObject(root, rootObject);

		int sizeBuf = json_serialization_size_pretty(rootValue);
		char* buf = new char[sizeBuf];
		json_serialize_to_buffer_pretty(rootValue, buf, sizeBuf);
		App->importer->SaveFile("scene", sizeBuf, buf, LlibraryType::LlibraryType_SCENE);
		delete[] buf;
		json_value_free(rootValue);
	}
}

void ModuleGameObjectManager::SaveGameObject(GameObject* object, JSON_Object* parent)
{
	JSON_Value* newValue = json_value_init_object();
	JSON_Object* objGO = json_value_get_object(newValue);

	string objectIdentifier(std::to_string(object->GetUID()));

	json_object_set_value(parent, objectIdentifier.data(), newValue);

	// Name and UUIDs
	//------------------------------------------------------------------------
	json_object_set_number(objGO, "UUID", object->GetUID());
	json_object_set_string(objGO, "Name", object->name.data());
	if (object->father)
	json_object_set_number(objGO, "Parent UUID", object->father->GetUID());

	//// Position
	////------------------------------------------------------------------------
	//JSON_Value* position = json_value_init_object();
	//JSON_Object* positionObj = json_value_get_object(position);

	//json_object_set_value(objGO, "Position", position);

	//float3 pos = object->GetPos();

	//json_object_set_number(positionObj, "X", pos.x);
	//json_object_set_number(positionObj, "Y", pos.y);
	//json_object_set_number(positionObj, "Z", pos.z);

	//// Scale
	////------------------------------------------------------------------------
	//JSON_Value* scale = json_value_init_object();
	//JSON_Object* scalenObj = json_value_get_object(scale);

	//json_object_set_value(objGO, "Scale", scale);

	//float3 size = object->GetScale();

	//json_object_set_number(scalenObj, "X", size.x);
	//json_object_set_number(scalenObj, "Y", size.y);
	//json_object_set_number(scalenObj, "Z", size.z);

	//// Rotation
	////------------------------------------------------------------------------
	//JSON_Value* rotation = json_value_init_object();
	//JSON_Object* rotationObj = json_value_get_object(rotation);

	//json_object_set_value(objGO, "Rotation", rotation);

	//Quat rot = object->GetRotation();

	//json_object_set_number(rotationObj, "X", rot.x);
	//json_object_set_number(rotationObj, "Y", rot.y);
	//json_object_set_number(rotationObj, "Z", rot.z);
	//json_object_set_number(rotationObj, "W", rot.w);
	//------------------------------------------------------------------------

	json_object_set_number(objGO, "isActive", object->GetActive());

	for (list<GameObject*>::iterator iterator = object->childs.begin(); iterator != object->childs.end(); ++iterator)
	{
		SaveGameObject(*iterator, parent);
	}


	JSON_Value* components = json_value_init_object();
	JSON_Object* componentsObj = json_value_get_object(components);

	json_object_set_value(objGO, "Components", components);
	if (object->transform)
		object->transform->SaveComponent(componentsObj);

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