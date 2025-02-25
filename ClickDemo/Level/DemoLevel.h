#pragma once

#include "Level/Level.h"

class DrawableActor;
class Node;
class Start;
class Player;
class Path;
class DemoLevel : public Level
{
	// RTTI Á¤ÀÇ.
	RTTI_DECLARATIONS(DemoLevel, Level)

public:
	bool IsValidLocation(int row, int col);
	bool ParseMap(const char* path);
	DemoLevel();
	virtual ~DemoLevel();

	virtual void Update(float deltaTime) override;
	virtual void Draw() override;

public:
	int mapSizeX = 0;
	int mapSizeY = 0;
	std::vector<std::vector<char>> map;
	
	Node* startNode = nullptr;
	Node* goalNode = nullptr;

private:
	Start* s = nullptr;
	Player* p = nullptr;

	bool isEmpty = false;
};