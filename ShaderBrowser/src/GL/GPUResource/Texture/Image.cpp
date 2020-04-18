#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace common;

namespace customGL
{
    ImageData::ImageData(unsigned char* data)
        : m_Datas(data)
    {
        this->autorelease();
    }
    
    ImageData::~ImageData()
    {
        stbi_image_free(m_Datas);
        m_Datas = nullptr;
    }
    
    
	Image* Image::create(const std::string& fileName, shared_ptr<const char*> extra /*= nullptr*/)
	{
		Image* image = new Image();
		if (!image->initWithFile(fileName.c_str()))
		{
			delete image;
			return nullptr;
		}
        image->name = fileName;
		return image;
	}

	Image::Image()
		: m_pData(nullptr)
		, m_iWidth(0)
		, m_iHeight(0)
		, m_iChannels(0)
		, m_eType(GL_RGBA)
	{

	}
    
    Image::Image(const Image& image)
        : m_pData(nullptr)
        , m_iWidth(image.m_iWidth)
        , m_iHeight(image.m_iHeight)
        , m_iChannels(image.m_iChannels)
        , m_eType(image.m_eType)
        , name(image.name)
    {
        m_pData = image.m_pData;
        m_pData->retain();
    }

	Image::~Image()
	{
        m_pData->release();
	}

	bool Image::initWithFile(const char* fileName)
	{
        stbi_set_flip_vertically_on_load(true);    // 翻转图片y轴
		unsigned char* data = stbi_load(fileName, &m_iWidth, &m_iHeight, &m_iChannels, 0);
        m_pData = new ImageData(data);
        m_pData->retain();
        
		switch (m_iChannels)
		{
		case 3:
			m_eType = GL_RGB;
			break;
		case 4:
			m_eType = GL_RGBA;
			break;
		}
		return true;
	}
    
}
