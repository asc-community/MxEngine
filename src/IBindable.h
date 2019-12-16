#pragma once

namespace MomoEngine
{
	struct IBindable
	{
	protected:
		unsigned int id = 0;
		virtual ~IBindable() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
	};
}