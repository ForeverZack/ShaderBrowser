#pragma once

namespace common
{

	// 单例模板
	template <typename C>
	class BaseSingleton
	{
	public:
		static C* getInstance();
        
	public:
		BaseSingleton() {}
		virtual ~BaseSingleton() {}
        
	protected:
		static C* m_oInstance;
        
	};

	//是隐式地在该编译单元为模板的所有特化提供其静态成员的定义，例如在首次使用 BaseSingleton<int>, BaseSingleton<float>, BaseSingleton<char>...会隐式提供静态成员的定义
	template <typename C>
	C* BaseSingleton< C>::m_oInstance = new C();	//为了线程安全考虑，这里单利是直接创建的，而不是在getInstance的时候去创建

	// 在类外部定义
	template <typename C>
	C* BaseSingleton<C>::getInstance()
	{
		return m_oInstance;
	}


}
