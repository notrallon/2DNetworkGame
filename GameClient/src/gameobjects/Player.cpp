#include "Player.h"
#include "Projectile.h"

#include <app/SharedContext.h>

static int s_playerCount = 0;

Player::Player() : m_MousePressed(false)
{
	m_Sprite.setSize(sf::Vector2f(40, 50));
	m_Sprite.setFillColor(sf::Color::Blue);
	m_Speed = 300;
}

Player::Player(SharedContext * context, bool owned) : GameObject::GameObject(context), m_ClientOwned(owned)
{
	m_Sprite.setSize(sf::Vector2f(40, 50));
	m_Sprite.setOrigin(m_Sprite.getSize().x / 2, m_Sprite.getSize().y / 4);

	if(!owned)
		m_Sprite.setFillColor(sf::Color::Blue);
	else
		m_Sprite.setFillColor(sf::Color::Red);

	m_Speed = 300;
	m_ObjectInfo.IP = sf::IpAddress::getLocalAddress();
	m_ObjectInfo.Speed = m_Speed;
	m_ClientOwned = owned;
	m_ObjectInfo.ID = 0;

	s_playerCount++;
}

Player::~Player()
{
}

void Player::Update(const float& dt)
{
	// Update the players info
	m_ObjectInfo.Position = m_Sprite.getPosition();
	if (!m_ClientOwned) return;

	sf::Vector2f dir = sf::Vector2f(0.0f, 0.0f);

	if (m_Context->window->hasFocus())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		{
			m_Sprite.move(sf::Vector2f(-m_Speed * dt, 0));
			dir += sf::Vector2f(-1, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		{
			m_Sprite.move(sf::Vector2f(m_Speed * dt, 0));
			dir += sf::Vector2f(1, 0);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
		{
			m_Sprite.move(sf::Vector2f(0, -m_Speed * dt));
			dir += sf::Vector2f(0, -1);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
		{
			sf::RectangleShape temp = m_Sprite;
			temp.move(sf::Vector2f(0, m_Speed * dt));
			if (temp.getGlobalBounds().intersects(Game::groundFloor.getGlobalBounds()))
			{
				temp.setPosition(m_Sprite.getPosition().x, m_Sprite.getPosition().y + 1);
				// Move the sprite down a pixel until it's about to hit the floor
				while (!temp.getGlobalBounds().intersects(Game::groundFloor.getGlobalBounds()))
				{
					m_Sprite.setPosition(temp.getPosition());
					temp.setPosition(m_Sprite.getPosition().x, m_Sprite.getPosition().y + 1);
				}
			}
			else
			{
				m_Sprite.move(sf::Vector2f(0, m_Speed * dt));
				dir += sf::Vector2f(0, 1);
			}
		}

		//Temp
		if (m_Sprite.getGlobalBounds().intersects(Game::groundFloor.getGlobalBounds()) && dir.y == 1)
			dir.y = 0;

		//if(m_Sprite.getGlobalBounds().intersects()

		m_ObjectInfo.MousePosition = sf::Vector2f(sf::Mouse::getPosition(*m_Context->window));
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !m_MousePressed)
		{
			m_MousePressed = true;
			m_ObjectInfo.Shooting = true;
		}
		else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && m_MousePressed)
		{
			m_MousePressed = false;
			m_ObjectInfo.Shooting = false;
		}
	}
	m_ObjectInfo.Direction = dir;
}

const ObjectInfo& Player::GetObjectInfo() const
{
	return m_ObjectInfo;
}

void Player::SetObjectInfo(ObjectInfo info)
{
	m_ObjectInfo.ID = info.ID;
	m_ObjectInfo.Position = info.Position;
	m_ObjectInfo.Direction = info.Direction;
	m_ObjectInfo.Speed = info.Speed;
	m_ObjectInfo.IP = info.IP;
	m_ObjectInfo.Port = info.Port;
	m_ObjectInfo.Connected = info.Connected;
	m_ObjectInfo.ObjectType = info.ObjectType;
	m_Sprite.setPosition(m_ObjectInfo.Position);
}

void Player::Draw(sf::RenderWindow& window)
{
	window.draw(m_Sprite);
}
