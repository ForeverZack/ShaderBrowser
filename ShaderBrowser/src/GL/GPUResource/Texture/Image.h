#ifndef GL_IMAGE_H
#define GL_IMAGE_H

#include <glad/glad.h>
#include "Common/Tools/Utils.h"
#include "Common/Components/Reference.h"
#include <memory>

namespace customGL
{
    // 注意：ImageData默认规定一旦创建，无法修改它的数据！！！(因为GPUOperateTexture2DCommand的部分操作会复制Image，而stb_image没有提供读取数据(指针)长度的方法(暂时没有需求，后面有需要再做修改))
    class ImageData : public common::Reference
    {
    public:
        ImageData(unsigned char* data);
        ~ImageData();
        
    public:
        unsigned char* m_Datas;
    };
    
	class Image
	{
	public:
		static Image* create(const std::string& fileName);
	public:
		Image();
        Image(const Image& image);  // copy
		~Image();

		// 释放Image
		void deleteImage();
	

		REGISTER_PROPERTY_GET(int, m_iWidth, Width)
		REGISTER_PROPERTY_GET(int, m_iHeight, Height)
		REGISTER_PROPERTY_GET(int, m_iChannels, Channels)
		REGISTER_PROPERTY_GET(GLenum, m_eType, Type)
        unsigned char* getData()
        {
            return m_pData->m_Datas;
        }
        
	private:
		bool initWithFile(const char* fileName);

	private:
		// 图片宽高以及通道
		int m_iWidth;
		int m_iHeight;
		int m_iChannels;
		// 数据
		ImageData* m_pData;
		// 图片通道类型
		GLenum m_eType;
        
        std::string name;
	};
}

#endif
