#ifndef __ModuleCamera_H__
#define __ModuleCamera_H__

#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

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
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	void Look(bool RotateAroundReference);
	void LookAt(const float3 &Spot);
	void Move(const float3 &Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

public:

	float3 X, Y, Z, Position, Reference;

private:
	MouseButton cameraMovementButton = MouseButton_Right;
	bool isCameraFocused = false;

	float cameraSpeed = 3;

	float4x4 ViewMatrix, ViewMatrixInverse;
};
#endif