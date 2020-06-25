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
#include "FlexMenuEditScreen.h"

class OLEDDisplay;
class FlexMenuItemEdit;

class FlexMenuDisplay_SH1106 : public FlexMenuDisplay, public FlexMenuEditScreen
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

	uint16_t getCharsForWidth(const uint8_t * pFont,const char* text, uint16_t length, uint16_t desiredWidth);

	virtual bool OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel) override;	//return true to leave

	void OnEditMode(FlexMenuBase * pCurMenu, bool bEnable) override;

	virtual bool DisplayNeedsRefresh(FlexMenuBase * pCurMenu) override;

};

#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYSH1106_H_ */
