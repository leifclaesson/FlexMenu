// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAY_OLED_BASE_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAY_OLED_BASE_H_

#include <Arduino.h>
#include "FlexMenuDisplay.h"
#include "FlexMenuBase.h"
#include "FlexMenuEditScreen.h"

class OLEDDisplay;
class FlexMenuItemEdit;


class FlexMenuDisplay_OLED_Base_Params
{
public:
	FlexMenuDisplay_OLED_Base_Params()
	{
	}

	int iScreenCX=128;
	int iScreenCY=64;

	OLEDDisplay * pOLEDDisplay;

};

class FlexMenuDisplay_OLED_Base : public FlexMenuDisplay, public FlexMenuEditScreen
{
public:
	FlexMenuDisplay_OLED_Base();
	virtual ~FlexMenuDisplay_OLED_Base();

	virtual void SetParams(FlexMenuDisplay_OLED_Base_Params * pParams);

protected:

	FlexMenuDisplay_OLED_Base_Params params;

	virtual void InitResources_MenuIcons()=0;
	virtual void InitResources_MenuFonts()=0;
	virtual void InitResources_Edit(FlexMenuEditScreenParams & ep)=0;


	uint8_t * icons[eFlexMenuIcon_Count];

	const uint8_t * pFont=NULL;

	int iLineHeight=12;

	const uint8_t * pFontSlider=NULL;
	int iFontHeightSlider=0;

	int iIconCX=0;
	int iIconCY=0;
	int iIconY=0;


	friend class FlexMenuManager;

	virtual void Init() override;

	virtual int GetVisibleItems() override;

	virtual void DrawDisplay(FlexMenuBase * pCurMenu) override;

	virtual void DrawSliderScreen(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem);

	uint16_t getCharsForWidth(const uint8_t * pFont,const char* text, uint16_t length, uint16_t desiredWidth);

	virtual bool OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel) override;	//return true to leave

	void OnEditMode(FlexMenuBase * pCurMenu, bool bEnable) override;
	virtual void ESCB_DrawEditBox(const FlexMenuEditScreenParams & editparams, const String & strDisplayText, const String & strBeforeCursor, bool bDrawCursor) override;

	virtual void ESCB_DrawOSK_Key(uint16_t x, uint16_t y, uint16_t cx, uint16_t cy, eEditOskDrawKey mode, const osk_icondef * pIcon, const String * pText) override;

	virtual bool DisplayNeedsRefresh(FlexMenuBase * pCurMenu) override;

	int iVisibleItems=0;

	virtual void DrawMessage(FlexMenuBase * pCurMenu);

};

#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAY_OLED_BASE_H_ */
