#include "Player.h"
#include "Engine/Engine.h"

Player::Player(DemoLevel* refLevel)
	: DrawableActor("e"), refLevel(refLevel)
{
	color = Color::Green;
	//position = Vector2(5, 5);
}

void Player::Update(float deltaTime)
{
	DrawableActor::Update(deltaTime);

	if (Engine::Get().GetKeyDown(VK_ESCAPE))
	{
		Engine::Get().QuitGame();
	}

	if (Engine::Get().GetKeyDown(VK_RBUTTON))
	{
		if (Engine::Get().MousePosition().x >= refLevel->mapSizeX)
		{
			if (Engine::Get().MousePosition().y >= refLevel->mapSizeY)
			{
				position.x = refLevel->mapSizeX - 1;
				position.y = refLevel->mapSizeY - 1;
			}

			else
			{
				position.x = refLevel->mapSizeX - 1;
				position.y = Engine::Get().MousePosition().y;
			}
		}

		else if (Engine::Get().MousePosition().y >= refLevel->mapSizeY)
		{
			position.x = Engine::Get().MousePosition().x;
			position.y = refLevel->mapSizeY - 1;
		}

		else
		{
			position = Engine::Get().MousePosition();
		}
	}
}