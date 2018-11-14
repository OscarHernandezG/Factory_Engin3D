#ifndef __ModuleGameObjectManager_H__
#define __ModuleGameObjectManager_H__

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

	void SaveGameObject(GameObject * object, JSON_Object * parent);

	void SaveComponent(Component * object, JSON_Object * parent);

	GameObject* CreateGameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr, const char* name = nullptr);

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