#include "Projectile.h"

Projectile::Projectile()
{
}

Projectile::Projectile(const sf::Vector2f & startpos, const sf::Vector2f & direction) : m_Direction(direction)
{
	m_Sprite.setPosition(startpos);
}

Projectile::~Projectile()
{
}

void Projectile::Update(const float& dt)
{
	m_Sprite.move(m_Direction * dt);
}

void Projectile::Draw(sf::RenderWindow& window)
{
	window.draw(m_Sprite);
}
