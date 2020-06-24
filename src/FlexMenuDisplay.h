#pragma once

class FlexMenuBase;

class FlexMenuDisplay
{
public:

	virtual void Init()=0;

	virtual int GetVisibleItems()=0;

	virtual void DrawDisplay(FlexMenuBase * pCurMenu)=0;

};
