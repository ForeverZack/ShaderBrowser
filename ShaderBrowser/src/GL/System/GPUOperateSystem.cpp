#include "GPUOperateSystem.h"
#include "Core/RenderCore.h"

namespace customGL
{
    GPUOperateSystem::GPUOperateSystem()
    {
        m_vCommands.clear();
    }
    
    GPUOperateSystem::~GPUOperateSystem()
    {
        
    }
    
    void GPUOperateSystem::addCommand(BaseGPUOperateCommand* cmd)
    {
//        // TODO: 为了让程序跑起来，现在立刻执行，不知道后面延迟执行会不会报错。。。
//        cmd->execute();
//        cmd->finish();
//        return;
        
        // 向当前逻辑帧命令队列添加命令
        addCommand(core::LogicCore::getInstance()->getFrameIndex(), cmd);
    }
    
    void GPUOperateSystem::addCommand(unsigned long frameIndex, BaseGPUOperateCommand* cmd)
    {
        m_mWaitCommands[frameIndex].push_back(cmd);
    }
    
    const std::vector<BaseGPUOperateCommand*>& GPUOperateSystem::getCommands(unsigned long frameIndex)
    {
        return m_mWaitCommands[frameIndex];
    }
    
    void GPUOperateSystem::eraseCommands(unsigned long frameIndex)
    {
        m_mWaitCommands.erase(frameIndex);
    }
    
    void GPUOperateSystem::update()
    {
        // 将逻辑线程命令队列加入到渲染线程待执行队列中
        unsigned int frameIndex = core::RenderCore::getInstance()->getFrameIndex();
        m_vCommands.clear();
        m_vCommands = getCommands(frameIndex);
        eraseCommands(frameIndex);
        
        // 注意！！！！！！！！！！！
        // 一定要等渲染线程拷贝完上一帧的命令队列数据，逻辑线程才可以继续执行下一帧！！！！！！！！！！！
        // set mutex
        core::LogicCore::getInstance()->setLogicState(core::LogicCore::LogicCoreState::LCS_Prepare);
        
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

