#pragma once

#ifdef FLEXMENU_U8G2

#include <Arduino.h>
#include "FlexMenuDisplay.h"
#include "FlexMenuBase.h"
#include "FlexMenuEditScreen.h"



class U8G2;

class FlexMenuDisplay_U8G2_Callback
{
public:

	virtual bool OverrideDrawScreen(FlexMenuBase * pCurMenu) { return false; };

};


class FlexMenuDisplay_U8G2_Params
{
public:
	FlexMenuDisplay_U8G2_Params()
	{
	}

	int iScreenCX=128;
	int iScreenCY=128;

	U8G2 * pU8G2=nullptr;

	FlexMenuDisplay_U8G2_Callback * pCallback=nullptr;

};

void drawStrWordWrap(U8G2 * pU8G2, int16_t x, int16_t y, uint16_t xmax_pixels, uint16_t max_lines, const char * text, int align);

class FlexMenuDisplay_U8G2 : public FlexMenuDisplay, public FlexMenuEditScreen
{
public:
	FlexMenuDisplay_U8G2();

	virtual void SetParams(FlexMenuDisplay_U8G2_Params * pParams);

protected:

	FlexMenuDisplay_U8G2_Params params;

	void InitResources_MenuIcons();
	void InitResources_MenuFonts();
	void InitResources_Edit(FlexMenuEditScreenParams & ep);

	virtual bool HistoryBuffer(FlexMenuBase * pCurMenu, uintptr_t * data) override;



	uint8_t * icons[eFlexMenuIcon_Count];

	const uint8_t * pFont=NULL;

	int iLineHeight=12;

	const uint8_t * pFontSlider=NULL;
	int iFontHeightSlider=0;

	int iFontShiftY=0;

	int iIconCX=0;
	int iIconCY=0;
	int iIconY=0;

	friend class FlexMenuManager;

	virtual void Init() override;

	virtual int GetVisibleItems() override;

	virtual void DrawScreen(FlexMenuBase * pCurMenu) override;

	virtual void DrawSliderScreen(FlexMenuBase * pCurMenu);

	uint16_t getCharsForWidth(const uint8_t * pFont,const char* text, uint16_t length, uint16_t desiredWidth);

	virtual bool OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel) override;	//return true to leave

	void OnEditMode(FlexMenuBase * pCurMenu, bool bEnable) override;
	virtual void ESCB_DrawEditBox(const FlexMenuEditScreenParams & editparams, const String & strDisplayText, const String & strBeforeCursor, bool bDrawCursor) override;

	virtual void ESCB_DrawOSK_Key(const FlexMenuEditScreenParams & editparams, uint16_t x, uint16_t y, uint16_t cx, uint16_t cy, eEditOskDrawKey mode, const osk_icondef * pIcon, const String * pText) override;

	virtual bool DisplayNeedsRefresh(FlexMenuBase * pCurMenu) override;

	virtual int GetStringWidth(const String & strInput, eFlexMenuFont font=eFlexMenuFont_Normal) override;
	virtual int GetCharsForWidth(const String & strInput, int width_pixels, eFlexMenuFont font=eFlexMenuFont_Normal) override;

	virtual int GetWidth() override;

	virtual void SetBacklight(uint8_t brightness, bool bMute) override;

	int iVisibleItems=0;

	virtual void DrawMessage(FlexMenuBase * pCurMenu);

	virtual void Output() override;

	const uint8_t * GetFont(eFlexMenuFont font);

	bool bDisplayMute=false;

};





#endif
