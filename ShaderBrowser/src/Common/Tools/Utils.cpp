#include "Utils.h"

namespace common
{
	void BROWSER_ASSERT(bool condition, const char* msg)
	{
        if (!condition)
        {
            std::cout << msg << endl;
            assert(condition);
        }
    }
    
    void BROWSER_WARNING(bool condition, const char* msg)
    {
        if (!condition)
        {
            std::cout << msg << endl;
        }
    }

	const GLchar* Utils::readFile(const char* filename)
	{
#ifdef WIN32
        // WIN32
		FILE* infile;
		fopen_s(&infile, filename, "rb");
#else
        // macOS
		FILE* infile = fopen(filename, "rb");
#endif

		if (!infile) {
#ifdef _DEBUG
			std::cerr << "Unable to open file '" << filename << "'" << std::endl;
#endif /* DEBUG */
			return NULL;
		}

		fseek(infile, 0, SEEK_END);
		int len = ftell(infile);
		fseek(infile, 0, SEEK_SET);

		GLchar* source = new GLchar[len + 1];

		fread(source, 1, len, infile);
		fclose(infile);

		source[len] = 0;

		return const_cast<const GLchar*>(source);
	}

    TextureData Utils::createTextureData(const std::string& uniformName, Texture2D* texture)
	{
		TextureData textureData;
		textureData.texture = texture;
		textureData.uniformName = uniformName;

		return textureData;
	}
    
    VertexAttribDeclaration* Utils::createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride)
    {
        VertexAttribDeclaration* declaration = new VertexAttribDeclaration();
        declaration->index = location;
        declaration->size = size;
        declaration->type = type;
        declaration->normalized = normalized;
        declaration->stride = stride;
        
        return declaration;
    }



}
