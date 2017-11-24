#include "Projectile.h"

#include <iostream>

Projectile::Projectile()
{
}

Projectile::Projectile(const sf::Vector2f& startpos, const sf::Vector2f& direction) : m_Direction(direction)
{
	m_Sprite.setPosition(startpos);
	m_Sprite.setRadius(6);
	m_Sprite.setOrigin(3, 3);
	m_Sprite.setFillColor(sf::Color::Green);
	m_Speed = 1000;
}

Projectile::~Projectile()
{
}

void Projectile::Update(const float& dt)
{
	m_Sprite.move(m_Direction * m_Speed * dt);
}

void Projectile::Draw(sf::RenderWindow& window)
{
	window.draw(m_Sprite);
}

void Projectile::SetObjectInfo(ObjectInfo info)
{
	GameObject::SetObjectInfo(info);
	m_Sprite.setPosition(info.Position);
}
