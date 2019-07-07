#include "Utils.h"

namespace common
{
	void BROWSER_ASSERT(bool condition, const char* msg)
	{
        if (!condition)
        {
            std::cout << "ERROR: " << msg << endl;
            assert(condition);
        }
    }
    
    void BROWSER_WARNING(bool condition, const char* msg)
    {
        if (!condition)
        {
            std::cout << "WARNING: " << msg << endl;
        }
    }

	GLchar* Utils::readFile(const char* filename)
	{
#ifdef _WIN32
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

		return source;
	}
    
    VertexAttribDeclaration* Utils::createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, VertexDataType dataType /*= VertexDataType::Float*/)
    {
        VertexAttribDeclaration* declaration = new VertexAttribDeclaration();
        declaration->index = location;
        declaration->size = size;
        declaration->type = type;
        declaration->normalized = normalized;
        declaration->stride = stride;
        declaration->data_type = dataType;
        
        return declaration;
    }
    
    VertexAttribDeclaration* Utils::createVertexAttribDeclaration(GLuint location, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLvoid* pointer, GLvoid* data, GLint dataSize, VertexDataType dataType /*=VertexDataType::Float*/)
    {
        VertexAttribDeclaration* declaration = new VertexAttribDeclaration();
        declaration->index = location;
        declaration->size = size;
        declaration->type = type;
        declaration->normalized = normalized;
        declaration->stride = stride;
        declaration->pointer = pointer;
        declaration->data = data;
        declaration->data_size = dataSize;
        declaration->data_type = dataType;
        
        return declaration;
    }

    FeedbackBufferDeclaration* Utils::createFeedbackBufferDeclaration(GLuint bindIdx, GLuint size, const string& name, FeedbackBufferType type/* = FeedbackBufferType::ArrayBuffer*/)
    {
        FeedbackBufferDeclaration* declaration = new FeedbackBufferDeclaration();
        declaration->varying = name;
        declaration->bindIdx = bindIdx;
        declaration->size = size;
        declaration->type = type;
        
        return declaration;
    }

}
