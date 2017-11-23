#pragma once

#include "GameObject.h"

class Projectile : public GameObject
{
public:
	Projectile();
	Projectile(const sf::Vector2f& startpos, const sf::Vector2f& direction);
	~Projectile();

	virtual void Update(const float& dt) override;
	virtual void Draw(sf::RenderWindow& window) override;

	void						SetObjectInfo(ObjectInfo info) override;

private:
	sf::CircleShape m_Sprite;
	sf::Vector2f	m_Direction;
	float			m_Speed;
};