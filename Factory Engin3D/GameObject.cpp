#include "Application.h"

#include "GameObject.h"
#include "ComponentTexture.h"
#include "ComponentBillboard.h"
#include "pcg-c-basic-0.9/pcg_basic.h"

#include "ComponentEmitter.h"
#include "ModuleParticles.h"

using namespace std;

GameObject::GameObject(GameObject* father, const char * name)
{
	this->father = father;

	if (name != nullptr)
		this->name = name;
	else this->name = "noName";
}

GameObject::GameObject(float3 position, Quat rotation, float3 scale, GameObject* father, const char* name)
{
	TransformInfo* info = new TransformInfo();
	info->position = position;
	info->rotation = rotation;
	info->scale = scale;
	info->whichInfo = UsingInfo_TRS;

	CreateGameObject(info);
	delete info;

	this->father = father;

	if (name != nullptr)
		this->name = name;
	else this->name = "noName";
}


GameObject::~GameObject()
{
	transform = nullptr;
}

void GameObject::RemoveChilds()
{
	for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
	{
		(*iterator)->RemoveComponents();
		(*iterator)->RemoveChilds();
		delete (*iterator);
	}
	childs.clear();
}

void GameObject::RemoveComponents()
{
	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		delete (*iterator);
	}
	components.clear();
}


void GameObject::RealDelete()
{
	transform = nullptr;

	if (father)
		father->childs.remove(this);

	RemoveComponents();

	RemoveChilds();

	father = nullptr;
}

void GameObject::Update()
{
	if (isActive)
	{
		for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
		{
			(*iterator)->Update();
		}

		for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
		{
			(*iterator)->Update();
		}
	}
}

void GameObject::PostUpdate()
{
	if (isActive)
	{

		list<Component*> toDelete;
		for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
		{
			if ((*iterator)->toDelete)
			{
				toDelete.push_back(*iterator);
			}

		}
		for (list<Component*>::iterator iterator = toDelete.begin(); iterator != toDelete.end(); ++iterator)
		{
			RemoveComponent(*iterator);

			App->gameObject->redoOc = true;
		}

		for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
		{
			(*iterator)->PostUpdate();
		}
	}

	//Can draw is calculated in renderer using the octree
	canDraw = false;
}

bool GameObject::SetParent(GameObject* parent)
{
	bool ret = false;
	if (parent != nullptr)
	{
		if(father)
			father->childs.remove(this);

		this->father = parent;
		parent->childs.push_back(this);
		ret = true;
	}

	return ret;
}

void GameObject::CreateFromJson(JSON_Object* info, vector<uint>& meshesToLoad)
{
	UID = json_object_get_number(info, "UUID");
	parentUUID = json_object_get_number(info, "Parent UUID");

	JSON_Array* components = json_object_get_array(info, "Components");

	for (int i = 0; i < json_array_get_count(components); i++) {
		JSON_Object* comp = json_array_get_object(components, i);

		ComponentType type = (ComponentType)(int)json_object_get_number(comp, "Type");
		if (type != ComponentType_GEOMETRY)
		{
			ComponentInfo* compInfo = LoadComponentInfo(comp, type);
			Component* newComponent = AddComponent(type, compInfo);

			if (type == ComponentType_CAMERA)
			{
				App->geometry->cameraObject = this;
				App->geometry->playingCamera = (Camera*)newComponent;
			}
			delete compInfo;
		}
		else if (type == ComponentType_GEOMETRY)
		{
			uint compUUID = json_object_get_number(comp, "UUID");

			Mesh* mesh = new Mesh(this);
			mesh->SetUUID(compUUID);

			GeometryInfo inf;
			inf.geometry = mesh;
			Component* newComponent = AddComponent(type, &inf);

			meshesToLoad.push_back(compUUID);
		}
	}
}

