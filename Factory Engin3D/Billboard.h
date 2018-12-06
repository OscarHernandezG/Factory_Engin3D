#ifndef __Billboard_H__
#define __Billboard_H__

#include "Component.h"
#include "GameObject.h"

enum Billboard_TYPE
{
	Billboard_NONE,
	Billboard_SCREEN,
	Billboard_WORLD,
	Billboard_AXIALY,
};
class Billboard : Component
{
public:
	Billboard(GameObject* gameObject);

	~Billboard();

	void Update();
	void Inspector();

public:
	Billboard_TYPE typeBoard = Billboard_NONE;
private:
	std::string currentType = "NONE";
	bool xAxisRot = false;
	bool yAxisRot = true; 
	bool zAxisRot = false;
};
#endif // !__Billboard_H__