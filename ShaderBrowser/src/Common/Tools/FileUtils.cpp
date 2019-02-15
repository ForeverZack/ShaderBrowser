#include "FileUtils.h"
#ifdef _WIN32
    // WIN32
    #include <corecrt_io.h>
    #pragma warning(disable:4996)
#else
    // macOS
    #include <unistd.h>
#endif
#include "Utils.h"

namespace common
{
#define MAX_FILE_DIRECTORY_LENGTH 1024

	FileUtils::FileUtils()
	{
		// 清理
		m_vSearchPath.clear();
	}

	FileUtils::~FileUtils()
	{
	}

	void FileUtils::addSearchPath(const std::string& path)
	{
		// 绝对路径
		std::string absolutePath = convertToAbsolutePath(path);

		for (auto itor = m_vSearchPath.cbegin(); itor != m_vSearchPath.cend(); ++itor)
		{
			if ((*itor) == absolutePath)
			{
				BROWSER_LOG("path has already been added into serch path vector in function FileUtils::addSearchPath");
				return;
			}
		}

		m_vSearchPath.push_back(absolutePath);
	}

	std::string FileUtils::getAbsolutePathForFilename(const std::string& filename)
	{
		std::string file_path = convertToAbsolutePath(filename);
		if (isDirectoryOrFileExist(file_path))
		{
			return file_path;
		}

		for (int i = 0; i < m_vSearchPath.size(); ++i)
		{
			const std::string& search_path = m_vSearchPath[i];
			file_path = convertToAbsolutePath(search_path + "/" + filename);
			if (isDirectoryOrFileExist(file_path))
			{
				return file_path;
			}
		}

		// 文件没有找到
		return "";
	}

	std::string FileUtils::getAbsolutePathForFilename(const std::string& filename, std::string& directoryPath)
	{
		std::string file_path = getAbsolutePathForFilename(filename);
		if (file_path != "")
		{
			size_t pos = file_path.find_last_of("/");
			directoryPath = file_path.substr(0, pos+1);
		}
		else
		{
			directoryPath = "";
		}

		return file_path;
	}

	bool FileUtils::isDirectoryOrFileExist(const std::string& path)
	{
        if (access(path.c_str(), 0) == -1)
        {
            return false;
        }
		return true;
	}

	std::string FileUtils::convertToAbsolutePath(const std::string& path)
	{
		//filePathbuf变量是保存着相对路径的char型数组,dir用来保存绝对路径
		char dir[MAX_FILE_DIRECTORY_LENGTH] = "";
		//1.转换绝对路径到dir
#ifdef _WIN32
        // WIN32
		_fullpath(dir, path.c_str(), MAX_FILE_DIRECTORY_LENGTH);
#else
        // macOS
		realpath(path.c_str(), dir);
#endif
		//2.替换绝对路径中的'\'为'/'
		//因为上述方法转换出来的绝对路径之间会以'\\'分隔，例如“C:\user\desktop”,字符串处理遇到'\'一般会报错，下面一行代码将'\'全部替换为'/'
		for (int i = 0; dir[i] != 0 && i < MAX_FILE_DIRECTORY_LENGTH; i++)
		{
			if (dir[i] == '\\')
			{
				dir[i] = '/';
			}
		}

		return std::string(dir);
	}



}
