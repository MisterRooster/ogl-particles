/*------------------------------------------------------------------------------------------------*\
| ogl-particles
|
| Copyright (c) 2023 MisterRooster (github.com/MisterRooster). All rights reserved.
| Licensed under the MIT license. See LICENSE file for full terms.
| This notice is not to be removed.
\*------------------------------------------------------------------------------------------------*/
#pragma once

// icons encoding for icons font:
// https://github.com/Templarian/MaterialDesign-Font/blob/master/MaterialDesignIconsDesktop.ttf

namespace nhahn
{
	#define ICON_MIN_MDI 0xF68C
	#define ICON_MAX_16_MDI 0xF68C
	#define ICON_MAX_MDI 0xF1C80

	#define ICON_MDI_WINDOW_CLOSE "\xf3\xb0\x96\xad"	// U+F05AD
	#define ICON_MDI_WINDOW_MAXIMIZE "\xf3\xb0\x96\xaf"	// U+F05AF
	#define ICON_MDI_WINDOW_MINIMIZE "\xf3\xb0\x96\xb0"	// U+F05B0
	#define ICON_MDI_WINDOW_RESTORE "\xf3\xb0\x96\xb2"	// U+F05B2

	#define ICON_MDI_SHIMMER "\xf3\xb1\x95\x85"	// U+F1545
	#define ICON_MDI_POUND "\xf3\xb0\x90\xa3"	// U+F0423
	#define ICON_MDI_POUND_BOX "\xf3\xb0\x90\xa4"	// U+F0424
	#define ICON_MDI_POUND_BOX_OUTLINE "\xf3\xb1\x85\xbf"	// U+F117F
	#define ICON_MDI_SPEEDOMETER "\xf3\xb0\x93\x85"	// U+F04C5
	#define ICON_MDI_SPEEDOMETER_MEDIUM "\xf3\xb0\xbe\x85"	// U+F0F85
	#define ICON_MDI_SPEEDOMETER_SLOW "\xf3\xb0\xbe\x86"	// U+F0F86

	#define ICON_MDI_MOUSE "\xf3\xb0\x8d\xbd"	// U+F037D
	#define ICON_MDI_MOUSE_MOVE_DOWN "\xf3\xb1\x95\x90"	// U+F1550
	#define ICON_MDI_MOUSE_MOVE_UP "\xf3\xb1\x95\x91"	// U+F1551
	#define ICON_MDI_MOUSE_MOVE_VERTICAL "\xf3\xb1\x95\x92"	// U+F1552
	#define ICON_MDI_MOUSE_OFF "\xf3\xb0\x8d\xbe"	// U+F037E
	#define ICON_MDI_CURSOR_DEFAULT_CLICK "\xf3\xb0\xb3\xbd"	// U+F0CFD
	#define ICON_MDI_CURSOR_DEFAULT_CLICK_OUTLINE "\xf3\xb0\xb3\xbe"	// U+F0CFE

	#define ICON_MDI_ARROW_UP_DOWN "\xf3\xb0\xb9\xb9"	// U+F0E79
	#define ICON_MDI_ARROW_UP_DOWN_BOLD "\xf3\xb0\xb9\xba"	// U+F0E7A
	#define ICON_MDI_ARROW_UP_DOWN_BOLD_OUTLINE "\xf3\xb0\xa7\x88"	// U+F09C8
	#define ICON_MDI_ARROW_LEFT_RIGHT "\xf3\xb0\xb9\xb3"	// U+F0E73
	#define ICON_MDI_ARROW_LEFT_RIGHT_BOLD "\xf3\xb0\xb9\xb4"	// U+F0E74
	#define ICON_MDI_ARROW_LEFT_RIGHT_BOLD_OUTLINE "\xf3\xb0\xa7\x81"	// U+F09C1

	#define ICON_MDI_SWAP_HORIZONTAL "\xf3\xb0\x93\xa1"	// U+F04E1
	#define ICON_MDI_SWAP_HORIZONTAL_BOLD "\xf3\xb0\xaf\x8d"	// U+F0BCD
	#define ICON_MDI_SWAP_HORIZONTAL_CIRCLE "\xf3\xb0\xbf\xa1"	// U+F0FE1
	#define ICON_MDI_SWAP_HORIZONTAL_CIRCLE_OUTLINE "\xf3\xb0\xbf\xa2"	// U+F0FE2
	#define ICON_MDI_SWAP_VERTICAL "\xf3\xb0\x93\xa2"	// U+F04E2
	#define ICON_MDI_SWAP_VERTICAL_BOLD "\xf3\xb0\xaf\x8e"	// U+F0BCE
	#define ICON_MDI_SWAP_VERTICAL_CIRCLE "\xf3\xb0\xbf\xa3"	// U+F0FE3
	#define ICON_MDI_SWAP_VERTICAL_CIRCLE_OUTLINE "\xf3\xb0\xbf\xa4"	// U+F0FE4

