#pragma once

#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <queue>
using namespace std;

namespace common
{
    // 注意事项
    // 1. std::function可以绑定到全局函数或类的静态成员函数，如果要绑定到类的非静态成员函数，则需要使用std::bind。
	// 2. 对于需要多个线程访问的变量，需要加互斥锁（可以利用unique_lock来简化操作，unique_lock只传一个mutex，会自动上锁，析构时会自动解锁）
			//explicit unique_lock(_Mutex& _Mtx)
			//	: _Pmtx(&_Mtx), _Owns(false)
			//{	// construct and lock
			//	_Pmtx->lock();
			//	_Owns = true;
			//}
	// 3. join()阻塞当前线程，直至 *this 所标识的线程完成其执行 ,*this 所标识的线程的完成同步于从 join() 的成功返回。; 
	//		detach()从thread对象分离执行的线程，允许执行独立地持续。一旦线程退出，则释放所有分配的资源。调用 detach 后， *this 不再占有任何线程。
    // 4. 对于m_bIsActive，应该放在condition_variable.wait的第二个回调函数中赋值，这样才能得到正确的线程激活状态
    
    // 互斥变量
    template <typename T>
    class MutexVariable
    {
    public:
        MutexVariable() {}
        MutexVariable(T val)
            : m_oVariable(val)
        {}
        ~MutexVariable() {}
    
    public:
		// 赋值运算
		T &operator=(const T& val)
		{
			std::unique_lock<std::mutex> lock(m_oMutex);
			m_oVariable = val;
			return m_oVariable;
		}
        bool operator==(const T& val)
        {
            std::unique_lock<std::mutex> lock(m_oMutex);
            return m_oVariable == val;
        }
        bool operator!=(const T& val)
        {
            std::unique_lock<std::mutex> lock(m_oMutex);
            return m_oVariable != val;
        }
		T getValue()
		{
			std::unique_lock<std::mutex> lock(m_oMutex);
			return m_oVariable;
		}
        
    private:
        // 变量
        T m_oVariable;
        // 互斥锁
        std::mutex m_oMutex;
    };
    
    
    // 互斥队列
    template <typename T>
    class MutexQueue
    {
    public:
        MutexQueue() {}
        ~MutexQueue() {}
    
    public:
        T pop()
        {
            std::unique_lock<std::mutex> lock(m_oMutex);
            T val = m_vQueue.front();
            m_vQueue.pop();
            return val;
        }
        void push(const T& val)
        {
            std::unique_lock<std::mutex> lock(m_oMutex);
            m_vQueue.push(val);
        }
        size_t size()
        {
            std::unique_lock<std::mutex> lock(m_oMutex);
            return m_vQueue.size();
        }
        bool empty()
        {
            std::unique_lock<std::mutex> lock(m_oMutex);
            return m_vQueue.empty();
        }
        void clear()
        {
            std::unique_lock<std::mutex> lock(m_oMutex);
            while(!m_vQueue.empty())
            {
                m_vQueue.pop();
            }
        }
        void operateQueue(std::function<void(std::queue<T>&)> callback)
        {
            std::unique_lock<std::mutex> lock(m_oMutex);
            callback(m_vQueue);
        }
        
    private:
        // 队列
        std::queue<T> m_vQueue;
        // 互斥锁
        std::mutex m_oMutex;
    };

    class BaseThread
    {
    public:
        // Task
        typedef std::function<void()> Task;
        // 休眠条件 std::condition_variable::wait(signal, SleepCondition) 不传默认返回false(休眠阻塞)  返回true时继续执行
        typedef std::function<bool()> SleepCondition;
        
    public:
        BaseThread(int customThreadId = -1, SleepCondition sleepCondition = nullptr, bool autoCreate = true);
        virtual ~BaseThread();
    
    public:
        // 添加任务
        void addTask(const Task& task);
        // 唤醒线程
        void weekupThreadOnce();
        // 结束线程
        void terminateThread();
        // 是否处于激活状态
        bool getIsActive();

    protected:
        // 弹出任务
        Task popTask();
        // 任务数量
        size_t getTaskCount();
        // 任务池是否为空
        bool isTasksEmpty();
        // 默认休眠条件
        bool defaultSleepCondition();
        
    private:
        // 线程函数
        void innerThreadFunc();
        
    protected:
        // 线程指针
        std::thread* m_pThread;
		// 信号互斥变量
		std::mutex m_SignalMutex;
        // 条件变量
        std::condition_variable m_oSleepCondition;
        // 任务函数
        MutexQueue<Task> m_oTasks;
        // 休眠条件函数
        SleepCondition m_pSleepCondition;
        // 是否初始化
        bool m_bInited;
        // 是否激活
        MutexVariable<bool> m_bIsActive;
        // 终止信号
		MutexVariable<bool> m_bReadyTerminate;
        // 是否终止
		MutexVariable<bool> m_bIsTerminate;
        // 线程id
        std::thread::id m_uThreadId;
        // 自定义线程id
        int m_iCustomThreadId;
    };
}
