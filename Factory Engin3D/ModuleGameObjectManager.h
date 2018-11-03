#ifndef __ModuleGameObjectManager_H__
#define __ModuleGameObjectManager_H__

#include "Module.h"

#include "Transform.h"
#include "GameObject.h"

class ModuleGameObjectManager : public Module
{
public:
	ModuleGameObjectManager(Application* app, bool start_enabled = true);
	~ModuleGameObjectManager();

	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();

	void CleanAllGameObjects();

	GameObject* CreateGameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr, char* name = nullptr);

public:
	GameObject* root;

	list<GameObject*> gameObjects;

};
#endif // !__ModuleGameObjectManager_H__