#include "DemoLevel.h"

#include "Actor/DrawableActor.h"
#include "Actor/Player.h"
#include "Actor/Start.h"
#include "Actor/Wall.h"
#include "Actor/Ground.h"

#include "Algorithm/AStar.h"
#include "Algorithm/Node.h"

#include "Engine/Engine.h"

#include <iostream>
#include <vector>

struct Location2D
{
	Location2D(int row = 0, int col = 0)
		: row(row), col(col)
	{
	}

	int row;
	int col;
};

bool DemoLevel::IsValidLocation(int row, int col)
{
	if (row < 0 || row >= mapSizeX || col < 0 || col >= mapSizeY)
	{
		return false;
	}

	return map[row][col] == '0' || map[row][col] == 'x';
}

bool DemoLevel::ParseMap(const char* path)
{
	FILE* file = nullptr;
	fopen_s(&file, path, "r");

	if (file)
	{
		char buffer[2048] = { };
		if (!fgets(buffer, 2048, file))
		{
			fclose(file);
			return false;
		}

		map.clear();

		sscanf_s(buffer, "size: %d", &mapSizeY);

		std::vector<std::string> lines;
		lines.reserve(mapSizeY);

		auto currentPosition = ftell(file);

		fseek(file, 0, SEEK_END);

		auto endPosition = ftell(file);
		int size = (int)(endPosition - currentPosition);

		fseek(file, currentPosition, SEEK_SET);
		size_t bytesRead = fread_s(buffer, 2048, size, 1, file);

		// 파일 읽을 때 사용할 인덱스.
		int index = 0;

		// 좌표 계산을 위한 변수 선언.
		int xPosition = 0;
		int yPosition = 0;

		// 해석 (파싱-Parcing).
		while (index < (int)bytesRead)
		{
			// 한 문자씩 읽기.
			char mapChar = buffer[index++];

			// 개행 문자인 경우 처리.
			if (mapChar == '\n')
			{
				++yPosition;
				xPosition = 0;
				continue;
			}

			// 맵 문자가 0이면 그라운드 액터 생성
			else if (mapChar == '1')
			{
				Ground* ground = new Ground(Vector2(xPosition, yPosition));
				actors.push_back(ground);
			}

			else if (mapChar == '0')
			{
				Wall* wall = new Wall(Vector2(xPosition, yPosition));
				actors.push_back(wall);
			}

			++xPosition;
		}

		char* context = nullptr;
		char* splitString = strtok_s(buffer, "\n", &context);
		if (splitString)
		{
			lines.emplace_back(splitString);
		}

		while (context)
		{
			splitString = strtok_s(nullptr, "\n", &context);
			if (!splitString)
			{
				break;
			}

			lines.emplace_back(splitString);
		}

		std::vector<char> line;
		line.reserve(mapSizeY);

		splitString = nullptr;

		for (auto& item : lines)
		{
			context = nullptr;

			splitString = strtok_s(const_cast<char*>(item.c_str()), ",", &context);
			if (splitString)
			{
				line.emplace_back(splitString[0]);
			}

			while (context)
			{
				splitString = strtok_s(nullptr, ",", &context);
				if (!splitString)
				{
					break;
				}

				line.emplace_back(splitString[0]);
			}

			map.emplace_back(line);
			line.clear();
		}
		return true;
		fclose(file);
	}
	return false;
}

