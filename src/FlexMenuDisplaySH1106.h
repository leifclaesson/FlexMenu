/*
 * FlexMenuDisplaySH1106.h
 *
 *  Created on: 21 Jun 2020
 *      Author: user
 */

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYSH1106_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYSH1106_H_

#include <Arduino.h>
#include "FlexMenuDisplay.h"
#include "FlexMenuBase.h"

class OLEDDisplay;
class FlexMenuItemEdit;

class FlexMenuDisplay_SH1106 : public FlexMenuDisplay
{
public:
	FlexMenuDisplay_SH1106();
	virtual ~FlexMenuDisplay_SH1106();

	virtual void Init() override;

	virtual int GetVisibleItems() override;

	virtual void DrawDisplay(FlexMenuBase * pCurMenu) override;

	uint8_t * icons[eFlexMenuIcon_Count];

	const uint8_t * pFont=NULL;
	const uint8_t * pFontSlider=NULL;
	const uint8_t * pFontEdit=NULL;

	int iScreenCX=128;
	int iScreenCY=64;
	int iLineHeight=12;
	int iVisibleItems=0;

	int iFontHeightSlider=0;

	int iIconCX=0;
	int iIconCY=0;
	int iIconY=0;

	OLEDDisplay * pDisplay;

	virtual void DrawSliderScreen(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem);



	virtual void DrawEditScreen(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem);

	virtual bool OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel) override;


	uint8_t * osk_table;


	int osk_width;
	int osk_height;
	int num_control_chars;

	int position=0;
	int osk_table_length;

	int scroll_y=0;
	int last_scroll_y=0;

	int iNudge=0;
	int iLastDirection=0;

	bool bCursorMode=false;

	FlexMenuBase * pLastCurItem=0;

	void InitEdit(FlexMenuItemEdit * pItemEdit);
	int iCursor=0;

	String strEdit;

	bool EditNeedsRefresh() override;

	uint32_t cursor_millis=0;

	bool bDrawCursor=false;
	bool bLastDrawCursor=false;

	int iMaxCharsX=0;

	int iScrollX=0;

	int iEditTextX=0;

	void CursorNav(int iDirection);

	void DoScrollKeyboard(int iDirection);

};

#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYSH1106_H_ */
