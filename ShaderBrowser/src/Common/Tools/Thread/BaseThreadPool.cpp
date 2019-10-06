#include "BaseThreadPool.h"
#include <iostream>

namespace common
{
    BaseThreadPool::BaseThreadPool(unsigned int maxCount, int customPoolId/* = -1*/)
        : BaseThread(customPoolId, std::bind(&BaseThreadPool::poolSleepCondition, this), false)
        , m_uMaxThreadCount(maxCount)
    {
        m_vThreads.reserve(m_uMaxThreadCount);
        m_vSleepThreads.reserve(m_uMaxThreadCount);
        m_vWeekupThreads.reserve(m_uMaxThreadCount);
        

        m_pThread = new std::thread(&BaseThreadPool::dispatchTask, this);
        //m_pThread->detach();
        m_uThreadId = m_pThread->get_id();
    }
    
    BaseThreadPool::~BaseThreadPool()
    {
		for (int i = 0; i < m_vThreads.size(); ++i)
		{
			delete m_vThreads[i];
		}
        m_vThreads.clear();
        m_vSleepThreads.clear();
        m_vWeekupThreads.clear();
	}
    
    void BaseThreadPool::test()
    {
        std::cout<<"==========cache size========"<<getTaskCount()<<endl;
        std::cout<<"==========weekup size========"<<m_vWeekupThreads.size()<<endl;
        std::cout<<"==========sleep size========"<<m_vSleepThreads.size()<<endl;
        std::cout<<"==========total size========"<<m_vThreads.size()<<endl;
    }
    
    bool BaseThreadPool::poolSleepCondition()
    {
        // 缓存任务不为空，或者仍有线程在工作，或者准备终结线程，则不休眠(返回true)
        bool isActive = m_vWeekupThreads.size()!=0 || defaultSleepCondition();
        m_bIsActive = isActive;
        return isActive;
    }

    void BaseThreadPool::dispatchTask()
    {
        Task new_task = nullptr;
        BaseThread* sleep_thread;
        while(true)
        {
            if (!poolSleepCondition())
            {
				std::unique_lock<std::mutex> signal(m_SignalMutex);
				m_oSleepCondition.wait(signal, m_pSleepCondition);
            }
            
			// 1.检查是否有没有缓存任务
            if(!isTasksEmpty())
            {
                // 检测是否有空闲线程，如果有就继续执行
                sleep_thread = getSleepThread();
                if (sleep_thread)
                {
                    new_task = popTask();
                    sleep_thread->addTask(new_task);
                }
            }
            
			// 2.回收空闲线程
            recoverSleepThreads();
            
            if (m_bReadyTerminate==true)
            {
				break;
            }

        }

		m_bIsActive = false;
		m_bIsTerminate = true;
    }
    
	BaseThread* BaseThreadPool::getSleepThread()
    {
        BaseThread* thread;
        if (m_vSleepThreads.size() > 0)
        {
            thread = m_vSleepThreads[0];
            m_vSleepThreads.erase(m_vSleepThreads.begin());
            m_vWeekupThreads.push_back(thread);
            return thread;
        }
        else if(m_vThreads.size() < m_uMaxThreadCount)
        {
            thread = new BaseThread(m_vThreads.size());
            m_vThreads.push_back(thread);
            m_vWeekupThreads.push_back(thread);
            return thread;
        }
        
        return nullptr;
    }
    
    void BaseThreadPool::recoverSleepThreads()
    {
        BaseThread* thread;
        for(int i=0; i<m_vWeekupThreads.size();)
        {
            thread = m_vWeekupThreads[i];
            if (!thread->getIsActive())
            {
                m_vWeekupThreads.erase(m_vWeekupThreads.begin()+i);
                m_vSleepThreads.push_back(thread);
            }
            else
            {
                ++i;
            }
        }
    }

}
