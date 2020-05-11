// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistributionand use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditionsand the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditionsand the following disclaimer in the documentation
// and /or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "ImageLoader.h"
#include "Core/Macro/Macro.h"
#include "Utilities/Profiler/Profiler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Vendors/stb/stb_image.h"
#include "Utilities/Math/Math.h"

namespace MxEngine
{
	Image ImageLoader::LoadImage(const std::string& filepath, bool flipImage)
	{
		MAKE_SCOPE_PROFILER("ImageLoader::LoadImage");
		MAKE_SCOPE_TIMER("MxEngine::ImageLoader", "ImageLoader::LoadImage()");
		Logger::Instance().Debug("MxEngine::ImageLoader", "loading image from file: " + filepath);

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

	/*
	    0X00
	    XXXX -> format of input
	    0X00
	*/

	/*
		result[0] = right
		result[1] = left
		result[2] = top
		result[3] = bottom
		result[4] = front
		result[5] = back
	*/
	ImageLoader::ImageArray ImageLoader::CreateFromSingle(const Image& image)
	{
		ImageArray result;
		size_t channels = 3; 
		size_t width = image.width / 4;
		size_t height = width;
		if (image.width / 4 != image.height / 3)
		{
			Logger::Instance().Warning("MxEngine::ImageLoader", "image size is invalid, it will be reduced to fit skybox cubemap");
			width = height = ToNearestPowTwo(std::min(image.width / 4, image.height / 3));
		}
		for (auto& arr : result)
		{
			arr.resize(height, width * channels, 0);
		}

		auto copySide = [&image, &width, &height, &channels](Array2D<unsigned char>& dst, size_t sliceX, size_t sliceY)
		{
			for (size_t i = 0; i < height; i++)
			{
				for (size_t j = 0; j < width; j++)
				{
					size_t y = i + sliceY * height;
					size_t x = j + sliceX * width;
					for (size_t k = 0; k < channels; k++)
					{
						dst[i][j * channels + k] = image.data[(y * image.width + x) * channels + k];
					}
				}
			}
		};

		copySide(result[0], 2, 1);
		copySide(result[1], 0, 1);
		copySide(result[2], 1, 0);
		copySide(result[3], 1, 2);
		copySide(result[4], 1, 1);
		copySide(result[5], 3, 1);
		return result;
	}
}