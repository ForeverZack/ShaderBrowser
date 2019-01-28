#pragma once

#include <vector>
#include <string>
#include "BaseSingleton.h"

namespace common
{
	class FileUtils : public BaseSingleton<FileUtils>
	{
	public:
		FileUtils();
		~FileUtils();

	public:
		// 添加搜索路径
		void addSearchPath(std::string path);

		// 相对路径转绝对路径
		std::string convertToAbsolutePath(std::string path);

		// 判断路径是否存在
		bool isDirectoryExist(std::string path);

	private:
		// 获取当前路径
		//std::string getCurrentDirectory();


	private:
		std::vector<std::string> m_vSearchPath;

	};

}
