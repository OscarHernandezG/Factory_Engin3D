#ifndef __Quadtree_H__
#define __Quadtree_H__

#include "MathGeoLib/Geometry/AABB.h"
#include "SDL/include/SDL.h"

#include <list>
#include <vector>
#include <algorithm>

#define MAX_NODE_ELEMENTS 10

class GameObject;

class QuadtreeNode
{
public:
	QuadtreeNode(const AABB& limits);
	~QuadtreeNode();

	void Insert(GameObject* object);
	bool HasChilds();
	void Subdivide();
	void RedistributeChilds();
	void GetBoxLimits(std::vector<const QuadtreeNode*>& nodes) const;
	void GetGameObjects(std::vector<GameObject*>& object) const;
	template<typename TYPE>
	void GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const;

public:
	AABB limits;
	std::list<GameObject*> objectsList;
	QuadtreeNode* childs[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	int subdivisions = 0;
};

class Quadtree
{
public:
	Quadtree() {};
	~Quadtree() {};
	void Create(const AABB& limits);
	void Clear();
	void Insert(GameObject* gameObject);
	void GetBoxLimits(std::vector<const QuadtreeNode*>& nodes) const;
	void GetGameObjects(std::vector<GameObject*>& object) const;
	void ReDoQuadtree(const AABB & limits = AABB(float3::zero,float3::zero), bool external = false);
	void ReDoLimits(GameObject * newObject);
	void UniqueObjects(std::vector<GameObject *> & objects) const;
	template<typename TYPE>
	void GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const;

public:

	QuadtreeNode* root = nullptr;

};

template<typename TYPE>
inline void Quadtree::GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const
{
	if (root != nullptr)
	{
		root->GetIntersects(objects, primitive);
		UniqueObjects(objects);
	}
}

template<typename TYPE>
inline void QuadtreeNode::GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const
{
	if (primitive.Intersects(limits))
	{
		for (std::list<GameObject*>::const_iterator iterator = objectsList.begin(); iterator != objectsList.end(); ++iterator)
		{
			if (primitive.Intersects((*iterator)->transform->boundingBox))
				objects.push_back(*iterator);

		}
		for (int i = 0; i < 4; ++i)
		{
			if (childs[i] != nullptr)
				childs[i]->GetIntersects(objects, primitive);
			else
				break;
		}
	}
}

#endif
