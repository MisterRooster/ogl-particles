/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "Utils.h"

#include <stdint.h>
#include <algorithm>


namespace nhahn
{
	// http://www.musicdsp.org/showone.php?id=273
	// fast rand float, using full 32bit precision
	// takes about 12 seconds for 2 billion calls
	static int RandSeed = 1;
	float Utils::Fast_RandFloat()
	{
		RandSeed *= 16807;
		return (float)RandSeed * 4.6566129e-010f;
	}
	
	float Utils::frand()
	{
		static uint32_t seed = 0xBA5EBA11;
		seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;

		union { uint32_t i; float f; } union_hack;

		union_hack.i = (seed >> 8) | 0x3F800000;
		return union_hack.f - 1.0f;
	}
	
	// http://www.rgba.org/articles/sfrand/sfrand.htm
	static unsigned int mirand = 1;
	float Utils::sfrand()
	{
		unsigned int a;
		mirand *= 16807;
		a = (mirand & 0x007fffff) | 0x40000000;
		return(*((float*)&a) - 3.0f);
	}

	int Utils::irand()
	{
		static uint32_t seed = 0xDEADBEEF;
		seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;

		return seed;
	}

	float Utils::smoothStep(float edge0, float edge1, float x)
	{
		x = std::min(std::max((x - edge0) / (edge1 - edge0), 0.0f), 1.0f);

		return x * x * (3.0f - 2.0f * x);
	}

	int Utils::stringHash(const char* str)
	{
		int hash = 5381;

		int c;
		while ((c = *str++))
			hash = ((hash << 5) + hash) + c;

		return hash;
	}

	void Utils::intWrite(void* dest, const int n, const unsigned int value)
	{
		switch (n) {
		case 1:
			((UINT8*)dest)[0] = (UINT8)value;
			break;
		case 2:
			((UINT16*)dest)[0] = (UINT16)value;
			break;
		case 3:
			((UINT8*)dest)[2] = (UINT8)((value >> 16) & 0xFF);
			((UINT8*)dest)[1] = (UINT8)((value >> 8) & 0xFF);
			((UINT8*)dest)[0] = (UINT8)(value & 0xFF);
			break;
		case 4:
			((UINT32*)dest)[0] = (UINT32)value;
			break;
		}
	}

	unsigned int Utils::intRead(const void* src, int n) {
		switch (n) {
		case 1:
			return ((UINT8*)src)[0];
		case 2:
			return ((UINT16*)src)[0];
		case 3:
			return ((UINT32)((UINT8*)src)[0]) |
				((UINT32)((UINT8*)src)[1] << 8) |
				((UINT32)((UINT8*)src)[2] << 16);
		case 4:
			return ((UINT32*)src)[0];
		default:
			return ((UINT8*)src)[0];
		}
	}

	INT32 Utils::hexToInt(std::string shex)
	{
		return std::stoul(shex, nullptr, 16);
	}

	INT32 Utils::hexToInt24(std::string shex)
	{
		std::string inputStr = shex.substr(0, 6);
		std::vector<byte> bytes;
		if (inputStr.empty()) return 0;

		for (size_t i = 0; i < 3; i += 2) {
			const std::string hex = inputStr.substr(i, 2);
			const byte decimal = std::strtol(hex.c_str(), 0, 16);
			bytes.push_back(decimal);
		}
		return -((bytes[2]) << 16 | (bytes[1] & 0xFF) << 8 | (bytes[0] & 0xFF));
	}

	std::string Utils::hexToAscii(std::string sHex)
	{
		std::ostringstream os;
		for (int i = 0; i < sHex.length(); i += 2)
		{
			std::string hex = sHex.substr(i, 2);
			os << std::dec << (char)std::stoul(hex, nullptr, 16);
		}
		return os.str();
	}

	std::vector<std::string> Utils::split(std::string str, std::string delimiter)
	{
		std::vector<std::string> res;
		size_t pos = 0;
		std::string token;
		while ((pos = str.find(delimiter)) != std::string::npos) {
			token = str.substr(0, pos);
			res.push_back(token);
			str.erase(0, pos + delimiter.length());
		}
		return res;
	}
}