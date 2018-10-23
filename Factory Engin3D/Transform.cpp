#include "Transform.h"


Transform::Transform(TransformInfo* info) : Component(info->gameObject, ComponentType_TRANSFORM)
{
	switch (info->whichInfo)
	{
	case UsingInfo_TRS:
		transform.FromTRS(info->position, info->rotation, info->scale);
		break;
	case UsingInfo_TRANSFORM:
		transform = info->transform;
	default:
		break;
	}

}
