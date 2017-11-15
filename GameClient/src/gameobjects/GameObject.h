#pragma once

#include <SFML/Graphics.hpp>

struct SharedContext;

class GameObject
{
public:
	GameObject();
	GameObject(SharedContext* context);
	~GameObject();

	virtual void Update(const float& dt) = 0;
	virtual void Draw(sf::RenderWindow& window) = 0;

protected:
	SharedContext* m_Context;

};

