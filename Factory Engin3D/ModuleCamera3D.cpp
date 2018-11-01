#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Light.h"
#include "MathGeoLib/MathGeoLib.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	X = math::float3(1.0f, 0.0f, 0.0f);
	Y = math::float3(0.0f, 1.0f, 0.0f);
	Z = math::float3(0.0f, 0.0f, 1.0f);

	//camera->transform->position = math::float3(0.0f, 0.0f, 5.0f);
	Reference = math::float3(0.0f, 0.0f, 0.0f);

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

	LookAt(float3(0, 0, 0));
	Look(false);

	CalculateViewMatrix();
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");
	
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	float speed = cameraSpeed * dt;

	if (!App->gui->IsAnyWindowHovered())
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)	speed *= 2.0f;

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT))
		{
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
				camera->transform->position += float3::unitY * speed;

			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
				camera->transform->position -= float3::unitY * speed;

			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
				camera->transform->position += camera->transform->rotation * float3::unitZ * speed;

			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
				camera->transform->position -= camera->transform->rotation * float3::unitZ * speed;

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
				camera->transform->position += camera->transform->rotation * float3::unitX * speed;

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
				camera->transform->position -= camera->transform->rotation * float3::unitX * speed;
		}


		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			Look(App->geometry->GetBBPos(), App->geometry->GetCurrentMeshPivot(), false);


		//Zoom with wheel
		float wheel = (float)App->input->GetMouseZ();
		if (wheel != 0)
		{
			// We do not use dt to zoom with the mouse wheel, the speed at which the mouse wheel rotates is independent of the fps
			// The movement of the mouse wheel is not a "KEY_REPEAT" every frame but a "KEY_DOWN" occasionally
			camera->transform->position += camera->transform->rotation * float3::unitZ * wheel * cameraSpeed;
		}

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_DOWN)
		{
			isCameraFocused = true;
			cameraMovementButton = MouseButton_Left;
		}


		if (isCameraFocused)
		{
			LookAt(App->geometry->GetCurrentMeshPivot());

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
					camera->transform->position += camera->transform->rotation * float3::unitZ * motion * dt;
				}
			}
		}

		//Mouse motion ----------------

	   //if(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_REPEAT)
		if (App->input->GetMouseButton(cameraMovementButton) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = App->input->GetMouseYMotion();

			Quat rotation = Quat::identity;

			float sensitivity = 1.0f;

			//camera->transform->position -= Reference;

			if (dx != 0)
			{
				float deltaX = (float)dx * sensitivity;


				rotation = Quat::RotateAxisAngle(float3::unitY, deltaX * dt);
				camera->transform->Rotate(rotation);

			}

			if (dy != 0)
			{
				float deltaY = (float)dy * sensitivity / 2;

				rotation = Quat::RotateAxisAngle(float3::unitX, deltaY * dt);
				camera->transform->Rotate(rotation);

			}
			//	camera->transform->position = Reference + Z * camera->transform->position.Length();
		}
	}
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	//camera->transform->position = Position;
	//this->Reference = Reference;

	//Z = (Position - Reference).Normalized();
	//X = Cross(math::float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	//Y = Cross(Z, X);

	//if (!RotateAroundReference)
	//{
	//	this->Reference = this->camera->transform->position;
	//	this->camera->transform->position += Z * 0.05f;
	//}

	//CalculateViewMatrix();
}

void ModuleCamera3D::Look(bool RotateAroundReference)
{
//	Look(camera->transform->position, Reference, RotateAroundReference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	//Reference = Spot;

	//Z = (camera->transform->position - Reference).Normalized();
	//X = Cross(math::float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	//Y = Cross(Z, X);

	//CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &Movement)
{
	//camera->transform->position += Movement;
	//Reference += Movement;

	//CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return cameraComponent->GetViewMatrix().ptr();
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = cameraComponent->GetViewMatrix();
	ViewMatrixInverse = ViewMatrix;
	ViewMatrixInverse.Inverse();
}