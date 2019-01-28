#ifndef GL_GLPROGRAM_H
#define GL_GLPROGRAM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>

namespace customGL
{
    // 最大支持的纹理单元数量
    static const int MAX_ACTIVE_TEXTURE = 16;
    
	class GLProgram
	{
	public:
		// 默认GLProgram名称
		static const char* DEFAULT_GLPROGRAM_NAME;

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


			// 最大值
			VERTEX_ATTR_MAXCOUNT,
		};
        
        // 预定义的uniform变量枚举
        enum
        {
            // 纹理0: 主纹理
            UNIFORM_CGL_TEXUTRE0 = 0,
            // 纹理1
            UNIFORM_CGL_TEXUTRE1,
            //
            
            
            // 预定义Uniform变量个数
            UNIFORM_MAX_COUNT
        };

		// 预定义的顶点着色器属性名称
		static const char* ATTRIBUTE_NAME_POSITION;
		static const char* ATTRIBUTE_NAME_COLOR;
		static const char* ATTRIBUTE_NAME_COORD;

		// 预定义的uniform变量	（与前面枚举要对应上！！）
		static const char* SHADER_UNIFORMS_ARRAY[UNIFORM_MAX_COUNT];

	public:
		static GLProgram* create(const char* vertSrc, const char* fragSrc);

	public:
		GLProgram();
		~GLProgram();

		// 使用着色器程序
		void useProgram();
		// 更新预定义uniform位置
		void updatePreDefinedUniformsLocation();
		// 设置uniform
        void setUniformWithInt(const std::string& uniformName, int value);
        void setUniformWithFloat(const std::string& uniformName, float value);
        void setUniformWithMat3(const std::string& uniformName, const glm::mat3& value);
        void setUniformWithMat4(const std::string& uniformName, const glm::mat4& value);
        void setUniformWithFloatV(const std::string& uniformName, int size, const float* fv);
        void setUniformWithVec2(const std::string& uniformName, GLfloat f1, GLfloat f2);
        void setUniformWithVec3(const std::string& uniformName, GLfloat f1, GLfloat f2, GLfloat f3);
		void setUniformWithVec4(const std::string& uniformName, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);
        void setUniformWithTex2D(const std::string& uniformName, GLuint textureId);
        
        // 获取uniform的位置
        GLint getUniformLocation(const std::string& uniformName);

	private:
		// 初始化着色器程序
		bool initProgram(const char* vertSrc, const char* fragSrc);
		// 创建着色器
		bool createShader(GLenum type, GLuint& shader, const char* shaderSrc);
		// 绑定预定义的顶点属性变量位置
		void bindPredefinedVertexAttribs();
        


	private:
		// 着色器程序(1.创建Program 2.绑定顶点片段着色器 3.链接)
		GLuint m_uProgram;
		// 顶点着色器(1.创建shader 2.绑定着色器内容 3.编译)
		GLuint m_uVertShader;
		// 片段着色器
		GLuint m_uFragShader;
        // uniform位置  注意:char*不可以用来做key值，除非自己重写他的比较方法和hash方法。
        std::unordered_map<std::string, GLint> m_mUniformLocations;
        // textureId队列
        GLuint m_vTexIds[MAX_ACTIVE_TEXTURE];
		// 纹理单元
		std::unordered_map<std::string, GLuint> m_mTextureUnits;
		// 纹理单元计数
		GLuint m_uTextureUnitIndex;
	};
}

#endif
