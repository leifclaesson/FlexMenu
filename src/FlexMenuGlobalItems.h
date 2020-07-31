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
		strTitle="";
		strValue="";
		SetDisplayCentered(false);
	}

	FlexMenuBase * pParent;
	eFlexMenuIcon icon;
	bool bIsLeave;

	String strTitle;
	String strValue;

	virtual void GetTitleText(String & strTitleDestination) { if(pParent) pParent->GetTitleText(strTitleDestination); else strTitleDestination=strTitle; };
	virtual void GetValueText(String & strValueDestination) { if(pParent) pParent->GetValueText(strValueDestination); else strValueDestination=strValue; }

	virtual bool IsLeave() { return bIsLeave; };

	virtual eFlexMenuIcon UseIcon() { return icon; };

	virtual bool DisplayCentered() { return (flags & 0x80)!=0; };
	void SetDisplayCentered(bool bCentered) { if(bCentered) flags |= 0x80; else flags &=(0xFF-0x80); };

};

class FlexMenuItemSpacer :
	public FlexMenuBase
{
public:
	virtual void GetTitleText(String & strTitleDestination) { strTitleDestination=""; };
	virtual bool AllowLand() { return false; };
};

