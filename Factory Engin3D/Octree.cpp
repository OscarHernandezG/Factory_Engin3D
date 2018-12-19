#include "Octree.h"
#include "GameObject.h"

OctreeNode::OctreeNode(const AABB & quad) : limits(quad) {}

OctreeNode::~OctreeNode()
{
	if (HasChilds())
	{
		for (size_t i = 0; i < 8; i++)
		{
			if (childs[i] != nullptr)
			{
				delete childs[i];
				childs[i] = nullptr;
			}
		}
	}
}

void OctreeNode::Insert(GameObject* object)
{

	if (objectsList.size() < MAX_NODE_ELEMENTS && !HasChilds() || subdivisions >= 5)
		objectsList.push_back(object);

	else
	{
		if (!HasChilds())
			Subdivide();

		objectsList.push_back(object);
		RedistributeChilds();
	}
}

bool OctreeNode::HasChilds()
{
	bool ret = false;

	if (childs[0] != nullptr)
		ret = true;

	return ret;
}

void OctreeNode::Subdivide()
{
	AABB childBox;

	///////////Bottom
	//NOB
	childBox.minPoint = { limits.MinX(), limits.MinY(), limits.MinZ() + limits.HalfSize().z };
	childBox.maxPoint = { limits.MaxX() - limits.HalfSize().x,  limits.MaxY() - limits.HalfSize().y, limits.MaxZ() };
	childs[0] = new OctreeNode(childBox);

	//NEB
	childBox.minPoint = { limits.CenterPoint().x, limits.MinY(), limits.CenterPoint().z };
	childBox.maxPoint = { limits.MaxX(), limits.MaxY() - limits.HalfSize().y, limits.MaxZ() };
	childs[1] = new OctreeNode(childBox);

	//SEB
	childBox.minPoint = { limits.MinX() + limits.HalfSize().x,limits.MinY(), limits.MinZ() };
	childBox.maxPoint = { limits.MaxX(), limits.MaxY() - limits.HalfSize().y, limits.MaxZ() - limits.HalfSize().z };
	childs[2] = new OctreeNode(childBox);

	//SOB
	childBox.minPoint = limits.minPoint;
	childBox.maxPoint = { limits.CenterPoint().x, limits.MaxY() - limits.HalfSize().y, limits.CenterPoint().z };
	childs[3] = new OctreeNode(childBox);

	///////////Top
	//NOT
	childBox.minPoint = { limits.MinX(),  limits.MinY() + limits.HalfSize().y, limits.MinZ() + limits.HalfSize().z };
	childBox.maxPoint = { limits.MaxX() - limits.HalfSize().x, limits.MaxY(), limits.MaxZ() };
	childs[4] = new OctreeNode(childBox);

	//NET
	childBox.minPoint = { limits.CenterPoint().x, limits.MinY() + limits.HalfSize().y, limits.CenterPoint().z };
	childBox.maxPoint = limits.maxPoint;
	childs[5] = new OctreeNode(childBox);

	//SET
	childBox.minPoint = { limits.MinX() + limits.HalfSize().x, limits.MinY() + limits.HalfSize().y, limits.MinZ() };
	childBox.maxPoint = { limits.MaxX(),limits.MaxY(), limits.MaxZ() - limits.HalfSize().z };
	childs[6] = new OctreeNode(childBox);

	//SOT
	childBox.minPoint = { limits.MinX(), limits.MinY() + limits.HalfSize().y, limits.MinZ() };
	childBox.maxPoint = { limits.CenterPoint().x, limits.MaxY(), limits.CenterPoint().z };
	childs[7] = new OctreeNode(childBox);

	for (int i = 0; i < 8; ++i)
	{
		childs[i]->subdivisions = subdivisions + 1;
	}
}

void OctreeNode::RedistributeChilds()
{
	std::list<GameObject*>::iterator iterator = objectsList.begin();
	while (iterator != objectsList.end())
	{
		uint intersects = 0u;
		for (int i = 0; i < 8; ++i)
		{
			if (childs[i]->limits.Intersects((*iterator)->transform->boundingBox))
				intersects++;
		}
		if (intersects == 8)
			++iterator;
		else
		{

			for (int i = 0; i < 8; ++i)
			{
				if (childs[i]->limits.Intersects((*iterator)->transform->boundingBox))
					childs[i]->Insert(*iterator);
			}
			iterator = objectsList.erase(iterator);
		}
	}
}

