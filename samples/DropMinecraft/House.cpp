#include <House.h>
#include <MxEngine.h>

using namespace MxEngine;

void BuildRectangle(
	const VectorInt2& blockCount,
	const BlockCtor& ctor,
	const Vector3& offset
)
{
	for (int x = 0; x < blockCount.x; x++)
		for (int z = 0; z < blockCount.y; z++)
			ctor(offset + Vector3(x, 0, z));
}

void BuildBorder(
	const VectorInt2& blockCount,
	const BlockCtor& ctor,
	const Vector3& offset
	)
{
	if (blockCount.x == 0 || blockCount.y == 0)
		return;
	if (blockCount.x == 1 && blockCount.y == 1)
	{ 
		BuildRectangle(VectorInt2(1), ctor, offset);
		return;
	}
	if (blockCount.x == 1)
	{
		BuildRectangle(VectorInt2(1, blockCount.y), ctor, offset);
		return;
	}
	if (blockCount.y == 1)
	{
		BuildRectangle(VectorInt2(blockCount.x, 1), ctor, offset);
		return;
	}
	BuildRectangle(VectorInt2(blockCount.x - 2, 1), ctor, offset + Vector3(1, 0, 0));
	BuildRectangle(VectorInt2(blockCount.x - 2, 1), ctor, offset + Vector3(0 + 1, 0, blockCount.y - 1));
	BuildRectangle(VectorInt2(1, blockCount.y), ctor, offset);
	BuildRectangle(VectorInt2(1, blockCount.y), ctor, offset + Vector3(blockCount.x - 1, 0, 0));
}

void BuildRoof(
	const VectorInt3& blockCount,
	const BlockCtor& ctor,
	const size_t heightOffset
)
{
	for (size_t y = 0; y < blockCount.y; y++)
	{
		auto share = y / (float)blockCount.y;
		auto xCount = (int)round(blockCount.x * (1 - share));
		auto zCount = (int)round(blockCount.z * (1 - share));
		auto margin = Vector3(
			(blockCount.x - xCount) / 2.f,
			heightOffset + y,
			(blockCount.z - zCount) / 2.f
		);
		BuildBorder(VectorInt2(xCount, zCount), ctor, margin);
	}
}

void BuildHouse(const VectorInt3& wallSize, 
				const VectorInt3& roofSize,
				const BlockCtor& ctorOfCoords)
{
	for (size_t y = 0; y < wallSize.y; y++)
		BuildBorder(VectorInt2(wallSize.x, wallSize.z), ctorOfCoords, Vector3(0, y, 0));
	BuildRoof(roofSize, ctorOfCoords, wallSize.y);
}

void BuildPyramid(const size_t size,
	const BlockCtor& ctorOfCoords)
{
	for (int i = 0; i < size; i++)
		BuildRectangle(VectorInt2(size - i), ctorOfCoords, Vector3(i / 2.f, i, i / 2.f));
}