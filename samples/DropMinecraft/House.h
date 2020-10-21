#include <MxEngine.h>

using BlockCtor = std::function<void(const MxEngine::Vector3&)>;

void BuildHouse(const MxEngine::VectorInt3& wallSize,
				const MxEngine::VectorInt3& roofSize,
				const BlockCtor& ctorOfCoords);

void BuildPyramid(const size_t size,
	const BlockCtor& ctorOfCoords);