ComponentInfo* GameObject::LoadComponentInfo(JSON_Object* info, ComponentType type)
{
	ComponentInfo* ret = nullptr;

	switch (type)
	{
	case ComponentType_TRANSFORM:
	{
		TransformInfo* compInfo = new TransformInfo();

//// Position
////------------------------------------------------------------------------
		JSON_Object* position = json_object_get_object(info, "Position");

		float3 pos;
		pos.x = json_object_get_number(position, "X");
		pos.y = json_object_get_number(position, "Y");
		pos.z = json_object_get_number(position, "Z");

//// Scale
////------------------------------------------------------------------------
		JSON_Object* scale = json_object_get_object(info, "Scale");

		float3 size;
		size.x = json_object_get_number(scale, "X");
		size.y = json_object_get_number(scale, "Y");
		size.z = json_object_get_number(scale, "Z");

//// Rotation
////------------------------------------------------------------------------
		JSON_Object* rotation = json_object_get_object(info, "Rotation");

		Quat rot;
		rot.x = json_object_get_number(rotation, "X");
		rot.y = json_object_get_number(rotation, "Y");
		rot.z = json_object_get_number(rotation, "Z");
		rot.w = json_object_get_number(rotation, "W");

////------------------------------------------------------------------------
		compInfo->UUID = json_object_get_number(info, "UUID");

		
		compInfo->position = pos;
		compInfo->rotation = rot.Normalized();
		compInfo->scale = size;


		ret = (ComponentInfo*)compInfo;
	}
		break;
	case ComponentType_GEOMETRY:
	{
		GeometryInfo* compInfo = new GeometryInfo();

		compInfo->UUID = json_object_get_number(info, "UUID");

		ret = (ComponentInfo*)compInfo;
	}
		break;
	case ComponentType_CAMERA:
	{
		CameraInfo* camInfo = new CameraInfo();

		camInfo->farPlaneDistance = json_object_get_number(info, "farPlaneDistance");
		camInfo->nearPlaneDistance = json_object_get_number(info, "nearPlaneDistance");
		camInfo->verticalFov = json_object_get_number(info, "verticalFov");
		camInfo->horizontalFov = json_object_get_number(info, "horizontalFov");

		JSON_Object* position = json_object_get_object(info, "Position");

		float3 pos;
		pos.x = json_object_get_number(position, "X");
		pos.y = json_object_get_number(position, "Y");
		pos.z = json_object_get_number(position, "Z");
		camInfo->position = pos;

		ret = (ComponentInfo*)camInfo;
	}
		break;
	case ComponentType_TEXTURE:
	{
		TextureInfo* textInfo = new TextureInfo();
		textInfo->texture = App->resources->LoadTexture(json_object_get_string(info, "Name"));

		ret = (ComponentInfo*)textInfo;
	}
	break;
	case ComponentType_EMITTER:
	{
		EmitterInfo* emitterInfo = new EmitterInfo();

		emitterInfo->duration = json_object_get_number(info, "duration");

		emitterInfo->loop = json_object_get_number(info, "loop");

		emitterInfo->burst = json_object_get_number(info, "burst");
		emitterInfo->minPart = json_object_get_number(info, "minPart");
		emitterInfo->maxPart = json_object_get_number(info, "maxPart");
		emitterInfo->repeatTime = json_object_get_number(info, "repeatTime");

		emitterInfo->isSubEmitter = json_object_get_boolean(info, "isSubEmitter");

		// posDifAABB
		float3 posDifAABB = float3(
			json_object_get_number(info, "posDifAABBX"),
			json_object_get_number(info, "posDifAABBY"),
			json_object_get_number(info, "posDifAABBZ"));
		emitterInfo->posDifAABB = posDifAABB;

		emitterInfo->gravity = json_object_get_number(info, "gravity");

		// boxCreation
		float3 boxMin = float3(
			json_object_get_number(info, "boxCreationMinX"),
			json_object_get_number(info, "boxCreationMinY"),
			json_object_get_number(info, "boxCreationMinZ"));

		float3 boxMax = float3(
			json_object_get_number(info, "boxCreationMaxX"),
			json_object_get_number(info, "boxCreationMaxY"),
			json_object_get_number(info, "boxCreationMaxZ"));

		emitterInfo->boxCreation = AABB(boxMin, boxMax);

		// SphereCreation
		emitterInfo->SphereCreation_rad = json_object_get_number(info, "SphereCreation_rad");

		emitterInfo->shapeType = (ShapeType)(int)json_object_get_number(info, "shapeType");
		emitterInfo->texture = App->resources->LoadTexture(json_object_get_string(info, "texture"));
		
		emitterInfo->isParticleAnimated = json_object_get_boolean(info, "isParticleAnimated");
		emitterInfo->dieOnAnimation = json_object_get_boolean(info, "dieOnAnimation");
		emitterInfo->textureColumns = json_object_get_number(info, "textureColumns");
		emitterInfo->textureRows = json_object_get_number(info, "textureRows");
		emitterInfo->animationSpeed = json_object_get_number(info, "animationSpeed");

		//AABB Colision
		emitterInfo->drawAABB = json_object_get_boolean(info, "drawAABB");

		emitterInfo->checkLife = json_object_get_boolean(info, "checkLife");
		emitterInfo->checkSpeed = json_object_get_boolean(info, "checkSpeed");
		emitterInfo->checkAcceleration = json_object_get_boolean(info, "checkAcceleration");
		emitterInfo->checkSize = json_object_get_boolean(info, "checkSize");
		emitterInfo->checkRotation = json_object_get_boolean(info, "checkRotation");
		emitterInfo->checkAngularAcceleration = json_object_get_boolean(info, "checkAngularAcceleration");
		emitterInfo->checkAngularVelocity = json_object_get_boolean(info, "checkAngularVelocity");

		emitterInfo->subEmiterUUID = json_object_get_number(info, "SubEmitter");

		emitterInfo->rateOverTime = json_object_get_number(info, "rateOverTime");

		emitterInfo->subEmitterActive = json_object_get_boolean(info, "subEmitterActive");


		StartValues* startValues = &emitterInfo->startValues;

		startValues->life.x = json_object_get_number(info, "lifeMin");
		startValues->life.y = json_object_get_number(info, "lifeMax");

		startValues->speed.x = json_object_get_number(info, "speedMin");
		startValues->speed.y = json_object_get_number(info, "speedMax");

		startValues->acceleration.x = json_object_get_number(info, "accelerationMin");
		startValues->acceleration.y = json_object_get_number(info, "accelerationMax");

		startValues->size.x = json_object_get_number(info, "sizeMin");
		startValues->size.y = json_object_get_number(info, "sizeMax");

		startValues->rotation.x = json_object_get_number(info, "rotationMin");
		startValues->rotation.y = json_object_get_number(info, "rotationMax");

		startValues->angularVelocity.x = json_object_get_number(info, "angularVelocityMin");
		startValues->angularVelocity.y = json_object_get_number(info, "angularVelocityMax");

		startValues->angularAcceleration.x = json_object_get_number(info, "angularAccelerationMin");
		startValues->angularAcceleration.y = json_object_get_number(info, "angularAccelerationMax");

		startValues->angularAcceleration.y = json_object_get_number(info, "angularAccelerationMax");

		JSON_Array* colorArray = json_object_get_array(info, "Colors");

		int numColors = json_array_get_count(colorArray);

		startValues->color.clear();
		startValues->color.resize(numColors);
		int i = 0;
		for (std::list<ColorTime>::iterator iterator = startValues->color.begin(); iterator != startValues->color.end(); ++iterator, ++i)
		{
			JSON_Object* currCol = json_array_get_object(colorArray, i);

			(*iterator).color = float4(
				json_object_get_number(currCol, "colorX"),
				json_object_get_number(currCol, "colorY"),
				json_object_get_number(currCol, "colorZ"),
				json_object_get_number(currCol, "colorW")
			);

			(*iterator).position = json_object_get_number(currCol, "position");

			(*iterator).name = json_object_get_string(currCol, "name");
		}


		startValues->timeColor = json_object_get_number(info, "timeColor");

		startValues->subEmitterActive = json_object_get_number(info, "subEmitterActive");

		startValues->particleDirection.x = json_object_get_number(info, "particleDirectionX");
		startValues->particleDirection.y = json_object_get_number(info, "particleDirectionY");
		startValues->particleDirection.x = json_object_get_number(info, "particleDirectionZ");

		ret = emitterInfo;
	}
		break;
	default:
		break;
	}

	return ret;
}

