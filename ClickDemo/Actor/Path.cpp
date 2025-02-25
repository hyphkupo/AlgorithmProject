#include "Path.h"

Path::Path(const Vector2& position)
	: DrawableActor("+")
{
	color = Color::Blue;
	this->position = position;
}