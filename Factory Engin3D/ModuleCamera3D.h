#ifndef __ModuleCamera_H__
#define __ModuleCamera_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

#include "GameObject.h"

enum MouseButton
{
	MouseButton_Left = SDL_BUTTON_LEFT,
	MouseButton_Middle = SDL_BUTTON_MIDDLE,
	MouseButton_Right = SDL_BUTTON_RIGHT
};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update();
	void OrbitArroundReference(float dx, float dy, float3 reference);
	bool CanMove(float3 originalPosition, float3 newPosition, float3 reference);
	void FreeLook(float dx, float dy);
	bool CleanUp();

	void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	void Look(bool RotateAroundReference);
	void LookAt(const float3 &Spot);
	void Move(const float3 &Movement);

	float3 GetPos() const;
	void SetPos(float3 pos);

	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	Frustum GetCameraFrustrum() const;


public:
	GameObject* camera = nullptr;

private:
	MouseButton cameraMovementButton = MouseButton_Right;
	bool isCameraFocused = false;

	float cameraSpeed = 3;

	Camera* cameraComponent = nullptr;
};

#endif // !__ModuleCamera_H__
