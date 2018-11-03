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
				delete[] childs[i];
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

template<typename TYPE>
void QuadtreeNode::GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const
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


////////////////////////////////////////////////////////////////////////////////////////


void Quadtree::Create(const AABB& limits)
{
	if (root != nullptr)
		Clear();

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
	if(root != nullptr)
		if (gameObject->transform->boundingBox.Intersects(root->limits))
			root->Insert(gameObject);
		
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

template<typename TYPE>
void Quadtree::GetIntersects(std::vector<GameObject*>& objects, const TYPE & primitive) const
{
	if (root != nullptr)
		root->GetIntersects(objects, primitive);
}
