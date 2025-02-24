#include "Start.h"
#include "Engine/Engine.h"

Start::Start(DemoLevel* refLevel)
	: DrawableActor("s"), refLevel(refLevel)
{
	color = Color::Red;
}

void Start::Update(float deltaTime)
{
	Super::Update(deltaTime);

	if (Engine::Get().GetKeyDown(VK_LBUTTON))
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