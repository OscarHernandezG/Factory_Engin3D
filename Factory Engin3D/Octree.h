#ifndef __Quadtree_H__
#define __Quadtree_H__

#include "MathGeoLib/Geometry/AABB.h"
#include "SDL/include/SDL.h"

#include <list>
#include <vector>
#include <algorithm>

#include "Brofiler/Brofiler.h"

#define MAX_NODE_ELEMENTS 10

class GameObject;

class OctreeNode
{
public:
	OctreeNode(const AABB& limits);
	~OctreeNode();

	void Insert(GameObject* object);
	bool HasChilds();
	void Subdivide();
	void RedistributeChilds();
	void GetBoxLimits(std::vector<const OctreeNode*>& nodes) const;
	void GetGameObjects(std::vector<GameObject*>& object) const;
	template<typename TYPE>
	void GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const;

public:
	AABB limits;
	std::list<GameObject*> objectsList;
	OctreeNode* childs[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	int subdivisions = 0;
};

class Octree
{
public:
	Octree() {};
	~Octree() {};
	void Create(const AABB& limits);
	void Clear();
	void Insert(GameObject* gameObject);
	void GetBoxLimits(std::vector<const OctreeNode*>& nodes) const;
	void GetGameObjects(std::vector<GameObject*>& object) const;
	void ReDoOctree(std::vector<GameObject*> objects);  //External Redo
	void ReDoOctree(const AABB& limits);				//Internal Redo
	void ReDoLimits(GameObject * newObject);
	void UniqueObjects(std::vector<GameObject *> & objects) const;
	template<typename TYPE>
	void GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const;

public:

	OctreeNode* root = nullptr;

};

template<typename TYPE>
inline void Octree::GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);
	if (root != nullptr)
	{
		root->GetIntersects(objects, primitive);
		UniqueObjects(objects);
	}
}

template<typename TYPE>
inline void OctreeNode::GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);
	if (primitive.Intersects(limits))
	{
		for (std::list<GameObject*>::const_iterator iterator = objectsList.begin(); iterator != objectsList.end(); ++iterator)
		{
			if (primitive.Intersects((*iterator)->transform->boundingBox))
				objects.push_back(*iterator);

		}
		for (int i = 0; i < 8; ++i)
		{
			if (childs[i] != nullptr)
				childs[i]->GetIntersects(objects, primitive);
			else
				break;
		}
	}
}

#endif
