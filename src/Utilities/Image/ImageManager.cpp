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
#include "Utilities/Logger/Logger.h"

namespace MxEngine
{
    void ImageManager::SaveTexture(StringId fileHash, const GResource<Texture>& texture, ImageType type)
    {
        ImageManager::SaveTexture(FileManager::GetFilePath(fileHash), texture, type);
    }

    void ImageManager::SaveTexture(const FilePath& filePath, const GResource<Texture>& texture, ImageType type)
    {
        ImageManager::SaveTexture(ToMxString(filePath), texture, type);
    }

    void ImageManager::SaveTexture(const MxString& filePath, const GResource<Texture>& texture, ImageType type)
    {
        File file(filePath, File::WRITE | File::BINARY);
        ImageConverter::RawImageData imageByteData;
        auto imageData = texture->GetRawTextureData();
        switch (type)
        {
        case ImageType::PNG:
            imageByteData = ImageConverter::ConvertImagePNG(imageData.data(), (int)texture->GetWidth(), (int)texture->GetHeight());
            break;                                                                                                               
        case ImageType::BMP:                                                                                                     
            imageByteData = ImageConverter::ConvertImageBMP(imageData.data(), (int)texture->GetWidth(), (int)texture->GetHeight());
            break;                                                                                                               
        case ImageType::TGA:                                                                                                     
            imageByteData = ImageConverter::ConvertImageTGA(imageData.data(), (int)texture->GetWidth(), (int)texture->GetHeight());
            break;                                                                                                               
        case ImageType::JPG:                                                                                                     
            imageByteData = ImageConverter::ConvertImageJPG(imageData.data(), (int)texture->GetWidth(), (int)texture->GetHeight());
            break;                                                                                                               
        case ImageType::HDR:                                                                                                     
            imageByteData = ImageConverter::ConvertImageHDR(imageData.data(), (int)texture->GetWidth(), (int)texture->GetHeight());
            break;
        }

        file.WriteBytes(imageByteData.data(), imageByteData.size());
    }

    void ImageManager::SaveTexture(const char* filePath, const GResource<Texture>& texture, ImageType type)
    {
        ImageManager::SaveTexture((MxString)filePath, texture, type);
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
            Logger::Instance().Warning("MxEngine::ImageManager", "cannot take screenshot at there is no viewport attached");
            return;
        }
        ImageManager::SaveTexture(filePath, viewport->GetRenderTexture(), type);
    }

    void ImageManager::TakeScreenShot(const char* filePath, ImageType type)
    {
        ImageManager::TakeScreenShot((MxString)filePath, type);
    }
}