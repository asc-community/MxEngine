// Copyright(c) 2019 - 2020, #Momo
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// 
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
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
#include "Utilities/Math/Math.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace MxEngine
{
	Image ImageLoader::LoadImage(const MxString& filepath, bool flipImage)
	{
		MAKE_SCOPE_PROFILER("ImageLoader::LoadImage");
		MAKE_SCOPE_TIMER("MxEngine::ImageLoader", "ImageLoader::LoadImage()");
		MXLOG_INFO("MxEngine::ImageLoader", "loading image from file: " + filepath);

		stbi_set_flip_vertically_on_load(flipImage);
		int width, height, channels;
		uint8_t* data = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		channels = 4;
		return Image(data, (size_t)width, (size_t)height, (size_t)channels, false);
	}

	Image ImageLoader::LoadImageFromMemory(const uint8_t* memory, size_t byteSize, bool flipImage)
	{
		MAKE_SCOPE_PROFILER("ImageLoader::LoadImage");
		MAKE_SCOPE_TIMER("MxEngine::ImageLoader", "ImageLoader::LoadImage()");
		MXLOG_INFO("MxEngine::ImageLoader", "loading image from memory");

		stbi_set_flip_vertically_on_load(flipImage);
		int width, height, channels;
		uint8_t* data = stbi_load_from_memory(memory, (int)byteSize, &width, &height, &channels, STBI_rgb_alpha);
		channels = 4;
		return Image(data, (size_t)width, (size_t)height, (size_t)channels, false);
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
	ImageLoader::ImageArray ImageLoader::CreateCubemap(const Image& image)
	{
		ImageArray result;
		size_t channels = 4; 
		size_t width = image.GetWidth() / 4; //-V112
		size_t height = image.GetHeight() / 3;
		if (width != height)
		{
			MXLOG_WARNING("MxEngine::ImageLoader", "image size is invalid, it will be reduced to fit skybox cubemap");
			width = height = FloorToPow2(Min(image.GetWidth() / 4, image.GetHeight() / 3)); //-V112
		}
		for (auto& arr : result)
		{
			arr.resize(height, width * channels, 0);
		}

		auto copySide = [&image, &width, &height, &channels](Array2D<unsigned char>& dst, size_t sliceX, size_t sliceY)
		{
			for (size_t i = 0; i < height; i++)
			{
				size_t y = i + sliceY * height;
				size_t x = sliceX * width;
				size_t bytesInRow = width * channels;

				std::memcpy(dst[i].data(), &image.GetRawData()[(y * image.GetWidth() + x) * channels], bytesInRow);
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