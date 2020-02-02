#pragma once

namespace MomoEngine
{
	struct IBindable
	{
    protected:
        using IdType = unsigned int;
		IdType id = 0;
		virtual ~IBindable() = default;

    public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
        virtual inline IdType GetNativeHandler() { return this->id; };
	};
}