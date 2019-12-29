#include "GPUOperateSystem.h"

namespace customGL
{
    GPUOperateSystem::GPUOperateSystem()
    {
        m_vCommands.clear();
        m_qWaitCommands.clear();
    }
    
    GPUOperateSystem::~GPUOperateSystem()
    {
        
    }
    
    void GPUOperateSystem::addCommand(BaseGPUOperateCommand* cmd)
    {
        // TODO: 为了让程序跑起来，现在立刻执行，不知道后面延迟执行会不会报错。。。
        cmd->execute();
        cmd->finish();
        return;
        
        m_qWaitCommands.push(cmd);
    }
    
    void GPUOperateSystem::update()
    {
        return;
        // 将逻辑线程命令队列加入到渲染线程待执行队列中
        m_vCommands.clear();
        m_qWaitCommands.operateQueue([&](std::queue<BaseGPUOperateCommand*>& queue)
        {
            int length = queue.size();
            for (int i=0; i<length; ++i)
            {
                m_vCommands.push_back(queue.front());
                queue.pop();
            }
        });
        
        // 注意！！！！！！！！！！！
        // 一定要等渲染线程拷贝完上一帧的命令队列数据，逻辑线程才可以继续执行下一帧！！！！！！！！！！！

//        // test
//        m_vCommands.clear();
//        return;
        
        // 渲染线程执行命令队列
        BaseGPUOperateCommand* cmd = nullptr;
        for (auto itor=m_vCommands.begin(); itor!=m_vCommands.end(); ++itor)
        {
            cmd = *itor;
            BROWSER_WARNING(cmd->getIsReady(), "BaseGPUOperateCommand is not ready!!!");
            
            cmd->execute();
            cmd->finish();
        }
        
        
    }
    
}

