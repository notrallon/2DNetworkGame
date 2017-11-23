#include "GameObject.h"
#include <app/SharedContext.h>


GameObject::GameObject()
{
}

GameObject::GameObject(SharedContext * context) : m_Context(context)
{
}


GameObject::~GameObject()
{
}

const ObjectInfo & GameObject::GetObjectInfo() const
{
	return m_ObjectInfo;
}

void GameObject::SetObjectInfo(ObjectInfo info)
{
	m_ObjectInfo = info;
}
