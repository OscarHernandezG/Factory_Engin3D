#include "Application.h"

#include "ModuleGameObject.h"

using namespace std;
ModuleGameObject::ModuleGameObject(Application * app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGameObject::~ModuleGameObject()
{

}

bool ModuleGameObject::Start()
{
	rootGameObject = new GameObject(float3::zero, Quat::identity, float3::one, nullptr, "Scene");

	return true;
}

update_status ModuleGameObject::Update()
{
	rootGameObject->Update(App->time->Getdt());

	//App->geometry->Draww(rootGameObject);

	return UPDATE_CONTINUE;
}

update_status ModuleGameObject::PostUpdate()
{
	list<GameObject*> toDelete;

	for (list<GameObject*>::iterator iterator = gameObjectsAll.begin(); iterator != gameObjectsAll.end(); ++iterator)

	{
		if ((*iterator)->toDelete)
		{
			toDelete.push_back(*iterator);
		}

	}
	for (list<GameObject*>::iterator iterator = toDelete.begin(); iterator != toDelete.end(); ++iterator)
	{
		if (*iterator != nullptr)
		{
			GameObject* it = *iterator;
			RemoveObjectsFromList(it, toDelete);

			gameObjectsAll.remove(it);
			(*iterator)->RealDelete();
			delete *iterator;
		}
	}
	return UPDATE_CONTINUE;
}

void ModuleGameObject::RemoveObjectsFromList(GameObject * it, list<GameObject*> &toDelete)
{
	for (list<GameObject*>::iterator childIt = it->childs.begin(); childIt != it->childs.end(); ++childIt)
	{
		RemoveObjectsFromList(*childIt, toDelete);
		gameObjectsAll.remove(*childIt);
		toDelete.remove(*childIt);
	}
}

bool ModuleGameObject::CleanUp()
{
	CleanAllGameObjects();

	return true;
}

void ModuleGameObject::CleanAllGameObjects()
{
	// Deleting a GameObject will cause all his childs to be deleted
	// If we delete the root GameObject, all the GameObjects will be deletet recursively
	if (rootGameObject != nullptr)
	{
		delete rootGameObject;
		rootGameObject = nullptr;
	}
}

void ModuleGameObject::SaveScene()
{
	if (rootGameObject != nullptr)
	{
		JSON_Value* rootValue = json_value_init_object();
		JSON_Object* rootObject = json_value_get_object(rootValue);

		SaveGameObject(rootGameObject, rootObject);

		int sizeBuf = json_serialization_size_pretty(rootValue);
		char* buf = new char[sizeBuf];
		json_serialize_to_buffer_pretty(rootValue, buf, sizeBuf);
		App->importer->SaveFile("scene", sizeBuf, buf, LlibraryType::LlibraryType_SCENE);
		delete[] buf;
		json_value_free(rootValue);
	}
}

void ModuleGameObject::SaveGameObject(GameObject* object, JSON_Object* parent)
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

	for (list<Component*>::iterator iterator = object->components.begin(); iterator != object->components.end(); ++iterator)
	{
		JSON_Value* component = json_value_init_object();
		JSON_Object* componentObj = json_value_get_object(component);

		json_object_set_value(componentsObj, to_string((*iterator)->GetUUID()).data() , component);

		SaveComponent(*iterator, componentObj);
	}
}

void ModuleGameObject::SaveComponent(Component* object, JSON_Object* parent)
{
	switch (object->type)
	{
	case ComponentType_TRANSFORM: 
		((Transform*)object)->SaveComponent(parent);
		break;
	case ComponentType_GEOMETRY: 
		((Geometry*)object)->SaveComponent(parent);
		break;
	case ComponentType_CAMERA: 
		((Camera*)object)->SaveComponent(parent);
		break;
	case ComponentType_TEXTURE:
		//((Transform*)object)->SaveComponent(parent);
		break;
	case ComponentType_LIGHT:
		//((Transform*)object)->SaveComponent(parent);
		break;
	default:
		break;
	}
}


GameObject* ModuleGameObject::CreateGameObject(float3 position, Quat rotation, float3 scale, GameObject* father, const char* name)
{
	GameObject* newGameObject = nullptr;

	if (father == nullptr)
		father = rootGameObject;

	newGameObject = new GameObject(position, rotation, scale, father, name);

	father->childs.push_back(newGameObject);

	gameObjectsAll.push_back(newGameObject);

	//newGameObject->transform->boundingBox.minPoint = float3(-5, -5, -5);
	//newGameObject->transform->boundingBox.minPoint = float3(5, 5, 5);

	//if (newGameObject)
	//	App->sceneIntro->quadtree.Insert(newGameObject);

	return newGameObject;
}

void ModuleGameObject::AddNewDynamic(GameObject* object)
{
	for (list<GameObject*>::iterator iterator = object->childs.begin(); iterator != object->childs.end(); ++iterator)
	{
		AddNewDynamic(*iterator);
	}

	if (find(dynamicObjects.begin(), dynamicObjects.end(), object) == dynamicObjects.end())
		dynamicObjects.push_back(object);
}

void ModuleGameObject::RemoveDynamic(GameObject* object)
{
	for (list<GameObject*>::iterator iterator = object->childs.begin(); iterator != object->childs.end(); ++iterator)
	{
		RemoveDynamic(*iterator);
	}
	auto it = find(dynamicObjects.begin(), dynamicObjects.end(), object);
	if (it != dynamicObjects.end())
	{
		dynamicObjects.remove(*it);
		App->sceneIntro->octree.Insert(object);
	}
}

bool ModuleGameObject::CanTransform(GameObject* object)
{
	return (object->GetObjectStatic() || App->time->gameState == GameState_NONE);
}

void ModuleGameObject::SaveBeforePlay()
{
	for (list<GameObject*>::iterator iterator = gameObjectsAll.begin(); iterator != gameObjectsAll.end(); ++iterator)
	{
		playingObjects[(*iterator)->UID] = (*iterator)->GetGlobalMatrix();
	}
}


void ModuleGameObject::LoadAfterPlay()
{
	for (list<GameObject*>::iterator listiterator = gameObjectsAll.begin(); listiterator != gameObjectsAll.end(); ++listiterator)
	{
		map<uint, float4x4>::iterator iterator = playingObjects.find((*listiterator)->UID);
		if (iterator != playingObjects.end())//Set transform before playing
		{
			(*listiterator)->SetTransform(iterator->second);//SET
			playingObjects.erase((*listiterator)->UID);
		}
		else//Remove objects that are created while playing
		{
			(*listiterator)->toDelete = true;
			App->geometry->currentGameObject = nullptr;
			App->sceneIntro->octree.ReDoOctree(AABB(), true);
		}
	}
}