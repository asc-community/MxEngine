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

#include "ImageConverter.h"
#include "Utilities/Profiler/Profiler.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Vendors/stb/stb_image_write.h"

#include <algorithm>

namespace MxEngine
{
    void CopyImageData(void* context, void* data, int size)
    {
        auto* storage = (ImageConverter::RawImageData*)(context);
        auto membegin = (uint8_t*)data;
        auto memend = membegin + size;
        storage->insert(storage->end(), membegin, memend);
    }

    ImageConverter::RawImageData ImageConverter::ConvertImagePNG(const uint8_t* imagedata, int width, int height, int channels, bool flipOnConvert)
    {
        ImageConverter::RawImageData data;

        MAKE_SCOPE_PROFILER("ImageWriter::ConvertImagePNG");
        MAKE_SCOPE_TIMER("MxEngine::ImageWriter", "ImageWriter::ConvertImagePNG()");

        stbi_flip_vertically_on_write(flipOnConvert);
        stbi_write_png_to_func(CopyImageData, (void*)&data, width, height, channels, (const void*)imagedata, width * channels);
        return data;
    }

    ImageConverter::RawImageData ImageConverter::ConvertImageBMP(const uint8_t* imagedata, int width, int height, int channels, bool flipOnConvert)
    {
        ImageConverter::RawImageData data;

        MAKE_SCOPE_PROFILER("ImageWriter::ConvertImageBMP");
        MAKE_SCOPE_TIMER("MxEngine::ImageWriter", "ImageWriter::ConvertImageBMP()");

        stbi_flip_vertically_on_write(flipOnConvert);
        stbi_write_bmp_to_func(CopyImageData, (void*)&data, width, height, channels, (const void*)imagedata);
        return data;
    }

    ImageConverter::RawImageData ImageConverter::ConvertImageTGA(const uint8_t* imagedata, int width, int height, int channels, bool flipOnConvert)
    {
        ImageConverter::RawImageData data;

        MAKE_SCOPE_PROFILER("ImageWriter::ConvertImageTGA");
        MAKE_SCOPE_TIMER("MxEngine::ImageWriter", "ImageWriter::ConvertImageTGA()");

        stbi_flip_vertically_on_write(flipOnConvert);
        stbi_write_tga_to_func(CopyImageData, (void*)&data, width, height, channels, (const void*)imagedata);
        return data;
    }

    ImageConverter::RawImageData ImageConverter::ConvertImageJPG(const uint8_t* imagedata, int width, int height, int channels, int quality, bool flipOnConvert)
    {
        ImageConverter::RawImageData data;

        MAKE_SCOPE_PROFILER("ImageWriter::ConvertImageJPG");
        MAKE_SCOPE_TIMER("MxEngine::ImageWriter", "ImageWriter::ConvertImageJPG()");

        stbi_flip_vertically_on_write(flipOnConvert);
        stbi_write_jpg_to_func(CopyImageData, (void*)&data, width, height, channels, (const void*)imagedata, quality);
        return data;
    }

    ImageConverter::RawImageData ImageConverter::ConvertImageHDR(const uint8_t* imagedata, int width, int height, int channels, bool flipOnConvert)
    {
        ImageConverter::RawImageData data;

        MAKE_SCOPE_PROFILER("ImageWriter::ConvertImageHDR");
        MAKE_SCOPE_TIMER("MxEngine::ImageWriter", "ImageWriter::ConvertImageHDR()");

        stbi_flip_vertically_on_write(flipOnConvert);
        stbi_write_hdr_to_func(CopyImageData, (void*)&data, width, height, channels, (const float*)imagedata);
        return data;
    }
}