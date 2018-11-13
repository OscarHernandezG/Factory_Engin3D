#include "Application.h"

#include "ModuleGameObject.h"


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
		GameObject* it = *iterator;

		RemoveObjectsFromList(it);

		gameObjectsAll.remove(it);
		(*iterator)->RealDelete();
		delete *iterator;
	}

	return UPDATE_CONTINUE;
}

void ModuleGameObject::RemoveObjectsFromList(GameObject * it)
{
	for (list<GameObject*>::iterator childIt = it->childs.begin(); childIt != it->childs.end(); ++childIt)
	{
		RemoveObjectsFromList(*childIt);
		gameObjectsAll.remove(*childIt);
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
		JSON_Value* rootValue = json_value_init_array();
		JSON_Array* rootObject = json_value_get_array(rootValue);

		SaveGameObject(rootGameObject, rootObject);





		int sizeBuf = json_serialization_size_pretty(rootValue);
		char* buf = new char[sizeBuf];
		json_serialize_to_buffer_pretty(rootValue, buf, sizeBuf);
		App->importer->SaveFile("scene", sizeBuf, buf, LlibraryType::LlibraryType_SCENE);
		delete[] buf;
		json_value_free(rootValue);
	}
}

void ModuleGameObject::SaveGameObject(GameObject* object, JSON_Array*& parent)
{
	JSON_Value* newValue = json_value_init_object();
	JSON_Object* objGO = json_value_get_object(newValue);

	//json_object_set_value(parent, objectIdentifier.data(), newValue);

	// Name and UUIDs
	//------------------------------------------------------------------------
	json_object_set_number(objGO, "UUID", object->GetUID());
	json_object_set_string(objGO, "Name", object->name.data());
	if (object->father)
	json_object_set_number(objGO, "Parent UUID", object->father->GetUID());


	json_object_set_number(objGO, "isActive", object->GetActive());


	JSON_Value* components = json_value_init_array();
	JSON_Array* componentsObj = json_value_get_array(components);

	json_object_set_value(objGO, "Components", components);


	for (list<Component*>::iterator iterator = object->components.begin(); iterator != object->components.end(); ++iterator)
	{
		JSON_Value* component = json_value_init_object();
		JSON_Object* componentObj = json_value_get_object(component);

		SaveComponent(*iterator, componentObj);

		json_array_append_value(componentsObj, component);
	}

	json_array_append_value(parent, newValue);
	
	
	for (list<GameObject*>::iterator iterator = object->childs.begin(); iterator != object->childs.end(); ++iterator)
	{
		SaveGameObject(*iterator, parent);
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