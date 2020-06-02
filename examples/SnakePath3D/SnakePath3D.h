#include <MxEngine.h>
#include <Library/Bindings/Bindings.h>
#include <Library/Primitives/Primitives.h>

using namespace MxEngine;

using namespace std;

class Cell : public Cube
{
public:
	const VectorInt2 Coord;

	enum class STATE
	{
		VOID,
		EMPTY,
		MARKED,
		LAST,
	};
	STATE state = STATE::MARKED;

	STATE GetState() const
	{
		return state;
	}

	void SetState(STATE state)
	{
		// add scale x scale y scale z
		// add absolute
		// texture storage
		float height = 3.0f;
		this->state = state;
		auto scale = this->ObjectTransform.GetScale();
		switch (state)
		{
		case Cell::STATE::VOID:
			this->Scale(1.0f, height / scale.y, 1.0f);
			this->ObjectTexture = Colors::MakeTexture(Colors::WHITE);
			break;
		case Cell::STATE::EMPTY:
			this->Scale(1.0f, 0.1f / scale.y, 1.0f);
			this->ObjectTexture = Colors::MakeTexture(Colors::GREY);
			break;
		case Cell::STATE::MARKED:
			this->Scale(1.0f, 0.1f / scale.y, 1.0f);
			this->ObjectTexture = Colors::MakeTexture(Colors::RED);
			break;
		case Cell::STATE::LAST:
			this->Scale(1.0f, 0.1f / scale.y, 1.0f);
			this->ObjectTexture = Colors::MakeTexture(Colors::YELLOW);
			break;
		default:
			break;
		}
	}

	Cell(float size, float margin, float x, float y, float altitudeOffset)
		: Coord(x, y)
	{
		this->Translate(-0.5f, -0.9f, -0.5f);
		this->Scale(size - 2 * margin, size / 10, size - 2 * margin);
		this->Translate(x * size, altitudeOffset, y * size);
	}

	static bool CommonSide(VectorInt2 c1, VectorInt2 c2)
	{
		return abs(c1.x - c2.x) + abs(c1.y - c2.y) == 1;
	}
};

class GameController
{
	Array2D<Cell*> cells;
	size_t size;
	float cellSize;
public:
	Vector3 StartPosition{ 0.0f };

	GameController(size_t size, float cellSize, float cellMargin, float cellOffset)
		: size(size), cellSize(cellSize)
	{
		auto ctx = Application::Get();
		cells.resize(size, size);
		for (size_t x = 0; x < size; x++)
		{
			for (size_t y = 0; y < size; y++)
			{
				cells[x][y] = (Cell*)&ctx->GetCurrentScene().AddObject(MxFormat("Cube({0}, {1})", x, y),
					MakeUnique<Cell>(cellSize, cellMargin, (float)x, (float)y, cellOffset));

			}
		}
	}

	size_t GetSize() const { return size; }

	bool InBounds(int x, int y)
	{
		return (x >= 0) && (y >= 0) && (x < size) && (y < size);
	}

	void Reset()
	{
		for (size_t x = 0; x < size; x++)
		{
			for (size_t y = 0; y < size; y++)
			{
				cells[x][y]->SetState(Cell::STATE::VOID);
			}
		}
	}

	size_t GenerateLevel(size_t stepCount)
	{
		static VectorInt2 dirs[]
		{
			{0, -1},
			{-1, 0},
			{0, 1},
			{1, 0}
		};
		size_t cellCount = 1;
		VectorInt2 currPos(Random::Get((int)size / 4, (int)size * 3 / 4), Random::Get((int)size / 4, (int)size * 3 / 4));
		
		StartPosition = Vector3(currPos.x, 1.5f, currPos.y);
		GetCollidedCell(StartPosition)->SetState(Cell::STATE::EMPTY);
		cellCount = 1;
		int i;
		if (StartPosition.x < size / 3)
			i = 2;
		else if (StartPosition.z < size / 3)
			i = 1;
		else if (StartPosition.z > 2 * size / 3)
			i = 0;
		else // choose any state for `i` anyway
			i = 3;

		for (; i < stepCount; i += Random::Get(1, 2))
		{
			int dir = i % 4;
			auto length = Random::Get(0, 3);
			for (int j = 0; j < length; j++)
			{
				auto cur = currPos + dirs[dir];
				if (InBounds(cur.x, cur.y) && GetCollidedCell({ cur.x, 0, cur.y })->state == Cell::STATE::VOID)
				{
					GetCollidedCell({ cur.x, 0, cur.y })->SetState(Cell::STATE::EMPTY);
					currPos = cur;
					cellCount++;
				}
				else break;
			}
		}
		return cellCount;
	}

	Cell* GetCollidedCell(const Vector3& pos)
	{
		VectorInt3 cellPos = glm::round(pos / cellSize);
		if (InBounds(cellPos.x, cellPos.z))
		{
			return cells[cellPos.x][cellPos.z];
		}
		else
			return nullptr;
	}
};

#include <stack>

class SnakePath3D : public Application
{
	UniqueRef<GameController> control;
	stack<Cell*> trace;
	int cellCount;
public:
	virtual void OnCreate () override;
	virtual void OnUpdate () override;
	virtual void OnDestroy() override;

	void GenerateLevel();

	SnakePath3D();
};

int main()
{
	SnakePath3D app;
	app.Run();
	return 0;
}