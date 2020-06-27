// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

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

