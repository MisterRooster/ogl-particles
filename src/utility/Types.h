/*------------------------------------------------------------------------------------------------*\
| verlet_basic
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <chrono>
//#include <thread>


/**	Commonly used types. */
namespace nhahn
{
	using std::int8_t;
	using std::uint8_t;
	using std::int16_t;
	using std::uint16_t;
	using std::int32_t;
	using std::uint32_t;
	using std::int64_t;
	using std::uint64_t;

	using INT8 = int8_t;
	using UINT8 = uint8_t;
	using INT16 = int16_t;
	using UINT16 = uint16_t;
	using INT32 = int32_t;
	using UINT32 = uint32_t;
	using INT64 = int64_t;
	using UINT64 = uint64_t;

	using byte = UINT8;
}