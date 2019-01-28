#include "FileUtils.h"
#ifdef WIN32
    // WIN32
    #include <corecrt_io.h>
    #pragma warning(disable:4996)
#else
    // macOS
    #include <unistd.h>
#endif

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

	void FileUtils::addSearchPath(std::string path)
	{
		
	}

	bool FileUtils::isDirectoryExist(std::string path)
	{
        if (access(path.c_str(), 0) == -1)
        {
            return false;
        }
		return true;
	}

	std::string FileUtils::convertToAbsolutePath(std::string path)
	{
		//filePathbuf变量是保存着相对路径的char型数组,dir用来保存绝对路径
		char dir[MAX_FILE_DIRECTORY_LENGTH] = "";
		//1.转换绝对路径到dir
#ifdef WIN32
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

	//std::string FileUtils::getCurrentDirectory()
	//{

	//}


}
