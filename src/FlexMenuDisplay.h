// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.
#pragma once

#include "FlexMenuBase.h"

class FlexMenuBase;

#if defined(FLEXMENU_U8G2)
#elif defined(FLEXMENU_OLED)
#else
#define FLEXMENU_OLED
#endif

typedef std::function<void(void)> fn_FlexMenuDisplayInterimCallback;

class FlexMenuDisplay
{
public:

	virtual void Init()=0;

	virtual int GetVisibleItems()=0;

	virtual void DrawScreen(FlexMenuBase * pCurMenu)=0;

	virtual bool OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)=0;	//return true to leave

	virtual void OnEditMode(FlexMenuBase * pCurMenu, bool bEnable)=0;

	virtual bool DisplayNeedsRefresh(FlexMenuBase * pCurMenu)=0;

	virtual void Output()=0;

	virtual void SetBacklight(uint8_t brightness, bool bMute)=0;

	virtual bool HistoryBuffer(FlexMenuBase * pCurMenu, uintptr_t * data) { (void)(pCurMenu); (void)(data); return false; };
	// The HistoryBuffer is used to prevent accidentally selecting an unintended value as the user pushes the rotary encoder.
	// HistoryBuffer() is called by FlexMenuManager at regular intervals, pointing to a value in the circular buffer.
	// This circular buffer is initialized to 0xFF every time the user navigates in or out of a menu.
	// Read the old value from the data pointer, then store the current value in the same slot.
	// Return true if using, false otherwise. If false is returned, then FlexMenuBase::HistoryBuffer() is called for the current menu.
	// The reason for FlexMenuDisplay having access to the HistoryBuffer, as opposed to just the current menu,
	// is FlexMenuEditScreen (the on-screen keyboard).

	virtual int GetStringWidth(const String & strInput, eFlexMenuFont font=eFlexMenuFont_Normal)=0;
	virtual int GetCharsForWidth(const String & strInput, int width_pixels, eFlexMenuFont font=eFlexMenuFont_Normal)=0;

	virtual int GetWidth()=0;

	fn_FlexMenuDisplayInterimCallback interimCallback;

};
