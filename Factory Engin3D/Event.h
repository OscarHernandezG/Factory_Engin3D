#ifndef __Event_H__
#define __Event_H__

#include "GameObject.h"
#include "Component.h"

class Event
{
public:

	enum type
	{
		none,
		gameObjectRemoved,
		componentRemoved,
		fileDropped
	};

	union data
	{
		data() 
		{
			string.ptr = nullptr;
			gameObject.ptr = nullptr;
			component.ptr = nullptr;
			component.type = ComponentType_NONE;
		}

		struct string
		{
			const char* ptr;
		} string;
		struct gameObject
		{
			GameObject* ptr;
		}gameObject;
		struct component
		{
			Component* ptr;
			ComponentType type;
		}component;
	};

public:
	Event(type type) : type(type) {}
	~Event();


public:

	type type = none;

	data data;
};
#endif // !__Event_H__
