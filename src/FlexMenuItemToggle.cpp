// * Copyright 2020 Leif Claesson. Licenced under the GNU GPL Version 3.

#include "stdafx.h"
#include "FlexMenuItemToggle.h"


FlexMenuItemToggle::FlexMenuItemToggle()
{
}


FlexMenuItemToggle::~FlexMenuItemToggle()
{
}

bool FlexMenuItemToggle::GetState()
{
	return (bool) derived_use_1;
}

void FlexMenuItemToggle::SetState(bool bState)
{
	derived_use_1=bState;
}

void FlexMenuItemToggle::GetTitleText(String & strTitleDestination)
{
	strTitleDestination=strTitle;
}

void FlexMenuItemToggle::OnPush()
{
	derived_use_1=derived_use_1==false;
	OnValueChanged();
	SetNeedsRefresh(true);
}

eFlexMenuIcon FlexMenuItemToggle::UseIcon()
{
	return derived_use_1?eFlexMenuIcon_Checked:eFlexMenuIcon_Unchecked;
}

void FlexMenuItemToggle::GetSaveString(String & strSave)
{
	strSave=GetState();
}

bool FlexMenuItemToggle::LoadString(const String & strLoad)
{
	if(strLoad=="1")
	{
		SetState(true);
		return true;
	}
	if(strLoad=="0")
	{
		SetState(false);
		return true;
	}
	return false;
}

