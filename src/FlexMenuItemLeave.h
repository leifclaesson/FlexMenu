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

