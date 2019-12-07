#ifndef GL_GLPROGRAM_H
#define GL_GLPROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>
#include "Common/Components/Reference.h"


namespace customGL
{
    // 最大支持的纹理单元数量
    static const int MAX_ACTIVE_TEXTURE = 16;
	// 最大支持骨骼矩阵数量
	static const int MAX_BONES_COUNT = 100;
    
    class Texture2D;
    
    class GLProgram : public common::Reference
	{
	public:
		// 默认GLProgram名称(网格模型着色器)
		static const char* DEFAULT_GLPROGRAM_NAME;
		// 默认带骨骼动画的网格模型着色器
		static const char* DEFAULT_SKELETON_GLPROGRAM_NAME;
        // 默认的line着色器
        static const char* DEFAULT_LINES_GLPROGRAM_NAME;
        
	public:
		// 预定义的顶点着色器属性位置枚举( layout (location = n) )
		enum
		{
			// 0. 顶点数据
			VERTEX_ATTR_POSITION = 0,
			// 1.顶点颜色
			VERTEX_ATTR_COLOR,
			// 2.纹理uv
			VERTEX_ATTR_TEX_COORD,
            // 3.法线
            VERTEX_ATTR_NORMAL,
            // 4.切线
            VERTEX_ATTR_TANGENT,
			// 5.骨骼id（最多4个骨骼影响一个顶点）
			VERTEX_ATTR_BONE_IDS,
			// 6.骨骼权重
			VERTEX_ATTR_BONE_WEIGHTS,


			// 最大值
			VERTEX_ATTR_MAXCOUNT,
		};
        
        // 预定义的uniform变量枚举
		enum
		{
			// 纹理0: 主纹理(漫反射纹理)
			UNIFORM_CGL_TEXUTRE0 = 0,
			// 纹理1
			UNIFORM_CGL_TEXUTRE1,
			// model matrix
			UNIFORM_CGL_MODEL_MATRIX,
			// view matrix
			UNIFORM_CGL_VIEW_MATRIX,
			// projection matrix
			UNIFORM_CGL_PROJECTION_MATRIX,
			// 漫反射颜色
			UNIFORM_CGL_ALBEDO_COLOR,
            // 平行光
            UNIFORM_CGL_DIRECTIONAL_LIGHT,

			// 骨骼矩阵
            UNIFORM_CGL_BONES_MATRIX,
            // 动态合批用的model矩阵
            UNIFORM_CGL_DYNAMIC_BATCH_MODEL_MATRIX,
            
            
            // 预定义Uniform变量个数
            UNIFORM_MAX_COUNT
        };

		// 预定义的顶点着色器属性名称
		static const char* ATTRIBUTE_NAME_POSITION;
		static const char* ATTRIBUTE_NAME_COLOR;
		static const char* ATTRIBUTE_NAME_COORD;
        static const char* ATTRIBUTE_NAME_NORMAL;
        static const char* ATTRIBUTE_NAME_TANGENT;
		static const char* ATTRIBUTE_NAME_BONE_IDS;
		static const char* ATTRIBUTE_NAME_BONE_WEIGHTS;

		// 预定义的uniform变量	（与前面枚举要对应上！！）
		static const char* SHADER_UNIFORMS_ARRAY[UNIFORM_MAX_COUNT];

	public:
		static GLProgram* create(const char* vertSrc, const char* fragSrc);
        static GLProgram* createAndSaveSource(const char* vertSrc, const char* fragSrc);

	public:
		GLProgram();
		~GLProgram();

