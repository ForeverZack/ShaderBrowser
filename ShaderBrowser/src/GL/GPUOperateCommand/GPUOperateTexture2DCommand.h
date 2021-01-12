#pragma once

#include "GL/GPUOperateCommand/BaseGPUOperateCommand.h"


namespace customGL
{
    class Texture2D;
    class Image;
    
    // 2D纹理操作命令
	class GPUOperateTexture2DCommand : public BaseGPUOperateCommand
	{
	public:
		GPUOperateTexture2DCommand();
        ~GPUOperateTexture2DCommand();
        
    public:
        // 准备执行 (逻辑线程调用)
        virtual void ready(GPUOperateType operateType);
        // 执行 (渲染线程调用)
        virtual void execute();
        // 结束执行 (渲染线程调用)
        virtual void finish();

    protected:
        // 创建纹理
        void createTexture2D();
        // 更新纹理数据
        void updateTexture2D();
        // 更新纹理属性
        void updateTexture2DProperties();
        // 删除纹理
        void deleteTexture2D();

		// 根据sRGB的设置转换internalformat
		GLenum convertInternalFormatBySRGB(GLenum internalFormat);
        
        REGISTER_PROPERTY_SET(Texture2D*, m_pTexture, Texture)
		REGISTER_PROPERTY_SET(bool, m_bSRGB, SRGB)
		REGISTER_PROPERTY_SET(GLenum, m_eWrapTypeS, WrapTypeS)
        REGISTER_PROPERTY_SET(GLenum, m_eWrapTypeT, WrapTypeT)
        REGISTER_PROPERTY_SET(GLenum, m_eFilterMin, FilterMin)
        REGISTER_PROPERTY_SET(GLenum, m_eFilterMag, FilterMag)
        void setTexParameters(GLenum wrapS, GLenum wrapT, GLenum filterMin, GLenum filterMag);
        void setImage(Image* image);
        
	protected:
        // 纹理操作对象
        Texture2D* m_pTexture;
        
        // 纹理数据来源 (注意，这个数据必须是拷贝过来的！！)
        Image* m_pImage;

		// 是否是sRGB纹理 (Color Space)
		bool m_bSRGB;
        
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

