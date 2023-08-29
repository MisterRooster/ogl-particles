/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <time.h>
#include <string>


namespace nhahn
{
	class FileSystem
	{
	public:
		static char* readTextFile(const char* filepath);
		static void writeTextFile(const char* filepath, char* data, unsigned long size);
		static char* readBinaryFile(const char* filepath);
		static void writeBinaryFile(const char* filepath, char* data, unsigned long size);

		static unsigned char* FileSystem::loadImageFile(const char* filepath, int* w, int* h,
			int* channels, int desired_channels);

		static time_t fileLastChanged(const char* path);
		static unsigned long fileSize(const char* filepath);
		static unsigned long fileSize(FILE* fp);

		static std::string getCurrentDirectory();
		static std::string getModuleDirectory();
	private:
		static std::string _cachedModulePath;
	};
}