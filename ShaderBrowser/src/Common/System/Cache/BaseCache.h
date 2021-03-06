#pragma once

#include <unordered_map>
#include <vector>
#include "Common/Tools/Utils.h"
#include "Common/Tools/BaseSingleton.h"

using namespace common;

namespace common
{
	// 类型说明: K: key, T: value, C: cache
	template <typename K, typename T, typename C>
	class BaseMapCache : public BaseSingleton<C>
	{
	public:
		BaseMapCache()
		{
			// 清理
			m_mContents.clear();
		}
		virtual ~BaseMapCache() {}

	public:
		// 注意：！！！
		// iterator是一个类的成员，而该类依赖于一个模板参数。因此我们必须在返回类型的声明中使用typename来告知编译器，iterator表示一个类型。
		// std::unordered_map<K, T>::iterator begin()	这样写错误！！！
		typename std::unordered_map<K, T>::iterator begin()
		{
			return m_mContents.begin();
		}
		typename std::unordered_map<K, T>::iterator end()
		{
			return m_mContents.end();
		}

	protected:
		// map操作
		bool add(K key, T val)
		{
			bool eraseFlag = false;

			auto itor = m_mContents.find(key);
			if (itor != m_mContents.end())
			{
				m_mContents.erase(itor);
				eraseFlag = true;
			}
			m_mContents.emplace(key, val);

			return eraseFlag;
		}
		T get(K key)
		{
			auto itor = m_mContents.find(key);
			if (itor != m_mContents.end())
			{
				return itor->second;
			}

			return nullptr;
		}
		bool removeByValue(T val)
		{
			for (auto itor = m_mContents.begin(); itor != m_mContents.end(); ++itor)
			{
				if (itor->second == val)
				{
					m_mContents.erase(itor);
					return true;
				}
			}
			return false;
		}
		bool remove(K key)
		{
			auto itor = m_mContents.find(key);
			if (itor != m_mContents.end())
			{
				m_mContents.erase(itor);
				return true;
			}
			return false;
		}

	protected:
		// 元素map
		std::unordered_map<K, T> m_mContents;

	};

	// 类型说明: T: value, C: cache
	template <typename T, typename C>
	class BaseVectorCache : public BaseSingleton<C>
	{
	public:
		BaseVectorCache()
		{
			// 清理
			m_mContents.clear();
		}
		virtual ~BaseVectorCache() {}

	protected:
		// vector操作
		void add(T val)
		{
			m_mContents.push_back(val);
		}
		bool remove(T val)
		{
			for (auto itor = m_mContents.begin(); itor != m_mContents.end(); ++itor)
			{
				if (itor->second == val)
				{
					m_mContents.erase(itor);
					return true;
				}
			}
			return false;
		}

	protected:
		// 元素队列
		std::vector<T> m_mContents;

	};

}

