#ifndef GL_IMAGE_H
#define GL_IMAGE_H

#include <glad/glad.h>
#include <string>
#include "Common/Tools/Utils.h"

namespace customGL
{
	class Image
	{
	public:
		static Image* create(const char* fileName);
	public:
		Image();
		~Image();

		// 释放Image
		void deleteImage();
	

		REGISTER_PROPERTY_GET(int, m_iWidth, Width)
		REGISTER_PROPERTY_GET(int, m_iHeight, Height)
		REGISTER_PROPERTY_GET(int, m_iChannels, Channels)
		REGISTER_PROPERTY_GET(unsigned char*, m_Datas, Data)
		REGISTER_PROPERTY_GET(GLenum, m_eType, Type)
	private:
		bool initWithFile(const char* fileName);

	private:
		// 图片宽高以及通道
		int m_iWidth;
		int m_iHeight;
		int m_iChannels;
		// 数据
		unsigned char* m_Datas;
		// 图片通道类型
		GLenum m_eType;
        
        std::string name;
	};
}

#endif
