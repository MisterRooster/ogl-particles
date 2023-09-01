/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

#include "imgui.h"


namespace ImGui
{
	static void HelpMarker(const char* desc);

	static void PaddedText(const char* str, ImVec2 padding, ImVec4 col = ImVec4(1, 1, 1, 1));
}