/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "FileSystem.h"
#include "Debug.h"

// Platform defines
#define FS2DGPU_PLATFORM_WIN32 1
#define FS2DGPU_PLATFORM_LINUX 2
#define FS2DGPU_PLATFORM_OSX 3

#if defined( __WIN32__ ) || defined( _WIN32 )
#	define FS2DGPU_PLATFORM FS2DGPU_PLATFORM_WIN32
#elif defined( __APPLE_CC__)
#	define FS2DGPU_PLATFORM FS2DGPU_PLATFORM_OSX
#else
#	define FS2DGPU_PLATFORM FS2DGPU_PLATFORM_LINUX
#endif


#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG 
#include "thirdparty/stb_image.h"


namespace nhahn
{
	std::string FileSystem::_cachedModulePath;

	char* FileSystem::readTextFile(const char* filepath)
	{
		FILE* file = fopen(filepath, "rt");
		if (file == NULL)
			FAIL("Unable to open file '%s'\n", filepath);

		unsigned long size = fileSize(file);
		char* data = new char[size + 1];
		memset(data, 0, size + 1);

		fread(data, 1, size, file);
		data[size] = '\0';
		fclose(file);

		return data;
	}

	void FileSystem::writeTextFile(const char* filepath, char* data, unsigned long size)
	{
		FILE* file = fopen(filepath, "wt");
		if (file == NULL)
			FAIL("Unable to open file '%s'\n", filepath);

		fwrite(data, 1, size, file);
		fclose(file);
	}

	char* FileSystem::readBinaryFile(const char* filepath)
	{
		FILE* file = fopen(filepath, "rb");
		if (file == NULL)
			FAIL("Unable to open file '%s'\n", filepath);

		unsigned long size = fileSize(file);
		char* data = (char*)malloc((size + 1) * sizeof(char));

		fread(data, sizeof(char), size, file);
		data[size] = '\0';
		fclose(file);

		return data;
	}

	void FileSystem::writeBinaryFile(const char* filepath, char* data, unsigned long size)
	{
		FILE* file = fopen(filepath, "wb");
		if (file == NULL)
			FAIL("Unable to open file '%s'\n", filepath);

		fwrite(data, 1, size, file);
		fclose(file);
	}

	unsigned long FileSystem::fileSize(const char* filepath)
	{
		FILE* file = fopen(filepath, "rb");
		if (file == NULL)
			FAIL("Unable to open file '%s'\n", filepath);

		unsigned long size = fileSize(file);
		fclose(file);

		return size;
	}

	unsigned long FileSystem::fileSize(FILE* fp)
	{
		unsigned long prev = ftell(fp);
		fseek(fp, 0, SEEK_END);
		unsigned long size = ftell(fp);
		fseek(fp, prev, SEEK_SET);

		return size;
	}

	unsigned char* FileSystem::loadImageFile(const char* filepath, int* w, int* h,
		int* channels, int desired_channels = 0)
	{
		unsigned char* texData = stbi_load(filepath, w, h, channels, desired_channels);
		if (texData == NULL) {
			FAIL("Unable to load image '%s', Error: %s\n", filepath, stbi_failure_reason());
			return NULL;
		}
		return texData;
	}
}

//--------------------------------------------------------------------------------------------------

#if FS2DGPU_PLATFORM == FS2DGPU_PLATFORM_WIN32
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>


namespace nhahn
{
	time_t FileSystem::fileLastChanged(const char* path)
	{
		HANDLE hFile;

		hFile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			FAIL("CreateFile failed with %d\n", GetLastError());
			return 0;
		}

		FILETIME ftCreate, ftAccess, ftWrite;
		SYSTEMTIME stUTC;

		// Retrieve the file times for the file.
		if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
			return 0;

		CloseHandle(hFile);

		// Convert the last-write time to local time.
		FileTimeToSystemTime(&ftWrite, &stUTC);

		// get local time
		struct tm tm;
		memset(&tm, 0, sizeof(tm));

		tm.tm_year = stUTC.wYear - 1900;
		tm.tm_mon = stUTC.wMonth - 1;
		tm.tm_mday = stUTC.wDay;

		tm.tm_hour = stUTC.wHour;
		tm.tm_min = stUTC.wMinute;
		tm.tm_sec = stUTC.wSecond;
		tm.tm_isdst = -1;

		return mktime(&tm);
	}

	std::string FileSystem::getCurrentDirectory()
	{
		DWORD len = GetCurrentDirectoryA(0, NULL);
		if (len > 0)
		{
			char* buffer = (char*)malloc(len * sizeof(char));

			DWORD n = GetCurrentDirectoryA(len, buffer);
			if (n > 0 && n <= len)
			{
				std::string result(buffer);
				if (result[result.size() - 1] != '\\')
					result.append("\\");

				free(buffer);
				return result;
			}

			free(buffer);
		}
		return "";
	}

	std::string FileSystem::getModuleDirectory()
	{
		if (_cachedModulePath.empty())
		{
			// get module path
			char modPath[MAX_PATH];
			memset(modPath, 0, sizeof(modPath));
			GetModuleFileNameA(0, modPath, sizeof(modPath));
			for (size_t i = strlen(modPath); i > 0; i--)
			{
				if (modPath[i - 1] == '\\')
				{
					modPath[i] = 0;
					break;
				}
			}
			_cachedModulePath = modPath;
		}
		return _cachedModulePath;
	}
}

//--------------------------------------------------------------------------------------------------

#elif FS2DGPU_PLATFORM == FS2DGPU_PLATFORM_LINUX
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace nhahn
{
	unsigned long FileSystem::fileSize(FILE* fp)
	{
		struct stat stat;
		int file = open(path, O_RDONLY);
		if (!file)
			return 0;
		fstat(file, &stat);
		close(file);
	#if __USE_XOPEN2K8 || __MINGW32__ || 1
		return stat.st_mtime;
	#else
		return stat.st_mtimespec.tv_sec;
	#endif
	}

	std::string FileSystem::getCurrentDirectory()
	{
		std::cerr << "This functions isn't implemented for Linux!\n";
		return "";
	}

	std::string FileSystem::getModuleDirectory()
	{
		char szDir[MAX_PATH] = { 0 };

		readlink("/proc/self/exe", szDir, MAX_PATH);
		char* pEnd = strrchr(szDir, '/');
		if (pEnd)
			*pEnd = '\0';

		return std::string(szDir);
	}
}

#else
	std::cerr << "Only Windows and Linux are currently supported!\n";
#endif