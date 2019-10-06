#include "BaseThread.h"

namespace common
{
    
    BaseThread::BaseThread(int customThreadId/* = -1*/, SleepCondition sleepCondition/* = nullptr*/, bool autoCreate/* = true*/)
        : m_pThread(nullptr)
        , m_iCustomThreadId(customThreadId)
        , m_bInited(false)
        , m_pSleepCondition(sleepCondition)
        , m_bIsActive(false)
		, m_bIsTerminate(false)
		, m_bReadyTerminate(false)
    {
        if (autoCreate)
        {
			if (!sleepCondition)
			{
				m_pSleepCondition = std::bind(&BaseThread::defaultSleepCondition, this);
			}
            m_pThread = new std::thread(&BaseThread::innerThreadFunc, this);
            //m_pThread->detach();    // detach之后线程变为"非joinable"
            m_uThreadId = m_pThread->get_id();
        }

    }
    
    BaseThread::~BaseThread()
    {
        // 结束进程
        terminateThread();
	}
    
    void BaseThread::weekupThreadOnce()
    {
		std::unique_lock<std::mutex> signal(m_SignalMutex);
		m_bIsActive = true;
        m_oSleepCondition.notify_one();
    }

    void BaseThread::terminateThread()
    {
         // 去销毁一个仍然可以“joinable”的C++线程对象会被认为是一种错误。为了销毁一个C++线程对象，约么join()函数需要被调用（并结束），要么detach()函数被调用。如果一个C++线程对象当销毁时仍然可以被join，异常会被抛出。
        m_bReadyTerminate = true;
        // 等待线程执行完成
        while(getIsActive());
        if (m_bIsTerminate==false)
        {
            m_oTasks.clear();
            weekupThreadOnce();
        }
		if (m_pThread->joinable())
		{
            // 等待线程函数退出
			m_pThread->join();
		}
		delete m_pThread;
	}
    
    bool BaseThread::getIsActive()
    {
        return m_bIsActive == true;
    }
    
    void BaseThread::addTask(const Task& task)
    {
        m_oTasks.push(task);
        weekupThreadOnce();
    }
    
    BaseThread::Task BaseThread::popTask()
    {
        if (!isTasksEmpty())
        {
            return m_oTasks.pop();
        }
        else
        {
            return nullptr;
        }
    }
    
    size_t BaseThread::getTaskCount()
    {
        return m_oTasks.size();
    }
    
    bool BaseThread::isTasksEmpty()
    {
        return m_oTasks.empty();
    }
    
    bool BaseThread::defaultSleepCondition()
    {
        // 缓存任务不为空，或者准备终结线程，则不休眠(返回true)
        bool isActive = !isTasksEmpty() || m_bReadyTerminate==true;
        m_bIsActive = isActive;
        return isActive;
    }
    
    void BaseThread::innerThreadFunc()
    {
        Task task = nullptr;

        while(true)
        {
            if (!m_bInited)
            {
                // 初始化后，不执行回调
                m_bInited = true;
				{
					std::unique_lock<std::mutex> signal(m_SignalMutex);
					m_oSleepCondition.wait(signal, m_pSleepCondition);// 默认返回false
				}
            }
            
            task = popTask();
            if (task)
            {
                task();
            }
            
            
            if (m_bReadyTerminate==true)
            {
                // 终止
                break;
            }
            // 执行完一次，判断有没有需要继续执行的任务
			if (!defaultSleepCondition())
			{
                std::unique_lock<std::mutex> signal(m_SignalMutex);
                m_oSleepCondition.wait(signal, m_pSleepCondition); // 默认返回false
			}
        }

		m_bIsActive = false;
		m_bIsTerminate = true;
    }
    
    
    
    
}
