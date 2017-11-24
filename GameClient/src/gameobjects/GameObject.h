#pragma once

#include <app/Game.h>
#include <SFML/Graphics.hpp>

struct SharedContext;

class GameObject
{
public:
								GameObject();
								GameObject(SharedContext* context);
								~GameObject();

	virtual void				Update(const float& dt) = 0;
	virtual void				Draw(sf::RenderWindow& window) = 0;

	virtual const ObjectInfo&	GetObjectInfo() const;
	virtual void				SetObjectInfo(ObjectInfo info);

protected:
	SharedContext*				m_Context;
	ObjectInfo					m_ObjectInfo;

};

