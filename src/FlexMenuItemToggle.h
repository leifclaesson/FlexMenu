// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"

class FlexMenuItemToggle;

typedef std::function<void(FlexMenuItemToggle *)> FlexMenuItemToggleCB;


class FlexMenuItemToggle :
	public FlexMenuBase
{
public:
	FlexMenuItemToggle();
	~FlexMenuItemToggle();

	bool bState=false;

	virtual void GetTitleText(String & strTitleDestination);
	virtual void OnPush();

	String strTitle;

	virtual eFlexMenuIcon UseIcon();

	void SetCallback(FlexMenuItemToggleCB & pCallback);

private:
	FlexMenuItemToggleCB * pfnCallback=NULL;

};