DemoLevel::DemoLevel()
{
	//ParseMap("../Assets/Map.txt");

	map =
	{
		{ '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1' },
		{ '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '1', '1' },
		{ '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', 'e', '1' },
		{ '1', '0', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '0', '0', '1' },
		{ '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1' },
		{ '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1' },
		{ '1', '0', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1' },
		{ '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1' },
		{ '1', 's', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1' },
		{ '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1' },
	};

	//if (ParseMap("../Assets/Map.txt"))
	if (true)
	{
		bool initialize = false;
		mapSizeY = map.size();
		mapSizeX = map[0].size();

		for (int y = 0; y < mapSizeY; ++y)
		{
			for (int x = 0; x < mapSizeX; ++x)
			{
				if (map[y][x] == '1')
				{
					Wall* wall = new Wall(Vector2(x, y));
					actors.push_back(wall);
				}
				if (map[y][x] == '0')
				{
					Ground* ground = new Ground(Vector2(x, y));
					actors.push_back(ground);
				}
				if (map[y][x] == 's' || map[y][x] == 'e')
				{
					Ground* ground = new Ground(Vector2(x, y));
					actors.push_back(ground);
				}
			}
		}

		for (int y = 0; y < mapSizeY; ++y)
		{
			if (startNode && goalNode)
			{
				initialize = true;
				break;
			}

			for (int x = 0; x < mapSizeX; ++x)
			{
				if (startNode && goalNode)
				{
					initialize = true;
					break;
				}

				if (map[y][x] == 's')
				{
					startNode = new Node(Vector2(x, y));
					map[y][x] = '0';
					continue;
				}

				if (map[y][x] == 'e')
				{
					goalNode = new Node(Vector2(x, y));
					map[y][x] = '0';
					continue;
				}
			}
		}

		s = new Start(this);
		p = new Player(this);
		s->SetPosition(startNode->position);
		p->SetPosition(goalNode->position);
		AddActor(s);
		AddActor(p);
	}
}

void DemoLevel::Update(float deltaTime)
{
	Super::Update(deltaTime);

	if (Engine::Get().GetKeyDown(VK_SPACE))
	{
		AStar aStar;

		startNode->position = s->Position();
		goalNode->position = p->Position();

		//if (map[startNode->position.x][startNode->position.y] == '1')
		//{
		//	actors[startNode->position.x * mapSize + startNode->position.y - 1] = new Ground(startNode->position);
		//}

		std::vector<Node*> path = aStar.FindPath(startNode, goalNode, map);

		if (!path.empty())
		{
			aStar.DisplayGridWithPath(map, path);
		}

		//if (!path.empty())
		//{
		//	//Engine::Get().Clear();
		//	/*aStar.DisplayGridWithPath(map, path);*/
		//	for (const Node* node : path)
		//	{
		//		// 경로는 '2'로 표시.
		//		//map[node->position.y][node->position.x] = '2';
		//		//pathChar.push_back(node->position);
		//		//Engine::Get().Draw(node->position, "+", Color::Blue);
		//	}
		//}
	}
}

void DemoLevel::Draw()
{
	Super::Draw();

	//if (Engine::Get().GetKeyDown(VK_SPACE))
	//{
	//	AStar aStar;

	//	startNode->position = s->Position();
	//	//if (map[startNode->position.x][startNode->position.y] == '1')
	//	//{
	//	//	actors[startNode->position.x * mapSize + startNode->position.y - 1] = new Ground(startNode->position);
	//	//}
	//	goalNode->position = p->Position();
	//	std::vector<Node*> path = aStar.FindPath(startNode, goalNode, map);

	//	if (!path.empty())
	//	{
	//		//Engine::Get().Clear();
	//		/*aStar.DisplayGridWithPath(map, path);*/
	//		for (const Node* node : path)
	//		{
	//			// 경로는 '2'로 표시.
	//			//map[node->position.y][node->position.x] = '2';
	//			//pathChar.push_back(node->position);]
	//			//for (auto& ac : actors)
	//			//{
	//			//	if (ac->Position().x == node->position.x && ac->Position().y == node->position.y)
	//			//	{
	//			//		SafeDelete(ac);
	//			//		map[node->position.y][node->position.x] = '+';
	//			//	}
	//			//}

	//			//Sleep(3000);
	//		}

	//		/*
	//		//for (int y = 0; y < map.size(); ++y)
	//		//{
	//		//	for (int x = 0; x < map[0].size(); ++x)
	//		//	{
	//		//		// 장애물.
	//		//		if (map[y][x] == '1')
	//		//		{
	//		//			std::cout << "1 ";
	//		//			//grid[y][x] = '1';
	//		//		}

	//		//		// 경로.
	//		//		else if (map[y][x] == '2')
	//		//		{
	//		//			std::cout << "* ";
	//		//			//grid[y][x] = '*';
	//		//		}

	//		//		// 빈 공간.
	//		//		else if (map[y][x] == '0')
	//		//		{
	//		//			std::cout << "0 ";
	//		//			//grid[y][x] = '0';
	//		//		}
	//		//	}

	//		//	std::cout << "\n";
	//		//}
	//		*/

	//	}
	//}
}