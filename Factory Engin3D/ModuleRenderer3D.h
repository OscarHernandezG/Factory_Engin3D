#ifndef __ModuleRender_H__
#define __ModuleRender_H__
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3x3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "Light.h"
#include "Camera.h"

#include <queue>

#define MAX_LIGHTS 8

struct compare {
	bool operator()(GameObject* infoA, GameObject* infoB)
	{
		return infoA->GetCamDistance() < infoB->GetCamDistance();
	}
};

union toOrder
{
	GameObject* gameobject;
	Component* component;
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D(); 

	bool Init();
	bool Start();
	void DrawOctreeObjects(GameObject * iterator);
	void DrawDynamicObjects(bool cameraCulling);
	update_status PreUpdate();
	update_status PostUpdate();
	void DebugDraw();
	void DrawObject(Component * geometry);
	bool CleanUp();

	void OnResize(int width, int height);

	void LoadProjectionMatrix();

	void SetLightAmbient();


	math::float4x4 Perspective(float fovy, float aspect, float n, float f) const;

	void DrawQuad(float3 * corners, Color color = White);

	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	Frustum GetCameraFrustrum() const;

	update_status Save(JSON_Object * object);
	update_status Load(JSON_Object * object);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	float3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool vsync = false;
	uint my_id = 0u;

	bool isFill = true;
	bool isWire = true;
	uint buffer_id = 0u;

	float2 prevSize{ SCREEN_WIDTH,SCREEN_HEIGHT };
	float2 currSize{ SCREEN_WIDTH,SCREEN_HEIGHT };

	float3 ambient_lihgt = { 1.0f,1.0f,1.0f };

	bool debugQuad = false;
	bool cameraCulling = false;

	Camera* currentCam = nullptr;

	std::priority_queue<GameObject*, std::vector<GameObject*>, compare> transparentObjects;
};
#endif // !__ModuleRender_H__