        // 拷贝glProgram
        GLProgram* clone();
		// 使用着色器程序
		void useProgram();
		// 更新预定义uniform位置
		void updatePreDefinedUniformsLocation();
		// 设置uniform
        void setUniformWithInt(const std::string& uniformName, int value);
        void setUniformWithFloat(const std::string& uniformName, float value);
        void setUniformWithMat3(const std::string& uniformName, const glm::mat3& value);
        void setUniformWithMat3x4(const std::string& uniformName, const glm::mat3x4& value);
        void setUniformWithMat4(const std::string& uniformName, const glm::mat4& value);
        void setUniformWithMat4x3(const std::string& uniformName, const glm::mat4x3& value);
        void setUniformWithFloatV(const std::string& uniformName, int count, const float* fv);
        void setUniformWithIntV(const std::string& uniformName, int count, const int* iv);
        void setUniformWithVec2(const std::string& uniformName, GLfloat f1, GLfloat f2);
        void setUniformWithIVec2(const std::string& uniformName, int i1, int i2);
        void setUniformWithVec2V(const std::string& uniformName, int count, const GLfloat* fv);
        void setUniformWithIVec2V(const std::string& uniformName, int count, const int* iv);
        void setUniformWithVec3(const std::string& uniformName, GLfloat f1, GLfloat f2, GLfloat f3);
        void setUniformWithIVec3(const std::string& uniformName, int i1, int i2, int i3);
        void setUniformWithVec3V(const std::string& uniformName, int count, const GLfloat* fv);
        void setUniformWithIVec3V(const std::string& uniformName, int count, const int* iv);
		void setUniformWithVec4(const std::string& uniformName, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);
        void setUniformWithIVec4(const std::string& uniformName, int i1, int i2, int i3, int i4);
        void setUniformWithVec4V(const std::string& uniformName, int count, const GLfloat* fv);
        void setUniformWithIVec4V(const std::string& uniformName, int count, const int* iv);
        void setUniformWithTex2D(const std::string& uniformName, Texture2D* texture);
		void setUniformWithSamplerBuffer(const std::string& uniformName, GLuint textureId);
		void setUniformWithImageBuffer(const std::string& uniformName, GLuint textureId, GLenum access, GLenum format);
        
        void setUniformWithMat3V(const std::string& uniformName, int count, const float* fv);
        void setUniformWithMat3x4V(const std::string& uniformName, int count, const float* fv);
        void setUniformWithMat4v(const std::string& uniformName, int count, const float* fv);
        void setUniformWithMat4x3v(const std::string& uniformName, int count, const float* fv);
        
        // 获取uniform的位置
        GLint getUniformLocation(const std::string& uniformName);
        
        // property
        REGISTER_PROPERTY_GET_SET(std::string, m_sAddtionVertCode, AddtionVertCode)
        REGISTER_PROPERTY_GET_SET(std::string, m_sAddtionFragCode, AddtionFragCode)
        REGISTER_PROPERTY_GET_SET(std::string, m_sAddtionCompCode, AddtionCompCode)
        REGISTER_PROPERTY_GET_SET(std::string, m_sCompLocalGroupDefCode, CompLocalGroupDefCode)

	protected:
		// 初始化着色器程序
		bool initProgram(const char* vertSrc, const char* fragSrc, bool saveSource = false);
        bool cloneProgram(GLProgram* srcGLProgram);
		// 创建着色器
		bool createShader(GLenum type, GLuint& shader, const char* shaderSrc, bool saveSource);
        bool createShader(GLenum type, GLuint& shader, const char* shaderSource);
		// 绑定预定义的顶点属性变量位置
		void bindPredefinedVertexAttribs();
        // 获取存储着色器源码的指针
        GLchar*& getSourceSavePointer(GLenum type);


	protected:
		// 着色器程序(1.创建Program 2.绑定顶点片段着色器 3.链接)
		GLuint m_uProgram;
		// 顶点着色器(1.创建shader 2.绑定着色器内容 3.编译)
		GLuint m_uVertShader;
		// 片段着色器
		GLuint m_uFragShader;
        // 计算着色器
        GLuint m_uCompShader;
        // uniform位置  注意:char*不可以用来做key值，除非自己重写他的比较方法和hash方法。
        std::unordered_map<std::string, GLint> m_mUniformLocations;
        // textureId队列
        GLuint m_vTexIds[MAX_ACTIVE_TEXTURE];
		// 纹理单元
		std::unordered_map<std::string, GLuint> m_mTextureUnits;
		// 纹理单元计数
		GLuint m_uTextureUnitIndex;
        
        // 附加顶点着色器代码（在预定义头和源码之间）
        std::string m_sAddtionVertCode;
        // 附加片段着色器代码（在预定义头和源码之间）
        std::string m_sAddtionFragCode;
        // 计算着色器本地工作组定义
        std::string m_sCompLocalGroupDefCode;
        // 附加计算着色器代码 (在预定义头和源码之间)
        std::string m_sAddtionCompCode;

        // 顶点着色器源码
        GLchar* m_sVertexSource;
        // 片段着色器源码
        GLchar* m_sFragSource;
        // 计算着色器远吗
        GLchar* m_sCompSource;
        
	};
}

#endif
