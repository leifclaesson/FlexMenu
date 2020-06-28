// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#pragma once
#include "FlexMenuBase.h"


class FlexMenuItemToggle :
	public FlexMenuBase
{
public:
	FlexMenuItemToggle();
	~FlexMenuItemToggle();

	virtual bool GetState();
	virtual void SetState(bool bState);

	virtual void GetTitleText(String & strTitleDestination);
	virtual void OnPush();

	String strTitle;

	virtual eFlexMenuIcon UseIcon();

	virtual void OnValueChanged() {};

private:

};


class FlexMenuItemToggleEx;

enum eFMIToggleCallback
{
	eFMIToggleCallback_ValueChanged,
};

typedef std::function<void(FlexMenuItemToggleEx *,eFMIToggleCallback)> FlexMenuItemToggleExCB;

class FlexMenuItemToggleEx : public FlexMenuItemToggle
{
public:

	virtual void OnValueChanged() override { if(fnCallback) fnCallback(this,eFMIToggleCallback_ValueChanged); }

	FlexMenuItemToggleExCB fnCallback;

};