void GameObject::CreateGameObject(TransformInfo* info)
{
	UID = pcg32_random();

	this->transform = (Transform*)AddComponent(ComponentType_TRANSFORM, info);
	if(transform)
		transform->SetUUID(pcg32_random());
}

//Components
//-------------------------------------------------------------------------
	// Add
Component* GameObject::AddComponent(ComponentType type, ComponentInfo* info)
{
	Component* newComponent = nullptr;

	if (info)
		info->gameObject = this;

	switch (type)
	{
	case ComponentType_TRANSFORM:
		if (info)
			newComponent = (Component*)new Transform((TransformInfo*)info);
		if (newComponent)
		{
			if (transform)
			{
				components.remove(transform);
				delete transform;
			}

			transform = (Transform*)newComponent;
		}
			break;
	case ComponentType_GEOMETRY:
		if (info)
			newComponent = (Component*)(((GeometryInfo*)info)->geometry);
		else
			newComponent = nullptr;
		break;
	case ComponentType_CAMERA:
		if (info)
			newComponent = (Component*)new Camera(this, (CameraInfo*)info);
		
		else
		newComponent = (Component*)new Camera(this);
		break;
	case ComponentType_TEXTURE:
		newComponent = (Component*)new Texture(this, (TextureInfo*)info);
		break;
	case ComponentType_LIGHT:
		break;
	case ComponentType_BILLBOARD:
		newComponent = (Component*)new Billboard(this);
		break;
	case ComponentType_EMITTER:
		newComponent = /*(Component*)*/new ComponentEmitter(this, (EmitterInfo*)info);

		App->particle->emitters.push_back((ComponentEmitter*)newComponent);
		break;
	default:
		break;
	}

	if (newComponent)
	{
		newComponent->type = type;
		newComponent->isActive = true;
		newComponent->gameObject = this;
		components.push_back(newComponent);
	}

	return newComponent;
}
	// Remove
