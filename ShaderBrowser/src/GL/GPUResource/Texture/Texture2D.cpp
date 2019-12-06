#include "Texture2D.h"
#include "Common/System/Cache/TextureCache.h"

namespace customGL
{

	Texture2D* Texture2D::create(std::string fileName)
	{
		Texture2D* texture = new Texture2D();
        if (texture->initWithFile(fileName))
        {
			texture->createGPUResource();
            return texture;
        }

		return nullptr;
	}

	Texture2D* Texture2D::create(Image* image)
	{
		Texture2D* texture = new Texture2D();
		if (texture->initWithImage(image))
		{
			texture->createGPUResource();
			return texture;
		}

		return nullptr;
	}

	Texture2D::Texture2D()
		: m_uTextureId(-1)
		, m_oImage(nullptr)
        , m_eWrapTypeS(GL_CLAMP_TO_EDGE)
        , m_eWrapTypeT(GL_CLAMP_TO_EDGE)
        , m_eFilterMin(GL_LINEAR_MIPMAP_LINEAR)
//        , m_eFilterMin(GL_LINEAR)
        , m_eFilterMag(GL_LINEAR)
	{

	}

	Texture2D::~Texture2D()
	{
		BROWSER_LOG("~Texture2D");

        // 从显存中移除
		if (m_uTextureId)
		{
			glDeleteTextures(1, &m_uTextureId);
			deleteGPUResource();
        }
        // 从cache中移除
        TextureCache::getInstance()->removeFromCache(this);
    }

	void Texture2D::createGPUResource()
	{
		BROWSER_LOG("createGPUResource");
	}

	void Texture2D::updateGPUResource()
	{
		BROWSER_LOG("updateGPUResource");
	}

	void Texture2D::deleteGPUResource()
	{
		BROWSER_LOG("deleteGPUResource");
	}

	bool Texture2D::initWithImage(Image* image)
	{
		m_oImage = image;

		if (!m_oImage)
		{
			BROWSER_ASSERT(false, "Texture init fail in function Texture2D::initWithFile");
			return false;
		}

		// 1.创建纹理
		glGenTextures(1, &m_uTextureId);
		// 2.绑定纹理
		glBindTexture(GL_TEXTURE_2D, m_uTextureId);
		// 3.设置纹理环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_eFilterMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_eFilterMag);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_eWrapTypeS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_eWrapTypeT);
		// 4.设置纹理数据
		/*
		第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
		第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
		第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
		第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
		下个参数应该总是被设为0（历史遗留的问题）。
		第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
		最后一个参数是真正的图像数据。
		*/
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_oImage->getWidth(), m_oImage->getHeight(), 0, m_oImage->getType(), GL_UNSIGNED_BYTE, m_oImage->getData());

		// 为当前绑定的纹理自动生成所有需要的多级渐远纹理
		glGenerateMipmap(GL_TEXTURE_2D);

		// 释放image数据
		m_oImage->deleteImage();

		return true;
	}

	bool Texture2D::initWithFile(std::string fileName)
	{
		// 纹理数据 TODO: 从cache中获得
		m_oImage = Image::create(fileName.c_str());
		return initWithImage(m_oImage);
	}
    
    void Texture2D::setTexParameters(GLenum wrapS, GLenum wrapT, GLenum filterMin, GLenum filterMag)
    {
        m_eWrapTypeS = wrapS;
        m_eWrapTypeT = wrapT;
        m_eFilterMin = filterMin;
        m_eFilterMag = filterMag;
        
        glBindTexture(GL_TEXTURE_2D, m_uTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_eFilterMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_eFilterMag);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_eWrapTypeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_eWrapTypeT);
    }
    
    void Texture2D::setTexWrapParams(GLenum wrapS, GLenum wrapT)
    {
        setTexParameters(wrapS, wrapT, m_eFilterMin, m_eFilterMag);
    }
    
    void Texture2D::setTexFilterParams(GLenum filterMin, GLenum filterMag)
    {
        setTexParameters(m_eWrapTypeS, m_eWrapTypeT, filterMin, filterMag);
    }

}
