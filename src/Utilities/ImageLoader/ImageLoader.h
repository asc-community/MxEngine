#pragma once

#include <string>

namespace MxEngine
{
	struct Image
	{
		int width;
		int height;
		int channels;
		unsigned char* data;
	};

	class ImageLoader
	{
	public:
		static Image LoadImage(const std::string& filepath, bool flipImage = true);
		static void FreeImage(Image image);
		static void FreeImage(unsigned char* imageData);
	};
}