void GameObject::RemoveComponent(Component* component)
{
	if (component != nullptr)
	{
		if (component->type == ComponentType_EMITTER)
		{
			((ComponentEmitter*)component)->ClearEmitter();
			App->particle->emitters.remove((ComponentEmitter*)component);
			LOG("Emitter removed");
		}
		components.remove(component);
		delete component;
		component = nullptr;
	}
}
	// Get
Component* GameObject::GetComponent(ComponentType type)
{
	Component* component = nullptr;

	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		if ((*iterator)->type == type)
		{
			component = (*iterator);
			break;
		}
	}

	return component;
}

list<Component*> GameObject::GetAllComponent(ComponentType type)
{
	list<Component*> component;

	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		if ((*iterator)->type == type)
		{
			components.push_back(*iterator);
		}
	}

	return component;
}


bool GameObject::HasComponent(ComponentType type)
{
	bool ret = false;
	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		if ((*iterator)->type == type)
		{
			ret = true;
			break;
		}
	}
	return ret;
}
//-------------------------------------------------------------------------


//Position
//-------------------------------------------------------------------------
	// Set
void GameObject::SetPos(float3 pos)
{
	if (transform)
	{
		transform->SetPos(pos);
	}
}

void GameObject::Move(float3 movement)
{
	if (transform)
		transform->Move(movement);
}
	// Get
float3 GameObject::GetPos() const
{
	if (transform)
		return transform->GetPos();

	return float3::zero;
}

float3 GameObject::GetGlobalPos() const
{
	if (transform)
		return transform->GetGlobalPos();

	return float3::zero;
}
float GameObject::GetCamDistance()
{
	return App->geometry->GetPlayingCamera()->GetPos().DistanceSq(GetGlobalPos());
}
//-------------------------------------------------------------------------

