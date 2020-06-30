// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"


class FlexMenuItemLeave :
	public FlexMenuBase
{
public:
	FlexMenuItemLeave() {};
	~FlexMenuItemLeave() {};

	virtual bool IsLeave() { return true; };

	virtual void GetTitleText(String & strTitleDestination) override { strTitleDestination="Back"; }
	virtual eFlexMenuIcon UseIcon() { return eFlexMenuIcon_LeftArrow; };

};


class FlexMenuTempItem:
	public FlexMenuBase
{
public:
	FlexMenuTempItem() { Reset(); };

	void Reset()
	{
		pParent=NULL;
		icon=eFlexMenuIcon_Blank;
		bIsLeave=false;
	}

	FlexMenuBase * pParent;
	eFlexMenuIcon icon;
	bool bIsLeave;

	virtual void GetTitleText(String & strTitleDestination) { if(pParent) pParent->GetTitleText(strTitleDestination); else strTitleDestination=""; };
	virtual void GetValueText(String & strValueDestination) { if(pParent) pParent->GetValueText(strValueDestination); else strValueDestination=""; }

	virtual bool IsLeave() { return bIsLeave; };

	virtual eFlexMenuIcon UseIcon() { return icon; };

};
