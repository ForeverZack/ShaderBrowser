#pragma once

#include <memory>
#include <vector>
#include "BaseThread.h"
using namespace std;

namespace common
{

    class BaseThreadPool : public BaseThread
    {
    public:
        BaseThreadPool(unsigned int maxCount, int customPoolId = -1);
        ~BaseThreadPool();
    
    public:
        // 打印线程数量
        void test();
        
    private:
        // 线程工作分配
        void dispatchTask();
        // 获取空闲线程
        BaseThread* getSleepThread();
        // 回收空闲线程
        void recoverSleepThreads();
        // 判断是否可以休眠
        bool poolSleepCondition();
        
    protected:
        // 以下变量都只有异步线程赋值操作，所以不需要加锁
        // 最大允许线程数量
        unsigned int m_uMaxThreadCount;
        // 所有线程队列
        std::vector<BaseThread*> m_vThreads;
        // 空闲线程队列
        std::vector<BaseThread*> m_vSleepThreads;
        // 繁忙线程队列
        std::vector<BaseThread*> m_vWeekupThreads;

    };
}
