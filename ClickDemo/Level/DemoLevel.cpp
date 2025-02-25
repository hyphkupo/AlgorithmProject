#include "DemoLevel.h"

#include "Actor/DrawableActor.h"
#include "Actor/Player.h"
#include "Actor/Start.h"
#include "Actor/Wall.h"
#include "Actor/Ground.h"
#include "Actor/Path.h"

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

			// 맵 문자가 1이면 그라운드 액터 생성
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
	//map =
	//{
	//	{ '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1' },
	//	{ '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '1', '1' },
	//	{ '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', 'e', '1' },
	//	{ '1', '0', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '0', '0', '1' },
	//	{ '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1' },
	//	{ '1', '0', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1' },
	//	{ '1', '0', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1' },
	//	{ '1', '0', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1' },
	//	{ '1', 's', '0', '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', '1' },
	//	{ '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1', '1' },
	//};

	if (ParseMap("./Assets/Map3.txt"))
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

	Path* pa = nullptr;

	if (Engine::Get().GetKeyDown(VK_SPACE))
	{
		for (Actor* a : actors)
		{
			Path* pt = a->As<Path>();
			if (pt)
			{
				a->Destroy();
				//SafeDelete(a);
			}
		}

		startNode->position = s->Position();
		goalNode->position = p->Position();
		
		AStar aStar;

		std::vector<Node*> path = aStar.FindPath(startNode, goalNode, map);

		std::vector<Vector2> pathNode;

		if (path.empty())
		{
			isEmpty = true;
		}

		// 경로가 있으면
		if (!path.empty())
		{
			for (Actor* a : actors)
			{
				for (Node* node : path)
				{
					if (node->position.x == startNode->position.x && node->position.y == startNode->position.y)
					{
						continue;
					}

					else if (node->position.x == goalNode->position.x && node->position.y == goalNode->position.y)
					{
						continue;
					}

					else if (node->position.x == a->Position().x && node->position.y && a->Position().y)
					{
						pathNode.push_back(Vector2(node->position.x, node->position.y));
					}
				}
			}
		}
		
		for (auto& ix : pathNode)
		{
			pa = new Path(ix);
			actors.push_back(pa);
		}

		pathNode.clear();
	}
}

void DemoLevel::Draw()
{
	Super::Draw();

	//if (Engine::Get().GetKeyDown(VK_SPACE))
	//{
	//	if (isEmpty)
	//	{
	//		Engine::Get().Draw(Vector2(Engine::Get().ScreenSize().x - 26, Engine::Get().ScreenSize().y - 14), "경로를 찾을 수 없음");
	//		isEmpty = false;
	//		//Sleep(3000);
	//	}
	//}
}