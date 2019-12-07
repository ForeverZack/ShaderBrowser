#pragma once

#include "GL/GPUResource/Shader/GLProgram.h"

namespace customGL
{
    /*
        实现ComputeShader的OpenGL部分
        关于ComputeShader的说明：它是由本地工作组(由执行单元构成，是三维的，在shader中声明layout(local_size_x=1024, local_size_y=1024, local_size_z = 1);不声明则默认值为1) 和 全局工作组(由本地工作组构成，在调用glDispatchCompute函数时传入,num_groups_x,num_groups_y,num_groups_z)构成的。
     
        计算着色器的内置变量声明:
             1. const uvec3 gl_WorkGroupSize;  // 本地工作组大小 (local_size_x, local_size_y, local_size_z)
             2. in uvec3 gl_NumWorkGroups;  // 全局工作组的大小(即传递给glDispatchCompute的参数，其值为num_groups_x, num_groups_y, num_groups_z)
             3. in uvec3 gl_LocalInvocationID;  // 当前执行单元在本地工作组中的位置。范围从uvec3(0)到gl_WorkGroupSize-uvec3(1)
             4. in uvec3 gl_WorkGroupID;    // 当前本地工作组在全局工作组中的位置。范围从uvec3(0)到gl_NumWorkGroups-uvec3(1)
             5. in uvec3 gl_GlobalInvocationID; // 当前执行单元在全局工作组中的位置，其值为gl_WorkGroupID*gl_WorkGroupSize+gl_LocalInvocaionID
             6. in uint gl_LocalInvocationIndex;    // 是gl_LocalInvocationID的一种扁平化形式(一维),其值为gl_LocalInvocationID.z*gl_WorkGroupSize.x*gl_WorkGroupSize.y+gl_LocalInvocationID.y*gl_WorkGroupSize.x+gl_LocalInvocationID.x
     
        通信与同步:
            1.在shader中使用运行屏障：barrier()
            2.在c++中使用内存屏障：glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
     
        另外，注意shader中对于共享内存 关键字shared 的使用。
     */
    
    class GLComputeProgram : public GLProgram
	{
	public:
		static GLComputeProgram* create(const char* src, int local_size_x = 1, int local_size_y = 1, int local_size_z = 1, int num_groups_x = 1, int num_groups_y = 1, int num_groups_z = 1);

	public:
		GLComputeProgram();
		~GLComputeProgram();
    
    public:
        // 执行计算着色器
        void executeComputeProgram();
        void executeComputeProgram(int num_groups_x, int num_groups_y, int num_groups_z);
        
    protected:
        // 初始化ComputeShader
        bool initComputeProgram(const char* src, int local_size_x = 1, int local_size_y = 1, int local_size_z = 1, int num_groups_x = 1, int num_groups_y = 1, int num_groups_z = 1);

    protected:
        // 本地组大小
        int m_iLocalSizeX;
        int m_iLocalSizeY;
        int m_iLocalSizeZ;
        // 全局工作组的大小
        int m_iNumGroupsX;
        int m_iNumGroupsY;
        int m_iNumGroupsZ;

	};
}

