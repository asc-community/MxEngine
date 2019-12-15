#pragma once

#include "GLUtils.h"
#include "VertexArray.h"
#include "Shader.h"
#include "IndexBuffer.h"

class Renderer
{
	bool depthBufferEnabled = false;
	GLbitfield clearMask = GL_COLOR_BUFFER_BIT;
public:
	Renderer();

	void DrawTriangles(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const;
	void DrawLines(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const;
	void DrawTrianglesInstanced(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader, size_t count) const;
	void Clear() const;
	void Flush() const;
	Renderer& UseSampling(bool value = true);
	Renderer& UseDepthBuffer(bool value = true);
	Renderer& UseCulling(bool value = true, bool counterClockWise = true, bool cullBack = true);
	Renderer& UseClearColor(float r, float g, float b, float a = 0.0f);
};