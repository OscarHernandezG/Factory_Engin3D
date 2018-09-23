#pragma once
#include "Module.h"
#include "Globals.h"
//#include "glmath.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4.h"
#include "MathGeoLib/src/Math/float4x4.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	void LookAt(const float3 &Spot);
	void Move(const float3 &Movement);
	float4x4* GetViewMatrix();

private:

	void CalculateViewMatrix();

public:
	
	float3 X, Y, Z, Position, Reference;

private:

	float4x4 ViewMatrix, ViewMatrixInverse;
};