#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "Utilities/Profiler/Profiler.h"

namespace MxEngine
{
	Image ImageLoader::LoadImage(const std::string& filepath, bool flipImage)
	{
        MAKE_SCOPE_PROFILER("ImageLoader::LoadImage");
        MAKE_SCOPE_TIMER("MxEngine::ImageLoader", "ImageLoader::LoadImage()");
        Logger::Instance().Debug("ImageLoader::LoadImage", "loading image from file: " + filepath);

		stbi_set_flip_vertically_on_load(flipImage);
		Image image;
		image.data = stbi_load(filepath.c_str(), &image.width, &image.height, &image.channels, STBI_rgb);
		return image;
	}

	void ImageLoader::FreeImage(Image image)
	{
		FreeImage(image.data);
	}

	void ImageLoader::FreeImage(unsigned char* imageData)
	{
		stbi_image_free(imageData);
	}
}