#include "AnimatorFeedback.h"
#include "Common/Tools/FileUtils.h"

using namespace customGL;

namespace browser
{
    // 顶点着色器shader
#define VERTEX_SHADER_PROGRAM_SRC "shaders/default/anim_feedback_test.vert"
    // varyings
    // 名称
    static const char* VARYINGS[] = {"result_position", "result_rotation", "result_scale"};
    // 数量
    static const int VARYINGS_NUM = 3;
    
	AnimatorFeedback::AnimatorFeedback()
	{
	}

	AnimatorFeedback::~AnimatorFeedback()
	{
	}
    
    bool AnimatorFeedback::initFeedback()
    {
        std::string vert_shader_fullpath = FileUtils::getInstance()->getAbsolutePathForFilename(VERTEX_SHADER_PROGRAM_SRC);
        
        std::string addtionVertCode = "";
        
        BaseFeedback::initFeedback(vert_shader_fullpath.c_str(), VARYINGS, VARYINGS_NUM, addtionVertCode, GL_SEPARATE_ATTRIBS);
        
        return true;
    }
    
  
    

}
