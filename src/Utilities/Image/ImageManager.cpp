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

#include "ImageManager.h"
#include "Utilities/FileSystem/FileManager.h"
#include "Utilities/Image/ImageConverter.h"
#include "Core/Application/RenderManager.h"
#include "Utilities/Logging/Logger.h"

namespace MxEngine
{
    void ImageManager::SaveTexture(StringId fileHash, const TextureHandle& texture, ImageType type)
    {
        ImageManager::SaveTexture(FileManager::GetFilePath(fileHash), texture, type);
    }

    void ImageManager::SaveTexture(const FilePath& filePath, const TextureHandle& texture, ImageType type)
    {
        ImageManager::SaveTexture(ToMxString(filePath), texture, type);
    }

    void ImageManager::SaveTexture(const MxString& filePath, const TextureHandle& texture, ImageType type)
    {
        File file(filePath, File::WRITE | File::BINARY);
        ImageConverter::RawImageData imageByteData;
        auto imageData = texture->GetRawTextureData();
        switch (type)
        {
        case ImageType::PNG:
            imageByteData = ImageConverter::ConvertImagePNG(imageData);
            break;
        case ImageType::BMP:
            imageByteData = ImageConverter::ConvertImageBMP(imageData);
            break;
        case ImageType::TGA:
            imageByteData = ImageConverter::ConvertImageTGA(imageData);
            break;
        case ImageType::JPG:
            imageByteData = ImageConverter::ConvertImageJPG(imageData);
            break;
        case ImageType::HDR:
            MXLOG_WARNING("MxEngine::ImageManager", "HDR texture format is not supported through ImageManager, use ImageConverter::ConvertImageHDR");
            break;
        }
        file.WriteBytes(imageByteData.data(), imageByteData.size());
    }

    void ImageManager::SaveTexture(const char* filePath, const TextureHandle& texture, ImageType type)
    {
        ImageManager::SaveTexture((MxString)filePath, texture, type);
    }

    void ImageManager::SaveTexture(StringId fileHash, const TextureHandle& texture)
    {
        ImageManager::SaveTexture(FileManager::GetFilePath(fileHash), texture);
    }

    void ImageManager::SaveTexture(const FilePath& filepath, const TextureHandle& texture)
    {
        auto ext = filepath.extension();
        if (ext == ".png")
        {
            ImageManager::SaveTexture(filepath, texture, ImageType::PNG);
        }
        else if (ext == ".jpg" || ext == ".jpeg")
        {
            ImageManager::SaveTexture(filepath, texture, ImageType::JPG);
        }
        else if (ext == ".bmp")
        {
            ImageManager::SaveTexture(filepath, texture, ImageType::BMP);
        }
        else if (ext == ".tga")
        {
            ImageManager::SaveTexture(filepath, texture, ImageType::TGA);
        }
        else if (ext == ".hdr")
        {
            ImageManager::SaveTexture(filepath, texture, ImageType::HDR);
        }
        else
        {
            MXLOG_WARNING("MxEngine::ImageManager", "image was not saved because extenstion was invalid: " + MxString(ext.string().c_str()));
        }
    }

    void ImageManager::SaveTexture(const MxString& filePath, const TextureHandle& texture)
    {
        ImageManager::SaveTexture(filePath.c_str(), texture);
    }

    void ImageManager::SaveTexture(const char* filePath, const TextureHandle& texture)
    {
        ImageManager::SaveTexture(MxString(filePath), texture);
    }

    void ImageManager::TakeScreenShot(StringId fileHash, ImageType type)
    {
        ImageManager::TakeScreenShot(FileManager::GetFilePath(fileHash), type);
    }

    void ImageManager::TakeScreenShot(const FilePath& filePath, ImageType type)
    {
        ImageManager::TakeScreenShot(ToMxString(filePath), type);
    }

    void ImageManager::TakeScreenShot(const MxString& filePath, ImageType type)
    {
        auto viewport = RenderManager::GetViewport();
        if (!viewport.IsValid())
        {
            MXLOG_WARNING("MxEngine::ImageManager", "cannot take screenshot at there is no viewport attached");
            return;
        }
        ImageManager::SaveTexture(filePath, viewport->GetRenderTexture(), type);
    }

    void ImageManager::TakeScreenShot(const char* filePath, ImageType type)
    {
        ImageManager::TakeScreenShot((MxString)filePath, type);
    }

    void ImageManager::TakeScreenShot(StringId fileHash)
    {
        ImageManager::TakeScreenShot(FileManager::GetFilePath(fileHash));
    }

    void ImageManager::TakeScreenShot(const FilePath& filePath)
    {
        auto ext = filePath.extension();
        if (ext == ".png")
        {
            ImageManager::TakeScreenShot(filePath, ImageType::PNG);
        }
        else if (ext == ".jpg" || ext == ".jpeg")
        {
            ImageManager::TakeScreenShot(filePath, ImageType::JPG);
        }
        else if (ext == ".bmp")
        {
            ImageManager::TakeScreenShot(filePath, ImageType::BMP);
        }
        else if (ext == ".tga")
        {
            ImageManager::TakeScreenShot(filePath, ImageType::TGA);
        }
        else if (ext == ".hdr")
        {
            ImageManager::TakeScreenShot(filePath, ImageType::HDR);
        }
        else
        {
            MXLOG_WARNING("MxEngine::ImageManager", "screenshots was not saved because extenstion was invalid: " + MxString(ext.string().c_str()));
        }
    }

    void ImageManager::TakeScreenShot(const MxString& filePath)
    {
        ImageManager::TakeScreenShot(filePath.c_str());
    }

    void ImageManager::TakeScreenShot(const char* filePath)
    {
        ImageManager::TakeScreenShot(FilePath(filePath));
    }

    Image ImageManager::CombineImages(ArrayView<Image> images, size_t imagesPerRaw)
    {
        #if defined(MXENGINE_DEBUG)
        for (size_t i = 1; i < images.size(); i++)
        {
            MX_ASSERT(images[i - 1].GetWidth()    == images[i].GetWidth());
            MX_ASSERT(images[i - 1].GetHeight()   == images[i].GetHeight());
            MX_ASSERT(images[i - 1].GetChannels() == images[i].GetChannels());
        }
        #endif
        MX_ASSERT(images.size() > 1);

        size_t width = images[0].GetWidth();
        size_t height = images[0].GetHeight();
        size_t channels = images[0].GetChannels();

        auto result = (uint8_t*)std::malloc(width * height * channels * images.size());
        MX_ASSERT(result != nullptr);

        const size_t imagesPerColumn = images.size() / imagesPerRaw;
        const size_t rawWidth = width * channels;
        size_t offset = 0;

        for (size_t t1 = 0; t1 < imagesPerColumn; t1++)
        {
            for (size_t i = 0; i < height; i++)
            {
                for (size_t t2 = 0; t2 < imagesPerRaw; t2++)
                {
                    auto& tex = images[t1 * imagesPerRaw + t2];
                    auto currentRaw = tex.GetRawData() + i * rawWidth;
                    std::copy(currentRaw, currentRaw + rawWidth, result + offset);
                    offset += rawWidth;
                }
            }
        }
        return Image(result, width * imagesPerRaw, height * imagesPerColumn, channels);
    }

    Image ImageManager::CombineImages(Array2D<Image>& images)
    {
        return ImageManager::CombineImages(ArrayView<Image>(images.data(), images.size()), images.width());
    }
}