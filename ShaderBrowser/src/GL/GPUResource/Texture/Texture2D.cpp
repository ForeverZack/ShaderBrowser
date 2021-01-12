#include "Texture2D.h"
#include "Common/System/Cache/TextureCache.h"
#include "GL/GPUOperateCommand/GPUOperateCommandPool.h"
#include "GL/GPUOperateCommand/GPUOperateTexture2DCommand.h"
#include "GL/System/GPUOperateSystem.h"

namespace customGL
{

	Texture2D* Texture2D::create(std::string fileName)
	{
		Texture2D* texture = new Texture2D();
        if (texture->initWithFile(fileName))
        {
            return texture;
        }

		return nullptr;
	}

	Texture2D* Texture2D::create(Image* image)
	{
		Texture2D* texture = new Texture2D();
		if (texture->initWithImage(image))
		{
			return texture;
		}

		return nullptr;
	}

	Texture2D::Texture2D()
		: m_uTextureId(0)
		, m_oImage(nullptr)
		, m_bSRGB(false)
        , m_eWrapTypeS(TexParameters_DefaultWrap)
        , m_eWrapTypeT(TexParameters_DefaultWrap)
        //, m_eFilterMin(GL_LINEAR_MIPMAP_LINEAR)
        , m_eFilterMin(TexParameters_DefaultFilter)
        , m_eFilterMag(TexParameters_DefaultFilter)
	{
        m_eResourceType = GPUResourceType::GRT_Texture2D;
//        m_eResouceState = GRS_UnLoad; // default
	}

	Texture2D::~Texture2D()
	{
		BROWSER_LOG("~Texture2D");
        
        if (m_oImage)
        {
            delete m_oImage;
        }
        // 从cache中移除
        TextureCache::getInstance()->removeFromCache(this);
    }

	void Texture2D::createGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState==GRS_DataLoaded, "Texture2D state must be GRS_DataLoaded, then it can be created on gpu");
                       
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTexture2DCommand>(GPUOperateCommandType::GOCT_Texture2D);
        cmd->setTexture(this);
        cmd->setImage(m_oImage);
		cmd->setSRGB(m_bSRGB);
        cmd->setTexParameters(m_eWrapTypeS, m_eWrapTypeT, m_eFilterMin, m_eFilterMag);
        cmd->ready(GPUOperateType::GOT_Create);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void Texture2D::updateGPUResource()
	{
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTexture2DCommand>(GPUOperateCommandType::GOCT_Texture2D);
        cmd->setTexture(this);
        cmd->setImage(m_oImage);
		cmd->setSRGB(m_bSRGB);
		cmd->ready(GPUOperateType::GOT_Update);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}

	void Texture2D::deleteGPUResource()
	{
        BROWSER_ASSERT(m_eResouceState==GRS_Loaded, "Texture2D state must be GRS_Loaded, then it can be destroyed on gpu");
        
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTexture2DCommand>(GPUOperateCommandType::GOCT_Texture2D);
        cmd->setTexture(this);
        cmd->ready(GPUOperateType::GOT_Delete);
        GPUOperateSystem::getInstance()->addCommand(cmd);
	}

    void Texture2D::updateGPUResourceProperties()
    {
        // GPU命令
        auto cmd = GPUOperateCommandPool::getInstance()->popCommand<GPUOperateTexture2DCommand>(GPUOperateCommandType::GOCT_Texture2D);
        cmd->setTexture(this);
        cmd->setTexParameters(m_eWrapTypeS, m_eWrapTypeT, m_eFilterMin, m_eFilterMag);
        cmd->ready(GPUOperateType::GOT_UpdateProperties_1);
        GPUOperateSystem::getInstance()->addCommand(cmd);
    }
    
	bool Texture2D::initWithImage(Image* image)
	{
		m_oImage = image;

		if (!m_oImage)
		{
			BROWSER_ASSERT(false, "Texture init fail in function Texture2D::initWithFile");
			return false;
		}
        
        // 创建GPU资源
        m_eResouceState = GRS_DataLoaded;
        createGPUResource();

		return true;
	}

	bool Texture2D::initWithFile(std::string fileName)
	{
		// 纹理数据 TODO: 从cache中获得
		m_oImage = Image::create(fileName.c_str());
		return initWithImage(m_oImage);
	}

	void Texture2D::setSRGB(bool sRBG)
	{
		if (m_bSRGB == sRBG)
		{
			return;
		}
		m_bSRGB = sRBG;

		updateGPUResource();
	}
    
    void Texture2D::setTexParameters(GLenum wrapS, GLenum wrapT, GLenum filterMin, GLenum filterMag)
    {
        m_eWrapTypeS = wrapS;
        m_eWrapTypeT = wrapT;
        m_eFilterMin = filterMin;
        m_eFilterMag = filterMag;
        
        updateGPUResourceProperties();
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
