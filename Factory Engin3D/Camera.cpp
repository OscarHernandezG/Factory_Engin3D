#include "Camera.h"
#include "GameObject.h"


Camera::Camera(GameObject* gameObject) : Component(gameObject)
{
	frustum.type = PerspectiveFrustum;

	if (gameObject != nullptr && gameObject->transform != nullptr)
	{
		frustum.pos = gameObject->transform->GetPos();
		frustum.front = gameObject->transform->GetRotation() * float3::unitZ;
		frustum.up = gameObject->transform->GetRotation() * float3::unitY;
	}
	else
	{
		frustum.pos = float3::zero;
		frustum.front = float3::unitZ;
		frustum.up = float3::unitY;
	}

	frustum.nearPlaneDistance = 0.0f;
	frustum.farPlaneDistance = 100.0f;
	frustum.verticalFov = 60.0f * DEGTORAD;
	frustum.horizontalFov = 2.f * atanf(tanf(frustum.verticalFov * 0.5f) * (SCREEN_WIDTH / SCREEN_HEIGHT));
}


float4x4 Camera::GetViewMatrix()
{
	UpdateFrustum();

	float4x4 matrix;

	matrix = frustum.ViewMatrix();
	matrix.Transpose();

	return matrix;
}

float* Camera::GetProjectionMatrix()
{
	UpdateFrustum();

	float4x4 matrix;

	matrix = frustum.ProjectionMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}


void Camera::UpdateFrustum()
{
	if (gameObject != nullptr && gameObject->transform != nullptr)
	{
		frustum.pos = gameObject->transform->GetPos();
		frustum.front = gameObject->transform->GetRotation() * float3::unitZ;
		frustum.up = gameObject->transform->GetRotation() * float3::unitY;
	}
	else
	{
		frustum.pos = float3::zero;
		frustum.front = float3::unitZ;
		frustum.up = float3::unitY;
	}

}
