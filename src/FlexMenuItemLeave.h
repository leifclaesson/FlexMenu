// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"
class FlexMenuItemLeave :
	public FlexMenuBase
{
public:
	FlexMenuItemLeave();
	~FlexMenuItemLeave();

	virtual bool IsLeave() { return true; };

	virtual void GetTitleText(String & strTitleDestination) override;
	virtual eFlexMenuIcon UseIcon() { return eFlexMenuIcon_LeftArrow; };

};

