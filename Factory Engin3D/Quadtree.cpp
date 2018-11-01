#include "Quadtree.h"
#include "GameObject.h"

QuadtreeNode::QuadtreeNode(const AABB & quad) : limits(limits) {}

void QuadtreeNode::Insert(GameObject object)
{
	if (objectsList.size() < MAX_NODE_ELEMENTS && !HasChilds())
		objectsList.push_back(object);

	else
	{
		if (!HasChilds())
			Subdivide();
		objectsList.push_back(object);
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
	childBox.minPoint = { limits.MinX(),limits.MinY(), limits.MinZ() + limits.Size().z / 2 };
	childBox.maxPoint = { limits.MaxX() - limits.Size().x / 2 ,limits.MaxY(), limits.MaxZ()};
	childs[0] = new QuadtreeNode(childBox);

	//NE
	childBox.minPoint = limits.CenterPoint();
	childBox.maxPoint = limits.maxPoint;
	childs[1] = new QuadtreeNode(childBox);

	//SE
	childBox.minPoint = { limits.MinX() + limits.Size().x / 2 ,limits.MinY(), limits.MinZ() };
	childBox.maxPoint = { limits.MaxX(),limits.MaxY(), limits.MaxZ() - limits.Size().z / 2 };
	childs[2] = new QuadtreeNode(childBox);

	//SO
	childBox.minPoint = limits.minPoint;
	childBox.maxPoint = limits.CenterPoint();
	childs[3] = new QuadtreeNode(childBox);

	for (int i = 0; i < 4; ++i)
	{
		childs[i]->parent = this;
	}
}




////////////////////////////////////////////////////////////////////////////////////////



void Quadtree::Create(const AABB& limits)
{
	root = new QuadtreeNode(limits);
}
void Quadtree::Clear()
{
	if (root != NULL)
	{
		delete root;
		root = NULL;
	}
}

void Quadtree::Insert(GameObject * gameObject)
{
	if(root != nullptr)
		if (gameObject->transform->boundingBox.Intersects(root->limits))
		{
			root->Insert(gameObject);
		}
}

