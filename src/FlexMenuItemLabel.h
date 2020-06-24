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
