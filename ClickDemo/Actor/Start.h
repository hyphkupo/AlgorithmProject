#pragma once

#include "Actor/DrawableActor.h"
#include "Level/DemoLevel.h"

class Start : public DrawableActor
{
	RTTI_DECLARATIONS(Start, DrawableActor)

public:
	Start(DemoLevel* refLevel);

	virtual void Update(float deltaTime) override;

private:
	DemoLevel* refLevel = nullptr;

};