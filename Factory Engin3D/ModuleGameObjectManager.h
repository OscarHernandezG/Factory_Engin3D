#ifndef __ModuleGameObjectManager_H__
#define __ModuleGameObjectManager_H__

#include "Module.h"

#include "Transform.h"
#include "GameObject.h"

class ModuleGameObjectManager : public Module
{
	friend class GameObject;
public:
	ModuleGameObjectManager(Application* app, bool start_enabled = true);
	~ModuleGameObjectManager();

	bool Start();

	update_status Update();
	update_status PostUpdate();

	void RemoveObjectsFromList(GameObject * it);

	bool CleanUp();

	void CleanAllGameObjects();

	void SaveScene();

	void SaveGameObject(GameObject * object, JSON_Object * parent);

	void SaveComponent(Component * object, JSON_Object * parent);

	GameObject* CreateGameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr, const char* name = nullptr);

	void AddNewDynamic(GameObject * object);
	void RemoveDynamic(GameObject * object);

public:
	GameObject* rootGameObject;

	list<GameObject*> gameObjectsAll;

	list<GameObject*> dynamicObjects;

};
#endif // !__ModuleGameObjectManager_H__