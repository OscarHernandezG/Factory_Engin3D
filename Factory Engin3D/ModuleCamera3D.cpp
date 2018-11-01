#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

#include "MathGeoLib/MathGeoLib.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	X = math::float3(1.0f, 0.0f, 0.0f);
	Y = math::float3(0.0f, 1.0f, 0.0f);
	Z = math::float3(0.0f, 0.0f, 1.0f);

	Position = math::float3(0.0f, 0.0f, 5.0f);
	Reference = math::float3(0.0f, 0.0f, 0.0f);

	CalculateViewMatrix();
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	bool ret = true;

	LOG("Setting up the camera");

	LookAt(float3(0, 0, 0));
	Look(false);

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
	float3 newPos(0, 0, 0);
	float speed = cameraSpeed * dt;
	if (!App->gui->IsAnyWindowHovered())
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)	speed *= 2.0f;

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT))
		{
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;
			if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;
			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;
		}


		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
			Look(App->geometry->GetBBPos(), App->geometry->GetCurrentMeshPivot(), false);


		//Zoom with wheel
		float wheel = (float)App->input->GetMouseZ();
		if (wheel != 0)
		{
			// We do not use dt to zoom with the mouse wheel, the speed at which the mouse wheel rotates is independent of the fps
			// The movement of the mouse wheel is not a "KEY_REPEAT" every frame but a "KEY_DOWN" occasionally
			newPos += (Reference - Position).Normalized() * wheel * cameraSpeed;
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
					newPos += (Reference - Position).Normalized() * motion * dt;
				}
			}

		}

		Position += newPos;
		Reference += newPos;

		//Mouse motion ----------------

	   //if(App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_REPEAT)
		if (App->input->GetMouseButton(cameraMovementButton) == KEY_REPEAT)
		{
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			float Sensitivity = 0.01f;

			Position -= Reference;

			if (dx != 0)
			{
				float DeltaX = (float)dx * Sensitivity;

				float3x3 rotationMatrix = float3x3::RotateY(DeltaX);
				X = rotationMatrix * X;
				Y = rotationMatrix * Y;
				Z = rotationMatrix * Z;
			}

			if (dy != 0)
			{
				float DeltaY = (float)dy * Sensitivity / 2;

				float3x3 rotationMatrix = float3x3::RotateAxisAngle(X, DeltaY);
				Y = rotationMatrix * Y;
				Z = rotationMatrix * Z;

				if (Y.y < 0.0f)
				{
					Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
					Y = Cross(Z, X);
				}
			}

			Position = Reference + Z * Position.Length();
		}

	}
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = Cross(math::float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	Y = Cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void ModuleCamera3D::Look(bool RotateAroundReference)
{
	Look(Position, Reference, RotateAroundReference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	Reference = Spot;

	Z = (Position - Reference).Normalized();
	X = Cross(math::float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	Y = Cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return ViewMatrix.ptr();
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -Dot(X, Position), -Dot(Y, Position), -Dot(Z, Position), 1.0f);
	ViewMatrixInverse = ViewMatrix;
	ViewMatrixInverse.Inverse();
}


















// FUTURE CAMERA

/*#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "Light.h"
#include "MathGeoLib/MathGeoLib.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	X = math::float3(1.0f, 0.0f, 0.0f);
	Y = math::float3(0.0f, 1.0f, 0.0f);
	Z = math::float3(0.0f, 0.0f, 1.0f);

	Position = math::float3(0.0f, 0.0f, 5.0f);
	Reference = math::float3(0.0f, 0.0f, 0.0f);

	CalculateViewMatrix();
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
			int dy = -App->input->GetMouseYMotion();

			float sensitivity = 0.01f;

			Position -= Reference;

			if (dx != 0)
			{
				float deltaX = (float)dx * sensitivity;

				Quat rotation = Quat::RotateAxisAngle(float3::unitY, deltaX * dt);
				camera->transform->SetRotation(rotation);
			}

			if (dy != 0)
			{
				float deltaY = (float)dy * sensitivity / 2;

				Quat rotation = Quat::RotateAxisAngle(float3::unitX, deltaY * dt);
				camera->transform->SetRotation(rotation);
			}

			Position = Reference + Z * Position.Length();
		}

	}
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	camera->transform->position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = Cross(math::float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	Y = Cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void ModuleCamera3D::Look(bool RotateAroundReference)
{
	Look(Position, Reference, RotateAroundReference);
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	Reference = Spot;

	Z = (Position - Reference).Normalized();
	X = Cross(math::float3(0.0f, 1.0f, 0.0f), Z).Normalized();
	Y = Cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
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
*/