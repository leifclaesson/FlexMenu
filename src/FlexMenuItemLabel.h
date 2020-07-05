// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"

class FlexMenuItemLabel :
	public FlexMenuBase
{
public:
	FlexMenuItemLabel();
	~FlexMenuItemLabel();

	// Inherited via FlexMenuBase
	virtual void GetTitleText(String & strTitleDestination) override;
	virtual void GetValueText(String & strValueDestination) override;

	String strTitle;
	String strValue;

	virtual void UpdateTitle(String & strTitle);
	virtual void UpdateValue(String & strValue);

	bool GetDisplayCentered() { return (flags & 0x80)!=0; };
	void SetDisplayCentered(bool bDisplayCentered) { if(bDisplayCentered) flags |= 0x80; else flags &=(0xFF-0x80); };

	virtual bool DisplayCentered() { return GetDisplayCentered(); };

};


class FlexMenuItemLabelEx;

typedef std::function<void(FlexMenuItemLabelEx *)> FlexMenuItemLabelExCB;

class FlexMenuItemLabelEx : public FlexMenuItemLabel
{
public:
	FlexMenuItemLabelExCB cbOnVisiblilityChange;
protected:
	virtual void OnVisibilityChange() override;
};
