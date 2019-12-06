#ifndef GL_TEXTURE2D_H
#define GL_TEXTURE2D_H

#include <glad/glad.h>
#include <string>
#include "GL/GPUResource/Texture/Image.h"
#include "Common/Components/Reference.h"
#include "Common/Tools/Utils.h"
#include "GL/GPUResource/BaseGPUResource.h"

using namespace common;

namespace customGL
{
	class Texture2D : public BaseGPUResource
	{
	public:
		static Texture2D* create(std::string fileName);
		static Texture2D* create(Image* image);
	public:
		Texture2D();
		~Texture2D();
        
    public:
        // 设置纹理参数（环绕方式、过滤方式）
        void setTexParameters(GLenum wrapS, GLenum wrapT, GLenum filterMin, GLenum filterMag);
        // 设置纹理参数（环绕方式）
        void setTexWrapParams(GLenum wrapS, GLenum wrapT);
        // 设置纹理参数（过滤方式）
        void setTexFilterParams(GLenum filterMin, GLenum filterMag);
        
        
        
        REGISTER_PROPERTY_GET(unsigned int, m_uTextureId, TextureId);

	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
		virtual void updateGPUResource();
		// 删除gpu资源
		virtual void deleteGPUResource();
        
	private:
		bool initWithImage(Image* image);
		bool initWithFile(std::string fileName);

	private:
		// 纹理（1.创建纹理 2.绑定纹理 3.设置纹理参数 4.纹理数据 5.生成多级渐远纹理）
		unsigned int m_uTextureId;
		// Image
		Image* m_oImage;
        
        // 纹理环绕方式
        /*
             环绕方式                 描述
             GL_REPEAT              对纹理的默认行为。重复纹理图像。
             GL_MIRRORED_REPEAT     和GL_REPEAT一样，但每次重复图片是镜像放置的。
             GL_CLAMP_TO_EDGE       纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
             GL_CLAMP_TO_BORDER     超出的坐标为用户指定的边缘颜色。
         */
        GLenum m_eWrapTypeS;
        GLenum m_eWrapTypeT;
        
        // 纹理过滤方式
        /*
            过滤方式                      描述
            GL_NEAREST（临近过滤）        OpenGL会选择中心点最接近纹理坐标的那个像素
            GL_LINEAR （线性过滤）        它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色
            GL_NEAREST_MIPMAP_NEAREST   使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
            GL_LINEAR_MIPMAP_NEAREST    使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
            GL_NEAREST_MIPMAP_LINEAR    在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
            GL_LINEAR_MIPMAP_LINEAR     在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
         */
        GLenum m_eFilterMin;
        GLenum m_eFilterMag;
	};
}

#endif
