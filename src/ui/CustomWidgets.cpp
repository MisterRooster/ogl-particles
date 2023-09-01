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
		ImVec2 cursor = GetCursorPos();

		InvisibleButton("##padded-text", ImVec2(padding.x * 2 + sz.x, padding.y * 2 + sz.y));
		ImVec2 final_cursor_pos = GetCursorPos();

		SetCursorPos(ImVec2(cursor.x + padding.x, cursor.y + padding.y));
		TextColored(col, str);
		SetCursorPos(final_cursor_pos);
	}
}