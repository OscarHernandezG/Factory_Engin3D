#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Light.h"
#include "MathGeoLib/MathGeoLib.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	bool ret = true;

	LOG("Setting up the camera");

	camera = new GameObject(nullptr);

	cameraComponent = (Camera*)camera->AddComponent(ComponentType_CAMERA, nullptr);

	cameraComponent->frustum.pos = float3(5, 5, 5);

	LookAt(float3(0, 0, 0));
	Look(false);

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	delete camera;
	
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update()
{
	float speed = cameraSpeed * App->time->Getdt();

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
	{
		isCameraFocused = true;
		cameraMovementButton = MouseButton_Left;
	}

	if (!App->gui->IsAnyWindowHovered())
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)	speed *= 2.0f;


		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT))
		{
			float3 forward = cameraComponent->frustum.front * speed;
			float3 right = cameraComponent->frustum.WorldRight() * speed;
			float3 movement = float3::zero;

			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
				movement += float3::unitY * speed;

			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
				movement -= float3::unitY * speed;

			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
				movement += forward;

			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
				movement -= forward;

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
				movement -= right;

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
				movement += right;

			cameraComponent->frustum.Translate(movement);
		}



		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			Look(App->geometry->currentGameObject->GetBBPos(), App->geometry->GetCurrentMeshPivot(), false);


		//Zoom with wheel
		float wheel = (float)App->input->GetMouseZ();
		if (wheel != 0)
		{
			// We do not use dt to zoom with the mouse wheel, the speed at which the mouse wheel rotates is independent of the fps
			// The movement of the mouse wheel is not a "KEY_REPEAT" every frame but a "KEY_DOWN" occasionally
			cameraComponent->frustum.pos += cameraComponent->frustum.front * wheel * cameraSpeed;
		}


		if (isCameraFocused)
		{
			//LookAt(App->geometry->GetCurrentMeshPivot());

			if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_UP)
			{
				Look(false);
				isCameraFocused = false;
				cameraMovementButton = MouseButton_Right;
			}

			else if (App->input->GetMouseButton(MouseButton_Right) == KEY_REPEAT)
			{
				float motion = App->input->GetMouseXMotion() + App->input->GetMouseYMotion();
				if (motion != 0)
				{
					cameraComponent->frustum.pos += cameraComponent->frustum.front * motion * App->time->Getdt();
				}
			}
		}

		//Mouse motion ----------------

	   //if(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_REPEAT)
		if (App->input->GetMouseButton(cameraMovementButton) == KEY_REPEAT)
		{
			float sensitivity = 0.01f;

			float dx = -App->input->GetMouseXMotion() * sensitivity;
			float dy = -App->input->GetMouseYMotion() * sensitivity;


			//camera->transform->position -= Reference;
			if (isCameraFocused)
				OrbitArroundReference(dx, dy, App->geometry->GetCurrentMeshPivot());
			else
				FreeLook(dx, dy);
			//	camera->transform->position = Reference + Z * camera->transform->position.Length();
		}
	}
	return UPDATE_CONTINUE;
}

void ModuleCamera3D::OrbitArroundReference(float dx, float dy, float3 reference)
{

	float3 originalPosition = cameraComponent->frustum.pos - reference;

	Quat yRotation(cameraComponent->frustum.up , dx);
	Quat xRotation(cameraComponent->frustum.WorldRight(), dy);

	float3 newPosition = originalPosition;

	newPosition = xRotation.Transform(newPosition);
	newPosition = yRotation.Transform(newPosition);

	if (CanMove(originalPosition, newPosition, reference))
		cameraComponent->frustum.pos = newPosition + reference;

	LookAt(reference);
}

bool ModuleCamera3D::CanMove(float3 originalPosition, float3 newPosition, float3 reference)
{
	bool ret = false;

	float margin = 0.5f;

	if (originalPosition.x < reference.x - margin || originalPosition.x >  reference.x + margin || originalPosition.z < reference.z - margin || originalPosition.z >  reference.z + margin)
		ret = true;
	else if (abs(originalPosition.x - reference.x) < abs(newPosition.x - reference.x) || abs(originalPosition.z - reference.z) < abs(newPosition.z - reference.z))
		ret = true;

	return ret;
}


void ModuleCamera3D::FreeLook(float dx, float dy)
{
	if (dx != 0)
	{
		float deltaX = (float)dx;


		Quat rotation = Quat::RotateY(deltaX);
		cameraComponent->frustum.front = (rotation *cameraComponent->frustum.front);

		cameraComponent->frustum.up = (rotation * cameraComponent->frustum.up);
	}

	if (dy != 0)
	{
		float deltaY = (float)dy;

		Quat rotation = Quat::RotateAxisAngle(cameraComponent->frustum.WorldRight(), deltaY);

		float3 newFrustUp = (rotation * cameraComponent->frustum.up);

		if (newFrustUp.y > 0.0f)
		{
			cameraComponent->frustum.up = newFrustUp;
			cameraComponent->frustum.front = (rotation * cameraComponent->frustum.front);
		}
	}
}
// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	SetPos(Position);

	LookAt(Reference);
}

void ModuleCamera3D::Look(bool RotateAroundReference)
{
//	Look(camera->transform->position, Reference, RotateAroundReference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &spot)
{
	float3 direction = spot - cameraComponent->frustum.pos;

	float3x3 lookMat = float3x3::LookAt(cameraComponent->frustum.front, direction.Normalized(), cameraComponent->frustum.up, float3::unitY);

	cameraComponent->frustum.front = lookMat.MulDir(cameraComponent->frustum.front).Normalized();
	cameraComponent->frustum.up = lookMat.MulDir(cameraComponent->frustum.up).Normalized();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &movement)
{
	cameraComponent->frustum.Translate(movement);
}

float3 ModuleCamera3D::GetPos() const
{
	return cameraComponent->GetPos();
}

void ModuleCamera3D::SetPos(float3 pos)
{
	cameraComponent->SetPos(pos);
}

// -----------------------------------------------------------------

Camera* ModuleCamera3D::SetEditorCamera() const
{
	return cameraComponent;
}