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
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	if (rootGameObject)
		rootGameObject->Update();

	//App->geometry->Draww(rootGameObject);

	return UPDATE_CONTINUE;
}

update_status ModuleGameObject::PostUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

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
		RemoveObjectsFromList(it, toDelete);

		gameObjectsAll.remove(it);
		(*iterator)->RealDelete();
		delete *iterator;
	}

	if(rootGameObject)
		rootGameObject->PostUpdate();

	if (redoOc)
	{
		std::vector<GameObject*> objects;
		//Get only valid object for octre
		for (std::list<GameObject*>::iterator iterator = App->gameObject->gameObjectsAll.begin(); iterator != App->gameObject->gameObjectsAll.end(); ++iterator)
		{
			if ((*iterator)->HasComponent(ComponentType_GEOMETRY) && (*iterator)->transform->boundingBox.Size().Length() > 0)
				objects.push_back(*iterator);
		}

		App->sceneIntro->octree.ReDoOctree(objects);
		redoOc = false;
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
	if (it == App->geometry->plane)
		App->geometry->plane = nullptr;
	if (it == App->geometry->cameraObject)
		App->geometry->cameraObject = nullptr;
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

void ModuleGameObject::LoadScene()
{
//Clear previous
	DeletePreviousScene();

//Prepare new
	App->sceneIntro->octree.Create(AABB(float3::zero, float3::zero));

	//Load new
	JSON_Value* scene = json_parse_file("Llibrary/Scenes/scene.json");

	if (json_value_get_type(scene) == JSONArray)
	{
		JSON_Array* objArray = json_value_get_array(scene);

		int numObjects = json_array_get_count(objArray);
		list<GameObject*> sceneGameObjects;

		vector<uint> meshesToLoad;

		LoadNewSceneGO(numObjects, objArray, meshesToLoad, sceneGameObjects);

		LoadNewSceneMeshes(meshesToLoad);

		for (list<GameObject*>::iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it)
		{
			Mesh* itMesh = (Mesh*)(*it)->GetComponent(ComponentType_GEOMETRY);
			if (itMesh)
			{
				SetGOMeshNewScene(itMesh, it);
			}
			if (!(*it)->SetParent(FindByID((*it)->parentUUID)))
			{
				rootGameObject = *it;
			}
		}
	}
}
void ModuleGameObject::SetGOMeshNewScene(Mesh * itMesh, std::list<GameObject *>::iterator &it)
{
	bool found = false;
	for (vector<ResourceMesh*>::iterator currentMeshBuffer = App->geometry->currentMeshes.begin(); currentMeshBuffer != App->geometry->currentMeshes.end(); ++currentMeshBuffer)
	{
		if (itMesh->GetUUID() == (*currentMeshBuffer)->uuid)
		{
			itMesh->buffer = *currentMeshBuffer;
			(*it)->SetABB(itMesh->buffer->boundingBox);
			found = true;
			break;
		}
	}
	if (found)
		App->sceneIntro->octree.Insert(*it);
}

void ModuleGameObject::LoadNewSceneMeshes(std::vector<uint> &meshesToLoad)
{
	sort(meshesToLoad.begin(), meshesToLoad.end());
	meshesToLoad.erase(unique(meshesToLoad.begin(), meshesToLoad.end()), meshesToLoad.end());

	vector<ResourceMesh*> tempVec;
	App->geometry->LoadMeshImporterUUID(meshesToLoad, App->geometry->currentMeshes);
}

void ModuleGameObject::LoadNewSceneGO(int numObjects, JSON_Array * objArray, std::vector<uint> &meshesToLoad, std::list<GameObject *> &sceneGameObjects)
{
	for (int i = 0; i < numObjects; ++i)
	{
		JSON_Object* currGO = json_array_get_object(objArray, i);

		GameObject* temp = CreateEmptyGameObject(nullptr, json_object_get_string(currGO, "name"));
		temp->CreateFromJson(currGO, meshesToLoad);

		sceneGameObjects.push_back(temp);
	}
}

void ModuleGameObject::DeletePreviousScene()
{
	rootGameObject->RealDelete();
	delete rootGameObject;

	gameObjectsAll.clear();
	App->geometry->currentGameObject = nullptr;
	App->geometry->plane = nullptr;
	App->sceneIntro->octree.Clear();
}

GameObject* ModuleGameObject::FindByID(uint UUID)
{

	for (list<GameObject*>::iterator it = gameObjectsAll.begin(); it != gameObjectsAll.end(); ++it)
	{
		if ((*it)->UID == UUID)
			return *it;
	}

	return nullptr;
}

void ModuleGameObject::SaveGameObject(GameObject* object, JSON_Array*& parent)
{
	JSON_Value* newValue = json_value_init_object();
	JSON_Object* objGO = json_value_get_object(newValue);

	//json_object_set_value(parent, objectIdentifier.data(), newValue);

	// Name and UUIDs
	//------------------------------------------------------------------------
	json_object_set_string(objGO, "name", object->name.data());
	json_object_set_number(objGO, "UUID", object->GetUID());
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

		(*iterator)->SaveComponent(componentObj);

		json_array_append_value(componentsObj, component);
	}

	json_array_append_value(parent, newValue);
	
	
	for (list<GameObject*>::iterator iterator = object->childs.begin(); iterator != object->childs.end(); ++iterator)
	{
		SaveGameObject(*iterator, parent);
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

	return newGameObject;
}

GameObject* ModuleGameObject::CreateEmptyGameObject(GameObject* father, const char* name)
{
	GameObject* newGameObject = nullptr;

	newGameObject = new GameObject(father, name);

	if (father)
		father->childs.push_back(newGameObject);

	gameObjectsAll.push_back(newGameObject);

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
		playingObjects[(*iterator)->UID] = (*iterator)->GetLocalMatrix();
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
			if ((*listiterator)->toDeleteFake)
			{
				(*listiterator)->SetActive(true);//SET
				App->sceneIntro->octree.Insert(*listiterator);
			}
		}
		else//Remove objects that are created while playing
		{
			(*listiterator)->toDelete = true;
			App->geometry->currentGameObject = nullptr;
		}
	}
}