#include "DemoLevel.h"
#include "Actor/Player.h"
#include "Actor/Start.h"

DemoLevel::DemoLevel()
{
	AddActor(new Start());
	AddActor(new Player());
}