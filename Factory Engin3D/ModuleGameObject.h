#ifndef __ModuleGameObjectManager_H__
#define __ModuleGameObjectManager_H__

#include "Module.h"

#include "Transform.h"
#include "GameObject.h"

class ModuleGameObject : public Module
{
	friend class GameObject;
public:
	ModuleGameObject(Application* app, bool start_enabled = true);
	~ModuleGameObject();

	bool Start();

	update_status Update();
	update_status PostUpdate();

	void RemoveObjectsFromList(GameObject * it);

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

	void SaveComponent(Component * object, JSON_Object * parent);

	GameObject* CreateGameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr, const char* name = nullptr);
	GameObject* CreateEmptyGameObject(GameObject* father, const char* name);
	void AddNewDynamic(GameObject * object);
	void RemoveDynamic(GameObject * object);

	bool CanTransform(GameObject * object);

public:
	GameObject* rootGameObject;

	list<GameObject*> gameObjectsAll;

	list<GameObject*> dynamicObjects;

};
#endif // !__ModuleGameObjectManager_H__