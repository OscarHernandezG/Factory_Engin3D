#pragma once

#include "Component.h"
#include "GameObject.h"



class Texture : Component
{
public:
	Texture(GameObject* gameObject);
	~Texture() {}

public:
	uint id = 0;
};