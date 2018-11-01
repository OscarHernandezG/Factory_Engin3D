#ifndef __Texture_H__
#define __Texture_H__

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
#endif // !__Texture_H__