float3 GameObject::GetScale() const
{
	if (transform)
		return transform->GetScale();

	return float3::one;
}

float3 GameObject::GetGlobalScale() const
{
	if (transform)
		return transform->GetGlobalScale();

	return float3::one;
}

float4x4 GameObject::GetGlobalMatrix() const
{
	float4x4 mat = float4x4::identity;
	if (transform)
	{
		mat = transform->GetMatrix();
	}

	return mat;
}

float4x4 GameObject::GetLocalMatrix() const
{
	float4x4 mat = float4x4::identity;
	if (transform)
	{
		mat = transform->GetLocalMatrix();
	}

	return mat;
}

Quat GameObject::GetRotation() const
{
	if (transform)
		return transform->GetRotation().Normalized();
}

Quat GameObject::GetGlobalRotation() const
{
	if (transform)
		return transform->GetGlobalRotation().Normalized();
}

void GameObject::SetTransform(float4x4 trans)
{
	if (transform)
	{
		transform->SetTransform(trans);
	}
}



void GameObject::SetScale(float3 scale)
{
	if (transform)
	{
		transform->SetScale(scale);
	}
}

void GameObject::Scale(float3 scale)
{
	if (transform)
	{
		transform->Scale(scale);
	}
}

void GameObject::SetRotation(Quat rotation)
{
	if (transform)
	{
		transform->SetRotation(rotation);
	}
}

void GameObject::Rotate(Quat rotation)
{
	if (transform)
	{
		transform->Rotate(rotation);
	}
}

void GameObject::SetIdentity()
{
	if (transform)
	{
		transform->SetIdentity();
	}
}

const AABB* GameObject::GetAABB() const
{
	return &transform->boundingBox;
}

float3 GameObject::GetBBPos()
{
	float3 distance{ 0,0,0 };
	if (transform)
	{
		float3 size = transform->boundingBox.Size();

		if (!childs.empty())
		{
			size = GetGlobalAABB(AABB(float3::zero,float3::zero)).Size();
		}

		float3 reScale = GetScale();
		distance.x = (size.x / 2) / math::Tan((0.3333333 * reScale.x));
		distance.y = (size.y / 2) / math::Tan((0.3333333 * reScale.y));
		distance.z = (size.z / 2) / math::Tan((0.3333333 * reScale.z));
	}
	return distance + GetGlobalPos();
}

AABB GameObject::GetGlobalAABB(AABB localAABB)
{
	AABB globalAABB = localAABB;
	if (!childs.empty())
		for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
		{
			globalAABB = (*iterator)->GetGlobalAABB(globalAABB);
		}
		
	else if(HasComponent(ComponentType_GEOMETRY))
	{
		float3 objectMin = GetAABB()->minPoint;
		float3 objectMax = GetAABB()->maxPoint;

		if (globalAABB.MinX() > objectMin.x)
			globalAABB.minPoint.x = objectMin.x;
		if (globalAABB.MinY() > objectMin.y)
			globalAABB.minPoint.y = objectMin.y;
		if (globalAABB.MinZ() > objectMin.z)
			globalAABB.minPoint.z = objectMin.z;

		if (globalAABB.MaxX() < objectMax.x)
			globalAABB.maxPoint.x = objectMax.x;
		if (globalAABB.MaxY() < objectMax.y)
			globalAABB.maxPoint.y = objectMax.y;
		if (globalAABB.MaxZ() < objectMax.z)
			globalAABB.maxPoint.z = objectMax.z;
	}
	return globalAABB;
}

void GameObject::SetABB(AABB aabb)
{
	if (transform)
	{
		transform->originalBoundingBox = aabb;

		transform->UpdateBoundingBox();
	}
}

void GameObject::SetActive(bool active)
{
	for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
	{
		(*iterator)->SetActive(active);
	}
	isActive = active;
}

void GameObject::SetObjectStatic(bool isStatic)
{
	for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
	{
		(*iterator)->SetObjectStatic(isStatic);
	}
	this->isStatic = isStatic;
}

int GameObject::CreateRandomUID()
{
	return 0;
}