	#define ICON_MDI_INFORMATION "\xf3\xb0\x8b\xbc"	// U+F02FC
	#define ICON_MDI_INFORMATION_SYMBOL "\xf3\xb1\xb1\xac"	// U+F1C6C
	#define ICON_MDI_INFORMATION_OUTLINE "\xf3\xb0\x8b\xbd"	// U+F02FD
	#define ICON_MDI_INFORMATION_BOX "\xf3\xb1\xb1\xa5"	// U+F1C65
	#define ICON_MDI_INFORMATION_BOX_OUTLINE "\xf3\xb1\xb1\xa6"	// U+F1C66
	#define ICON_MDI_INFORMATION_SLAB_SYMBOL "\xf3\xb1\xb1\xab"	// U+F1C6B
	#define ICON_MDI_INFORMATION_SLAB_BOX "\xf3\xb1\xb1\xa7"	// U+F1C67
	#define ICON_MDI_INFORMATION_SLAB_BOX_OUTLINE "\xf3\xb1\xb1\xa8"	// U+F1C68
	#define ICON_MDI_INFORMATION_SLAB_CIRCLE "\xf3\xb1\xb1\xa9"	// U+F1C69
	#define ICON_MDI_INFORMATION_SLAB_CIRCLE_OUTLINE "\xf3\xb1\xb1\xaa"	// U+F1C6A
	#define ICON_MDI_INFORMATION_VARIANT "\xf3\xb0\x99\x8e"	// U+F064E
	#define ICON_MDI_INFORMATION_VARIANT_BOX "\xf3\xb1\xb1\xad"	// U+F1C6D
	#define ICON_MDI_INFORMATION_VARIANT_BOX_OUTLINE "\xf3\xb1\xb1\xae"	// U+F1C6E
	#define ICON_MDI_INFORMATION_VARIANT_CIRCLE "\xf3\xb1\xb1\xaf"	// U+F1C6F
	#define ICON_MDI_INFORMATION_VARIANT_CIRCLE_OUTLINE "\xf3\xb1\xb1\xb0"	// U+F1C70
	
	#define ICON_MDI_HELP "\xf3\xb0\x8b\x96"	// U+F02D6
	#define ICON_MDI_HELP_BOX "\xf3\xb0\x9e\x8b"	// U+F078B
	#define ICON_MDI_HELP_BOX_OUTLINE "\xf3\xb1\xb0\x8c"	// U+F1C0C
	#define ICON_MDI_HELP_CIRCLE "\xf3\xb0\x8b\x97"	// U+F02D7
	#define ICON_MDI_HELP_CIRCLE_OUTLINE "\xf3\xb0\x98\xa5"	// U+F0625
	#define ICON_MDI_HELP_RHOMBUS "\xf3\xb0\xae\xa5"	// U+F0BA5
	#define ICON_MDI_HELP_RHOMBUS_OUTLINE "\xf3\xb0\xae\xa6"	// U+F0BA6
	
	#define ICON_MDI_TOOLTIP_EDIT "\xf3\xb0\x94\xa4"	// U+F0524
	#define ICON_MDI_TOOLTIP_EDIT_OUTLINE "\xf3\xb1\x8b\x85"	// U+F12C5
	#define ICON_MDI_TOOLTIP_IMAGE "\xf3\xb0\x94\xa5"	// U+F0525
	#define ICON_MDI_TOOLTIP_IMAGE_OUTLINE "\xf3\xb0\xaf\x95"	// U+F0BD5
	#define ICON_MDI_TOOLTIP_QUESTION "\xf3\xb1\xae\xba"	// U+F1BBA
	#define ICON_MDI_TOOLTIP_QUESTION_OUTLINE "\xf3\xb1\xae\xbb"	// U+F1BBB
	#define ICON_MDI_TOOLTIP_TEXT "\xf3\xb0\x94\xa8"	// U+F0528
	#define ICON_MDI_TOOLTIP_TEXT_OUTLINE "\xf3\xb0\xaf\x97"	// U+F0BD7
}
