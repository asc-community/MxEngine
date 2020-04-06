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
#include "Utilities/Profiler/Profiler.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Vendors/stb/stb_image.h"

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
}