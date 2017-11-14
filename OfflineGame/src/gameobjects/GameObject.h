#pragma once

#include <SFML/Graphics.hpp>

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Update(const float& dt) = 0;
	virtual void Draw(sf::RenderWindow& window) = 0;

private:


};

