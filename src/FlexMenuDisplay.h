#pragma once

#include "FlexMenuBase.h"

class FlexMenuBase;

class FlexMenuDisplay
{
public:

	virtual void Init()=0;

	virtual int GetVisibleItems()=0;

	virtual void DrawDisplay(FlexMenuBase * pCurMenu)=0;

	virtual bool OnNavigate(FlexMenuBase * pCurMenu, eFlexMenuNav direction, uint8_t accel)=0;	//return true to leave

	virtual bool EditNeedsRefresh()=0;



};
