#pragma once
#include "FlexMenuBase.h"
class FlexMenuItemSpacer :
	public FlexMenuBase
{
public:
	FlexMenuItemSpacer();
	~FlexMenuItemSpacer();
	virtual void GetTitleText(String & strTitleDestination) { strTitleDestination=""; };
	virtual bool AllowLand() { return false; };
};