void OctreeNode::GetBoxLimits(std::vector<const OctreeNode*>& nodes) const
{
	nodes.push_back(this);
	for (int i = 0; i < 8; i++)
	{
		if (childs[i] != nullptr)
			childs[i]->GetBoxLimits(nodes);
		else
			break;
	}
}

void OctreeNode::GetGameObjects(std::vector<GameObject*>& object) const
{
	for (std::list<GameObject*>::const_iterator iterator = objectsList.begin(); iterator != objectsList.end(); ++iterator)
			object.push_back(*iterator);
	
	for (int i = 0; i < 8; i++)
	{
		if (childs[i] != nullptr)
			childs[i]->GetGameObjects(object);
		else
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////


void Octree::Create(const AABB& limits)
{
	if (root != nullptr)
		ReDoOctree(limits);

	root = new OctreeNode(limits);
}
void Octree::Clear()
{
	if (root != nullptr)
	{
		delete root;
		root = nullptr;
	}
}

void Octree::Insert(GameObject * gameObject)
{
	if (root != nullptr && gameObject->transform->boundingBox.IsFinite() && gameObject->GetObjectStatic() 
		&& gameObject->GetActive() && !gameObject->toDelete)
	{
		if (root->limits.Contains(gameObject->transform->boundingBox))
			root->Insert(gameObject);
		else
			ReDoLimits(gameObject);
	}
}

void Octree::GetBoxLimits(std::vector<const OctreeNode*>& nodes) const
{
	if (root != nullptr)
		root->GetBoxLimits(nodes);
}

void Octree::GetGameObjects(std::vector<GameObject*>& objects) const
{
	if (root != nullptr)
	{
		root->GetGameObjects(objects);
		UniqueObjects(objects);
	}
}

void Octree::ReDoOctree(std::vector<GameObject*> objects) // External Redo
{
	if (root != nullptr)
	{
		Clear();
		if (!objects.empty())
		{
			Create((*objects.begin())->transform->boundingBox);

			for (std::vector<GameObject*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
			{
				Insert(*iterator);
			}
		}
		else
			Create(AABB(float3::zero,float3::zero));
	}
}
void Octree::ReDoOctree(const AABB & limits) //Internal Redo
{
	if (root != nullptr)
	{
		std::vector<GameObject*> objects;
		GetGameObjects(objects);

		Clear();
		Create(limits);

		for (std::vector<GameObject*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
		{
			Insert(*iterator);
		}
	}
}

void Octree::ReDoLimits(GameObject* newObject)
{
	if (root != nullptr)
	{
		float3 minPoint = root->limits.minPoint;
		float3 maxPoint = root->limits.maxPoint;
		float3 objectMin = newObject->transform->boundingBox.minPoint;
		float3 objectMax = newObject->transform->boundingBox.maxPoint;

		if (minPoint.x > objectMin.x)
			minPoint.x = objectMin.x;
		if (minPoint.y > objectMin.y)
			minPoint.y = objectMin.y;
		if (minPoint.z > objectMin.z)
			minPoint.z = objectMin.z;

		if (maxPoint.x < objectMax.x)
			maxPoint.x = objectMax.x;
		if (maxPoint.y < objectMax.y)
			maxPoint.y = objectMax.y;
		if (maxPoint.z < objectMax.z)
			maxPoint.z = objectMax.z;

		ReDoOctree(AABB(minPoint, maxPoint));
		Insert(newObject);
	}
}

void Octree::UniqueObjects(std::vector<GameObject*>& objects) const
{
	if (!objects.empty())
	{
		std::sort(objects.begin(), objects.end()); //all equals objects together
		objects.erase(std::unique(objects.begin(), objects.end()), objects.end());
		//erase corrupt objects after unique(that returns the new last object position)
	}
}