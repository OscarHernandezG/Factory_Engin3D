#ifndef __Quadtree_H__
#define __Quadtree_H__

#include "MathGeoLib/Geometry/AABB.h"
#include "SDL/include/SDL.h"

#include <list>

#define MAX_NODE_ELEMENTS 4

class GameObject;

class QuadtreeNode
{
public:
	QuadtreeNode(const AABB& limits);
	~QuadtreeNode() {};

	void Insert(GameObject object);
	bool HasChilds();
	void Subdivide();

public:
	AABB limits;
	std::list<GameObject> objectsList;
	QuadtreeNode* parent = nullptr;
	QuadtreeNode* childs[4] = { nullptr, nullptr, nullptr, nullptr };
};

class Quadtree
{
public:
	Quadtree() {};
	~Quadtree() {};
	void Create(const AABB& limits);
	void Clear();
	void Insert(GameObject* gameObject);

public:

	QuadtreeNode* root = nullptr;

};
#endif
