// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.
#pragma once

#include "FlexMenuBase.h"

class FlexMenuBase;

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

	virtual bool HistoryBuffer(FlexMenuBase * pCurMenu, uintptr_t * data) { (void)(pCurMenu); (void)(data); return false; };
	// The HistoryBuffer is used to prevent accidentally selecting an unintended value as the user pushes the rotary encoder.
	// HistoryBuffer() is called by FlexMenuManager at regular intervals, pointing to a value in the circular buffer.
	// This circular buffer is initialized to 0xFF every time the user navigates in or out of a menu.
	// Read the old value from the data pointer, then store the current value in the same slot.
	// Return true if using, false otherwise. If false is returned, then FlexMenuBase::HistoryBuffer() is called for the current menu.
	// The reason for FlexMenuDisplay having access to the HistoryBuffer, as opposed to just the current menu,
	// is FlexMenuEditScreen (the on-screen keyboard).


};
