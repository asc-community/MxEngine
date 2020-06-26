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

#pragma once

#include <array>
#include "Utilities/Array/Array2D.h"
#include "Utilities/STL/MxString.h"

namespace MxEngine
{
	/*!
	Image class is a POD type which contains reference to an existing image
	*/
	struct Image
	{
		int width;
		int height;
		int channels;
		unsigned char* data;
	};

	/*!
	ImageLoader class is used to load images from disk. Also it contains methods to create cubemaps from their scans
	*/
	class ImageLoader
	{
	public:
		/*!
		loads image from disk. As OpenGL treats images differently as expected, all images are flipped automatically
		\param filepath path to an image on disk
		\param flipImage should the image be vertically flipped. As MxEngine uses primarily OpenGL, usually you want to do this
		\returns Image object if image file exists or nullptr data and width = height = channels = 0 if not
		*/
		static Image LoadImage(const MxString& filepath, bool flipImage = true);
		/*!
		destroys image object. Must be called to clear allocated data
		\param image Image to destroy
		*/
		static void FreeImage(Image image);
		/*!
		destroys image object. Must be called to clear allocated data
		\param imageData raw image data of Image to destroy
		*/
		static void FreeImage(unsigned char* imageData);

		using ImageArray = std::array<Array2D<unsigned char>, 6>;
		/*!
		creates cubemap projections from its scan:
		 X
		XXXX
		 X
		\param image image from which cubemap will be created
		\returns 6 2d arrays of raw image data (can be passed as individual images to OpenGL)
		*/
		static ImageArray CreateCubemap(const Image& image);
	};
}