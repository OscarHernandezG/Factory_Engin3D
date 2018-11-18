#ifndef __ModuleGameObject_H__
#define __ModuleGameObject_H__

#include "Module.h"

#include "Transform.h"
#include "GameObject.h"

#include <map>

class ModuleGameObject : public Module
{
	friend class GameObject;
public:
	ModuleGameObject(Application* app, bool start_enabled = true);
	~ModuleGameObject();

	bool Start();

	update_status Update();
	update_status PostUpdate();

	void RemoveObjectsFromList(GameObject * it, std::list<GameObject*> &toDelete);

	bool CleanUp();

	void CleanAllGameObjects();

	void SaveScene();

	void LoadScene();

	void SetGOMeshNewScene(Mesh * itMesh, std::list<GameObject *>::iterator &it);

	void LoadNewSceneMeshes(std::vector<uint> &meshesToLoad);

	void LoadNewSceneGO(int numObjects, JSON_Array * objArray, std::vector<uint> &meshesToLoad, std::list<GameObject *> &sceneGameObjects);

	void DeletePreviousScene();

	GameObject * FindByID(uint UUID);

	void SaveGameObject(GameObject * object, JSON_Array*& parent);

	GameObject* CreateGameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr, const char* name = nullptr);
	GameObject* CreateEmptyGameObject(GameObject* father, const char* name);
	void AddNewDynamic(GameObject * object);
	void RemoveDynamic(GameObject * object);

	bool CanTransform(GameObject * object);

	void SaveBeforePlay();
	void LoadAfterPlay();


public:
	GameObject* rootGameObject;

	std::list<GameObject*> gameObjectsAll;

	std::list<GameObject*> dynamicObjects;

	std::map<uint, float4x4> playingObjects;
};
#endif // !__ModuleGameObjectManager_H__