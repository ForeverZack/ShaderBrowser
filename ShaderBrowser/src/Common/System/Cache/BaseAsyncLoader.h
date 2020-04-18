#pragma once

#include <thread>
#include <mutex>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Common/Tools/Thread/BaseThreadPool.h"

namespace common
{
	// 异步加载数据
	template <typename DataType, typename CallbackFunc, typename ExtraCreateDataType=const char*>
	class AsyncData
	{
	public:
		AsyncData(const std::string fp, CallbackFunc cb, shared_ptr<ExtraCreateDataType> extra = nullptr)
			: fullpath(fp)
			, data(nullptr)
			, loadSuccess(false)
			, extradata(extra)
		{
			callbacks.clear();
			callbacks.push_back(cb);
		}

		// 资源绝对路径
		std::string fullpath;
		// 创建方法的额外参数
		shared_ptr<ExtraCreateDataType> extradata;
		// 加载完成回调 (由于可能存在纹理资源正在加载但还没加载完成，这时候加载了同一张相同的纹理资源，会造成内存泄漏(后面加载的内存会替代前面的内存)，所以把回调方法合并进来)
		std::vector<CallbackFunc> callbacks;
		// 数据
		DataType* data;
		// 是否加载完成
		bool loadSuccess;
	};

	// 加载器
    // CreateFunc: 资源读取函数类型(通过构造函数将读取方法传入进来); DataType: 数据类型; CallbackFunc: 回调类型; 额外的创建参数类型
	template <typename DataType, typename CallbackFunc, typename ExtraCreateDataType = const char*>
	class BaseAsyncLoader
	{
        // Loader默认允许最大线程数
        #define DEFAULT_LOADER_MAX_THREAD_COUNT 2
		typedef std::function<DataType*(const std::string&, shared_ptr<ExtraCreateDataType>)> CreateFunc;

	public:
		BaseAsyncLoader(CreateFunc createFunc, int maxThreadCount = DEFAULT_LOADER_MAX_THREAD_COUNT)
			: m_CreateFunc(createFunc)
		{
			m_vResponses.clear();
			m_vRequestQueue.clear();
			m_vResponseQueue.clear();
			m_mLoadingAsyncDatas.clear();
            
            m_pThreadPool = new BaseThreadPool(maxThreadCount);
		}
		virtual ~BaseAsyncLoader()
		{
            delete m_pThreadPool;
		}

	public:
		// 异步加载接口(注意！！这里使用绝对路径)
		void loadResourceAsync(const std::string& fullpath, CallbackFunc callback, shared_ptr<ExtraCreateDataType> extra = nullptr)
		{
			std::unique_lock<std::mutex> lock(m_oLoadingDataMutex);

			// 检测资源是否正在被加载（防止重复加载，造成内存泄漏）
			auto itor = m_mLoadingAsyncDatas.find(fullpath);
			if (itor != m_mLoadingAsyncDatas.end())
			{
				itor->second->callbacks.push_back(callback);
				return;
			}

			// 如果没有，则正常加载该资源
			{
				m_oRequestMutex.lock();
				AsyncData<DataType, CallbackFunc, ExtraCreateDataType>* asyncData = new AsyncData<DataType, CallbackFunc, ExtraCreateDataType>(fullpath, callback, extra);
				m_vRequestQueue.push_back(asyncData);
				m_oRequestMutex.unlock();

				// 插入正在加载的列表
				m_mLoadingAsyncDatas.emplace(fullpath, asyncData);
			}
            
            // 添加加载任务
            m_pThreadPool->addTask(std::bind(&BaseAsyncLoader<DataType, CallbackFunc, ExtraCreateDataType>::loadResource, this));
		}
		// 获取加载返回队列
		const std::vector<AsyncData<DataType, CallbackFunc, ExtraCreateDataType>*>& getResponseQueue()
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
//			std::mutex signalMutex;
//			std::unique_lock<std::mutex> signal(signalMutex);
			AsyncData<DataType, CallbackFunc, ExtraCreateDataType>* asyncData = nullptr;
//			while (true)
//			{
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
                    return;
				}


				// 加载数据
				{
					DataType* result = nullptr;
					if (asyncData->extradata)
					{
						result  = m_CreateFunc(asyncData->fullpath.c_str(), asyncData->extradata);
					}
					else
					{
						result = m_CreateFunc(asyncData->fullpath.c_str(), nullptr);
					}
					asyncData->data = result;
					asyncData->loadSuccess = true;
				}

				// 返回加载结果
				{
					m_oResponseMutex.lock();
					m_vResponseQueue.push_back(asyncData);
					m_oResponseMutex.unlock();
				}

				// 从正在加载的资源列表中删除
				m_oLoadingDataMutex.lock();
				m_mLoadingAsyncDatas.erase(asyncData->fullpath);
				m_oLoadingDataMutex.unlock();

//			}
		}

	protected:
        // 线程池
        BaseThreadPool* m_pThreadPool;
		// 请求互斥量
		std::mutex m_oRequestMutex;
		// 返回互斥量
		std::mutex m_oResponseMutex;
		// 加载请求队列（供异步加载的线程读取数据）
		std::deque<AsyncData<DataType, CallbackFunc, ExtraCreateDataType>*> m_vRequestQueue;
		// 加载返回队列（供异步加载的线程写入结果数据）
		std::deque<AsyncData<DataType, CallbackFunc, ExtraCreateDataType>*> m_vResponseQueue;

		// 当前正在加载的互斥量
		std::mutex m_oLoadingDataMutex;
		// 当前正在加载对象 (因为针对每种不同的加载器，当前只会开启一个线程去加载，每次只加载一个资源对象，所以这里暂时不需要用队列去保存加载资源的指针)
		std::unordered_map<std::string, AsyncData<DataType, CallbackFunc, ExtraCreateDataType>*> m_mLoadingAsyncDatas;

		// 加载返回队列（供主线程访问加载的数据结果）
		std::vector<AsyncData<DataType, CallbackFunc, ExtraCreateDataType>*> m_vResponses;

		// 数据加载方法
		CreateFunc m_CreateFunc;
	};
}
