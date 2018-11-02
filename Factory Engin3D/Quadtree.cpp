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

void QuadtreeNode::Insert(GameObject object)
{

	if (objectsList.size() > MAX_NODE_ELEMENTS && !HasChilds())
		Subdivide();

	else
	{
		//if (!HasChilds())

	}

	objectsList.push_back(object);
	RedistributeChilds();
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

void QuadtreeNode::RedistributeChilds()
{
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




////////////////////////////////////////////////////////////////////////////////////////



void Quadtree::Create(const AABB& limits)
{
	root = new QuadtreeNode(limits);
	if (root != nullptr)
		LOG("Quadtree Created");
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

