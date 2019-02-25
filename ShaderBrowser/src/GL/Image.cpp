#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace customGL
{

	Image* Image::create(const char* fileName)
	{
		Image* image = new Image();
		if (!image->initWithFile(fileName))
		{
			delete image;
			return nullptr;
		}
        image->name = fileName;
		return image;
	}

	Image::Image()
		: m_Datas(nullptr)
		, m_iWidth(0)
		, m_iHeight(0)
		, m_iChannels(0)
		, m_eType(GL_RGBA)
	{

	}

	Image::~Image()
	{
        if (m_Datas)
        {
            deleteImage();
        }
	}

	bool Image::initWithFile(const char* fileName)
	{
        stbi_set_flip_vertically_on_load(true);    // 翻转图片y轴
		m_Datas = stbi_load(fileName, &m_iWidth, &m_iHeight, &m_iChannels, 0);
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

	void Image::deleteImage()
	{
		stbi_image_free(m_Datas);
        m_Datas = nullptr;
	}

}
