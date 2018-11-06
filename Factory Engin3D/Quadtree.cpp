#include "Quadtree.h"
#include "GameObject.h"

QuadtreeNode::QuadtreeNode(const AABB & quad) : limits(quad) {}

QuadtreeNode::~QuadtreeNode()
{
	if (HasChilds())
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (childs[i] != nullptr)
			{
				delete childs[i];
				childs[i] = nullptr;
			}
		}
	}
}

void QuadtreeNode::Insert(GameObject* object)
{

	if (objectsList.size() < MAX_NODE_ELEMENTS && !HasChilds())
		objectsList.push_back(object);

	else
	{
		if (!HasChilds())
			Subdivide();

		objectsList.push_back(object);
		RedistributeChilds();
	}
}

bool QuadtreeNode::HasChilds()
{
	bool ret = false;

	if (childs[0] != nullptr)
		ret = true;
	
	return ret;
}

void QuadtreeNode::Subdivide()
{
	AABB childBox;
	//NO
	childBox.minPoint = { limits.MinX(), limits.MinY(), limits.MinZ() + limits.HalfSize().z};
	childBox.maxPoint = { limits.MaxX() - limits.HalfSize().x, limits.MaxY(), limits.MaxZ()};
	childs[0] = new QuadtreeNode(childBox);

	//NE
	childBox.minPoint = { limits.CenterPoint().x, limits.MinY(), limits.CenterPoint().z };
	childBox.maxPoint = limits.maxPoint;
	childs[1] = new QuadtreeNode(childBox);

	//SE
	childBox.minPoint = { limits.MinX() + limits.HalfSize().x,limits.MinY(), limits.MinZ() };
	childBox.maxPoint = { limits.MaxX(),limits.MaxY(), limits.MaxZ() - limits.HalfSize().z};
	childs[2] = new QuadtreeNode(childBox);

	//SO
	childBox.minPoint = limits.minPoint;
	childBox.maxPoint = { limits.CenterPoint().x, limits.MaxY(), limits.CenterPoint().z };
	childs[3] = new QuadtreeNode(childBox);

	for (int i = 0; i < 4; ++i)
	{
		childs[i]->parent = this;
	}
}

void QuadtreeNode::RedistributeChilds()
{
	std::list<GameObject*>::iterator iterator = objectsList.begin();
	while (iterator != objectsList.end())
	{
		uint intersects = 0u;
		for (int i = 0; i < 4; ++i)
		{
			if (childs[i]->limits.Intersects((*iterator)->transform->boundingBox))
				intersects++;
		}
		if (intersects == 4)
			++iterator;
		else
		{

			for (int i = 0; i < 4; ++i)
			{
				if (childs[i]->limits.Intersects((*iterator)->transform->boundingBox))
					childs[i]->Insert(*iterator);
			}
			iterator = objectsList.erase(iterator);
		}
	}
}

void QuadtreeNode::GetBoxLimits(std::vector<const QuadtreeNode*>& nodes) const
{
	nodes.push_back(this);
	for (int i = 0; i < 4; i++)
	{
		if (childs[i] != nullptr)
			childs[i]->GetBoxLimits(nodes);
		else
			break;
	}
}

void QuadtreeNode::GetGameObjects(std::vector<GameObject*>& object) const
{
	for (std::list<GameObject*>::const_iterator iterator = objectsList.begin(); iterator != objectsList.end(); ++iterator)
	{
		bool copy = false;
		for (std::vector<GameObject*>::iterator vecIterator = object.begin(); vecIterator != object.end(); ++vecIterator)
		{
			if ((*iterator) == (*vecIterator))
				copy = true;
		}
		if(!copy)
			object.push_back(*iterator);
	}
	for (int i = 0; i < 4; i++)
	{
		if (childs[i] != nullptr)
			childs[i]->GetGameObjects(object);
		else
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////


void Quadtree::Create(const AABB& limits)
{
	if (root != nullptr)
		ReDoQuadtree(limits);

	root = new QuadtreeNode(limits);
}
void Quadtree::Clear()
{
	if (root != nullptr)
	{
		delete root;
		root = nullptr;
	}
}

void Quadtree::Insert(GameObject * gameObject)
{
	if (root != nullptr)
	{
		if (root->limits.Contains(gameObject->transform->boundingBox))
			root->Insert(gameObject);
		else
			ReDoLimits(gameObject);
	}
}

void Quadtree::GetBoxLimits(std::vector<const QuadtreeNode*>& nodes) const
{
	if (root != nullptr)
			root->GetBoxLimits(nodes);
}

void Quadtree::GetGameObjects(std::vector<GameObject*>& objects) const
{
	if (root != nullptr)
		root->GetGameObjects(objects);
}

void Quadtree::ReDoQuadtree(const AABB& limits, bool external)
{
	if (root != nullptr)
	{
		std::vector<GameObject*> objects;
		GetGameObjects(objects);
		if (external)
		{
			Clear();
			Create(AABB(-float3::one,float3::one));
		}
		else
		{
			Clear();
			Create(limits);
		}
		for (std::vector<GameObject*>::iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
		{
			Insert(*iterator);
		}
	}
}

void Quadtree::ReDoLimits(GameObject* newObject)
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

		ReDoQuadtree(AABB(minPoint, maxPoint));
		Insert(newObject);
	}
}