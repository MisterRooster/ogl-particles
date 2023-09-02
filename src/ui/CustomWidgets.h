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
	void HelpMarker(const char* desc);

	void PaddedText(const char* str, ImVec2 padding, ImVec4 col = ImVec4(1, 1, 1, 1));

	void PaddedImage(ImTextureID tex, ImVec2 size, ImVec2 padding,
		ImVec2 uv0 = ImVec2(0,0), ImVec2 uv1 = ImVec2(1, 1),
		ImVec4 tint_col = ImVec4(1, 1, 1, 1), ImVec4 border_col = ImVec4(0, 0, 0, 0));
}