/*
 * FlexMenuDisplaySH1106.h
 *
 *  Created on: 21 Jun 2020
 *      Author: user
 */

#ifndef LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYOLEDDISPLAY_H_
#define LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYOLEDDISPLAY_H_

#include <Arduino.h>
#include "FlexMenuDisplay.h"
#include "FlexMenuBase.h"
#include "FlexMenuEditScreen.h"

class OLEDDisplay;
class FlexMenuItemEdit;


class FlexMenuDisplay_OLEDDisplay_Params
{
public:
	FlexMenuDisplay_OLEDDisplay_Params()
	{
		memset(icons,0,sizeof(icons));
	}


	uint8_t * icons[eFlexMenuIcon_Count];

	const uint8_t * pFont=NULL;

	int iScreenCX=128;
	int iScreenCY=64;
	int iLineHeight=12;

	const uint8_t * pFontSlider=NULL;
	int iFontHeightSlider=0;

	int iIconCX=0;
	int iIconCY=0;
	int iIconY=0;

	OLEDDisplay * pOLEDDisplay;

};

class FlexMenuDisplay_OLEDDisplay : public FlexMenuDisplay, public FlexMenuEditScreen
{
public:
	FlexMenuDisplay_OLEDDisplay();
	virtual ~FlexMenuDisplay_OLEDDisplay();

	virtual void SetParams(FlexMenuDisplay_OLEDDisplay_Params * pParams);

protected:

	FlexMenuDisplay_OLEDDisplay_Params params;

	friend class FlexMenuManager;

	virtual void Init() override;

	virtual int GetVisibleItems() override;

	virtual void DrawDisplay(FlexMenuBase * pCurMenu) override;

	virtual void DrawSliderScreen(FlexMenuBase * pCurMenu,FlexMenuBase * pCurItem);

	uint16_t getCharsForWidth(const uint8_t * pFont,const char* text, uint16_t length, uint16_t desiredWidth);

	virtual bool OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel) override;	//return true to leave

	void OnEditMode(FlexMenuBase * pCurMenu, bool bEnable) override;

	virtual bool DisplayNeedsRefresh(FlexMenuBase * pCurMenu) override;

	int iVisibleItems=0;

};

#endif /* LIBRARIES_FLEXMENU_SRC_FLEXMENUDISPLAYOLEDDISPLAY_H_ */
