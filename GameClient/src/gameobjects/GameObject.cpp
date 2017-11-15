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
