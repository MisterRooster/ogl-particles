/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#include "CustomWidgets.h"

namespace ImGui
{
	void HelpMarker(const char* desc)
	{
		TextDisabled("(?)");
		if (BeginItemTooltip())
		{
			PushTextWrapPos(GetFontSize() * 35.0f);
			TextUnformatted(desc);
			PopTextWrapPos();
			EndTooltip();
		}
	}

	void PaddedText(const char* str, ImVec2 padding, ImVec4 col)
	{
		ImVec2 sz = CalcTextSize(str);
		ImVec2 last_crs = ImGui::GetCursorScreenPos();
		ImVec2 elem_sz = ImVec2(padding.x * 2 + sz.x, padding.y * 2 + sz.y);

		SetCursorScreenPos(ImVec2(last_crs.x + padding.x, last_crs.y + padding.y));
		TextColored(col, str);
		SetCursorScreenPos(last_crs);
		InvisibleButton("##padded-text", elem_sz);
	}

	void PaddedImage(ImTextureID tex, ImVec2 size, ImVec2 padding, ImVec2 uv0, ImVec2 uv1,
		ImVec4 tint_col, ImVec4 border_col)
	{
		ImVec2 last_crs = ImGui::GetCursorScreenPos();
		ImVec2 elem_sz = ImVec2(padding.x * 2 + size.x, padding.y * 2 + size.y);

		SetCursorScreenPos(ImVec2(last_crs.x + padding.x, last_crs.y + padding.y));
		ImGui::Image(tex, size, uv0, uv1, tint_col, border_col);
		SetCursorScreenPos(last_crs);
		InvisibleButton("##padded-text", elem_sz);
	}
}