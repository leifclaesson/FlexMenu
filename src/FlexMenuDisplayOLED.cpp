/*
 * FlexMenuDisplayOLED.cpp
 *
 *  Created on: 26 Jun 2020
 *      Author: user
 */

#include <FlexMenuDisplayOLED.h>

#ifdef FLEXMENU_OLED

#include "fonts\Regular_10.h"
#include "fonts\Bold_12.h"
#include "fonts\Monospaced_14.h"

#include "icons\icons.h"


osk_icondef osk_icons[7]=	//must match eFlexMenuEdit.
{
	{0,0,0,0,0},
	{arrows,10,8,0,1},
	{spacebar,10,8,0,2},
	{delete_symbol,12,9,1,0},
	{backspace,12,9,-1,0},
	{cancel,7,6,0,1},
	{okay,8,6,0,1},
};

char PROGMEM _osk_table[]={
		"0123456789_+=*"
		"ABCDEFGHIJKLMN"
		"OPQRSTUVWXYZ.-"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		"abcdefghijklmn"
		"opqrstuvwxyz.-"
		"0123456789-+=*"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		"!?#$%&:;/\\()[]"
		"������������ߵ"
		"<>@'`{|}~^\"_.,"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		};


/*
char PROGMEM _osk_table[]={
		"ABCDEFGHIJKLMN"
		"OPQRSTUVWXYZ.-"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		"abcdefghijklmn"
		"opqrstuvwxyz.-"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		"!?#$%&:;/\\()[]"
		"<>@'`{|}~^\"_.,"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		"0123456789-+=*"
		"������������ߵ"
		"\1\2\3\4\5\6\0\0\0\0\0\0\0\0"
		};
*/


FlexMenuDisplay_OLED::FlexMenuDisplay_OLED()
{

}

FlexMenuDisplay_OLED::~FlexMenuDisplay_OLED()
{
}

void FlexMenuDisplay_OLED::InitResources_MenuIcons()
{

	icons[eFlexMenuIcon_LeftArrow]=leftarrow_10;
	icons[eFlexMenuIcon_RightArrow]=rightarrow_10;
	icons[eFlexMenuIcon_Circle]=circle_10;
	icons[eFlexMenuIcon_Selection]=selection_10;
	icons[eFlexMenuIcon_Checked]=checked_10;
	icons[eFlexMenuIcon_Unchecked]=unchecked_10;
	icons[eFlexMenuIcon_Execute]=execute_10;
	icons[eFlexMenuIcon_Cursor]=cursor_10;

	iIconCX=6;
	iIconCY=11;
	iIconY=0;




}

void FlexMenuDisplay_OLED::InitResources_MenuFonts()
{
	pFont=(const uint8_t *) Regular_10;
	pFontSlider=(const uint8_t *) Bold_12;
	iFontHeightSlider=0;
	iLineHeight=10;

}

void FlexMenuDisplay_OLED::InitResources_Edit(FlexMenuEditScreenParams & ep)
{


	ep.iMaxCharsX=15;

	ep.osk_width=14;
	ep.osk_height=3;

	ep.iEditTextShiftX=3;

	ep.osk_table=(uint8_t *) _osk_table;
	ep.osk_table_length=sizeof(_osk_table)-1;
	ep.num_control_chars=6;

	ep.osk_icons=osk_icons;

	ep.pFontEdit=(const uint8_t *) Monospaced_14;
	ep.iFontHeightEdit=15;

	ep.initial_osk_selection=(ep.osk_width*3)+4;	//start on the X (cancel/exit) key


}

#endif
