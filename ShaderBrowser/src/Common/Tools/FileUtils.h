#pragma once

#include <vector>
#include <string>
#include "BaseSingleton.h"

namespace common
{
	/************************************************************************/
	/* 目前仅支持使用路径全名，或者在已经添加搜索路径下查找。并且搜索路径格式仅能查找到"搜索路径"+filename的文件。
		例如： res/texture/awesomeface.png
		可查找到文件的使用格式：
			"res/texture/awesomeface.png", " texture/awesomeface.png", 绝对路径
	*/
	/************************************************************************/

	class FileUtils : public BaseSingleton<FileUtils>
	{
	public:
		FileUtils();
		~FileUtils();

	public:
		// 添加搜索路径
		void addSearchPath(const std::string& path);

		// 获取文件绝对路径
		std::string getAbsolutePathForFilename(const std::string& filename);

		// 获取文件的绝对路径 和 该文件所在的目录路径
		std::string getAbsolutePathForFilename(const std::string& filename, std::string& directoryPath);

		// 尝试通过制定目录获取文件的绝对路径  (如材质文件中直接使用 xxx.png 或 ./xxx.png，会先尝试从材质文件所在目录去查找文件是否存在)
		std::string tryGetAbsolutePathForFilename(const std::string& filename, const std::string& directory);

		// 判断路径或者文件是否存在
		bool isDirectoryOrFileExist(const std::string& path);

	private:

		// 相对路径转绝对路径
		std::string convertToAbsolutePath(const std::string& path);

		


	private:
		std::vector<std::string> m_vSearchPath;

	};

}
