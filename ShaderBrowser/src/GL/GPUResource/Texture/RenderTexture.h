#pragma once

#include <unordered_map>
#include <glad/glad.h>
#include "Common/Components/Reference.h"
#include "Common/Tools/Utils.h"
#include "GL/GPUResource/BaseGPUResource.h"

using namespace common;

namespace customGL
{
    class GPUOperateRenderTextureCommand;

	// 渲染纹理附件类型
	enum RenderTextureAttachmentType
	{
		// 纹理附件 tbo: texture buffer object
		RTAttachmentType_TBO = 0,
		// 渲染缓冲附件 rbo: render buffer objct
		RTAttachmentType_RBO,
	};

	// 渲染纹理附件
	class RenderTextureAttachment
	{
	public:
		RenderTextureAttachment()
			: attachment(GL_COLOR_ATTACHMENT0)
			, type(RenderTextureAttachmentType::RTAttachmentType_TBO)
			, buffer_object(0)
			, property1(GL_RGBA)
			, property2(GL_LINEAR)
			, property3(GL_CLAMP_TO_BORDER)
		{
		}
		RenderTextureAttachment(GLuint att, RenderTextureAttachmentType t = RenderTextureAttachmentType::RTAttachmentType_TBO, GLuint p1 = GL_RGBA, GLuint p2 = GL_LINEAR, GLuint p3 = GL_CLAMP_TO_BORDER)
			: attachment(att)
			, type(t)
			, buffer_object(0)
			, property1(p1)
			, property2(p2)
			, property3(p3)
		{
		}

		// 如果显式声明了移动构造函数或移动赋值运算符，则：1.不自动生成复制构造函数。2.不自动生成复制赋值运算符。
		// 注意：如果显式声明了复制构造函数或析构函数，则弃用复制赋值运算符的自动生成。
		// 注意：如果显式声明了复制赋值运算符或析构函数，则弃用复制构造函数的自动生成。
		RenderTextureAttachment(const RenderTextureAttachment& rt_attachment)
		{
			attachment = rt_attachment.attachment;
			type = rt_attachment.type;
			buffer_object = rt_attachment.buffer_object;
			property1 = rt_attachment.property1;
			property2 = rt_attachment.property2;
			property3 = rt_attachment.property3;
		}
		RenderTextureAttachment& operator=(const RenderTextureAttachment& rt_attachment)
		{
			attachment = rt_attachment.attachment;
			type = rt_attachment.type;
			buffer_object = rt_attachment.buffer_object;
			property1 = rt_attachment.property1;
			property2 = rt_attachment.property2;
			property3 = rt_attachment.property3;
			return *this;
		};

		// 如果显式声明了复制构造函数、复制赋值运算符、移动构造函数、移动赋值运算符或析构函数，则：1.不自动生成移动构造函数。2.不自动生成移动赋值运算符。
		RenderTextureAttachment(RenderTextureAttachment&& rt_attachment) noexcept
		{
			attachment = rt_attachment.attachment;
			type = rt_attachment.type;
			buffer_object = rt_attachment.buffer_object;
			property1 = rt_attachment.property1;
			property2 = rt_attachment.property2;
			property3 = rt_attachment.property3;
		}
		// 注意：如果显式声明了复制赋值运算符或析构函数，则弃用复制构造函数的自动生成。
		RenderTextureAttachment& operator=(const RenderTextureAttachment&& rt_attachment)
		{
			attachment = rt_attachment.attachment;
			type = rt_attachment.type;
			buffer_object = rt_attachment.buffer_object;
			property1 = rt_attachment.property1;
			property2 = rt_attachment.property2;
			property3 = rt_attachment.property3;
			return *this;
		};


	public:
		// 附件附加类型 (绑定点)
		GLuint attachment;
		// 附件类型
		RenderTextureAttachmentType type;
		// 缓冲对象 （0表示对象未创建）
		GLuint buffer_object = 0;
		// 缓冲对象属性
		GLuint property1;	// tbo: GL_RGBA;	rbo: GL_DEPTH24_STENCIL8
		GLuint property2;	// tbo: GL_RGB/GL_DEPTH_STENCIL
		GLuint property3;	// tbo: GL_UNSIGNED_BYTE/GL_UNSIGNED_INT_24_8
		GLuint property4;	// tbo: filter
		GLuint property5;	// tbo: wrap
	};
    
	// 渲染纹理
	class RenderTexture : public BaseGPUResource
	{
	public:
		static RenderTexture* create(unsigned int width, unsigned int height);
		static RenderTexture* create(unsigned int width, unsigned int height, const std::unordered_map<GLuint, RenderTextureAttachment>& attachments);

	public:
		RenderTexture(unsigned int width, unsigned int height, const std::unordered_map<GLuint, RenderTextureAttachment>& attachments);
		RenderTexture(unsigned int width, unsigned int height, bool autoCreateGPURes = true);
		~RenderTexture();
		friend class GPUOperateRenderTextureCommand;

    public:
		// 使用当前渲染纹理 (渲染线程使用)
		void useRenderTexture();

		REGISTER_PROPERTY_GET(unsigned int, m_uFrameBufferId, FrameBufferId);
		REGISTER_PROPERTY_GET(unsigned int, m_uWidth, Width);
		REGISTER_PROPERTY_GET(unsigned int, m_uHeight, Height);
		GLuint getAttachmentTextureBufferObject(GLuint attachment)
		{
			auto itor = m_mAttachments.find(attachment);
			if (itor != m_mAttachments.end())
			{
				return itor->second.buffer_object;
			}
			return 0;
		}
		void setAttachment(GLuint attachment, const RenderTextureAttachment& data);
		

	protected:
		// 创建gpu资源
		virtual void createGPUResource();
		// 更新gpu资源
		virtual void updateGPUResource();
		// 删除gpu资源
		virtual void deleteGPUResource();
        // 更新gpu资源属性
//        void updateGPUResourceProperties();
       

	private:
		// 宽度
		unsigned int m_uWidth;
		// 高度
		unsigned int m_uHeight;
		// 帧缓冲
		GLuint m_uFrameBufferId;
		// 帧缓冲附件
		std::unordered_map<GLuint, RenderTextureAttachment> m_mAttachments;
	};
}

