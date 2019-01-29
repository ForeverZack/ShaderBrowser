#pragma once

#include <thread>
#include <mutex>
#include <deque>
#include <vector>

template <typename DataType, typename CallbackFunc>
// 异步加载数据
class AsyncData
{
public:
	AsyncData(const std::string fp, CallbackFunc cb)
		: fullpath(fp)
		, callback(cb)
		, data(nullptr)
		, loadSuccess(false)
	{
	}

	// 资源绝对路径
	std::string fullpath;
	// 加载完成回调
	CallbackFunc callback;
	// 数据
	DataType* data;
	// 是否加载完成
	bool loadSuccess;
};

// 加载器
// 
// CreateFunc: 资源读取函数类型(通过构造函数将读取方法传入进来)，DataType: 数据类型，回调类型
template <typename CreateFunc, typename DataType, typename CallbackFunc>
class BaseAsyncLoader
{
public:
	BaseAsyncLoader(CreateFunc createFunc)
		: m_oThread(nullptr)
		, m_CreateFunc(createFunc)
	{
		m_vResponses.clear();
		m_vRequestQueue.clear();
		m_vResponseQueue.clear();
	}
	virtual ~BaseAsyncLoader()
	{
	}

public:
	// 异步加载接口(注意！！这里使用绝对路径)
	void loadResourceAsync(const std::string& fullpath, CallbackFunc callback)
	{
		if (!m_oThread)
		{
			m_oThread = new std::thread(&BaseAsyncLoader<CreateFunc, DataType, CallbackFunc>::loadResource, this);
		}

		m_oRequestMutex.lock();
		AsyncData<DataType, CallbackFunc>* asyncData = new AsyncData<DataType, CallbackFunc>(fullpath, callback);
		m_vRequestQueue.push_back(asyncData);
		m_oRequestMutex.unlock();

		// notify
		m_oSleepCondition.notify_one();
	}
	// 获取加载返回队列
	const std::vector<AsyncData<DataType, CallbackFunc>*>& getResponseQueue()
	{
		m_vResponses.clear();
		m_oResponseMutex.lock();
		for (int i = 0; i < m_vResponseQueue.size(); ++i)
		{
			m_vResponses.push_back(m_vResponseQueue[i]);
		}
		m_vResponseQueue.clear();
		m_oResponseMutex.unlock();

		return m_vResponses;
	}

private:
	// 异步加载函数
	void loadResource()
	{
		std::mutex signalMutex;
		std::unique_lock<std::mutex> signal(signalMutex);
		AsyncData<DataType, CallbackFunc>* asyncData = nullptr;
		while (true)
		{
			m_oRequestMutex.lock();
			if (m_vRequestQueue.empty())
			{
				asyncData = nullptr;
			}
			else
			{
				asyncData = m_vRequestQueue.front();
				m_vRequestQueue.pop_front();
			}
			m_oRequestMutex.unlock();

			// 判断有没有加载数据的请求，如果没有，则休眠线程，等待下一次激活
			if (!asyncData)
			{
				// wait
				m_oSleepCondition.wait(signal);
				continue;
			}

			// 加载数据
			asyncData->data = m_CreateFunc(asyncData->fullpath.c_str());
			asyncData->loadSuccess = true;

			// 返回加载结果
			m_oResponseMutex.lock();
			m_vResponseQueue.push_back(asyncData);
			m_oResponseMutex.unlock();
		}
	}

protected:
	// 异步加载线程
	std::thread* m_oThread;
	// 条件变量
	std::condition_variable m_oSleepCondition;
	// 请求互斥量
	std::mutex m_oRequestMutex;
	// 返回互斥量
	std::mutex m_oResponseMutex;
	// 加载请求队列（供异步加载的线程读取数据）
	std::deque<AsyncData<DataType, CallbackFunc>*> m_vRequestQueue;
	// 加载返回队列（供异步加载的线程读取数据）
	std::deque<AsyncData<DataType, CallbackFunc>*> m_vResponseQueue;

	// 加载返回队列（供主线程访问加载的数据结果）
	std::vector<AsyncData<DataType, CallbackFunc>*> m_vResponses;

	// 数据加载方法
	CreateFunc m_CreateFunc;
};